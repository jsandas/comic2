#include "comic2/resource_loader.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "comic2/bootstrap.hpp"
#include "comic2/room_loader.hpp"

namespace comic2 {
namespace {

constexpr std::size_t kMaxDecodedBytes = 0x10000;

std::uint16_t read_u16(std::span<const std::uint8_t> bytes, std::size_t off) {
  if (off + 1 >= bytes.size()) {
    throw std::runtime_error("read_u16 out of range");
  }
  return static_cast<std::uint16_t>(
      bytes[off] | (static_cast<std::uint16_t>(bytes[off + 1]) << 8));
}

bool path_exists(const std::filesystem::path &path) {
  return std::filesystem::exists(path) &&
         std::filesystem::is_regular_file(path);
}

bool is_room_payload_candidate_name(std::string name) {
  std::transform(name.begin(), name.end(), name.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::toupper(c));
                 });

  if (name.size() != 7 || name.rfind("FR", 0) != 0 || name[5] != '.') {
    return false;
  }

  const bool room_index_is_digits = std::isdigit(name[2]) &&
                                    std::isdigit(name[3]) &&
                                    std::isdigit(name[4]);
  const bool variant_is_digit = std::isdigit(name[6]);
  return room_index_is_digits && variant_is_digit;
}

std::vector<std::filesystem::path>
discover_room_payload_candidates(const std::filesystem::path &root) {
  std::vector<std::filesystem::path> candidates;

  std::error_code ec;
  if (!std::filesystem::exists(root, ec) ||
      !std::filesystem::is_directory(root, ec)) {
    return candidates;
  }

  for (const auto &entry : std::filesystem::directory_iterator(root, ec)) {
    if (ec || !entry.is_regular_file()) {
      continue;
    }

    const std::string name = entry.path().filename().string();
    if (is_room_payload_candidate_name(name)) {
      candidates.push_back(entry.path());
    }
  }

  std::sort(candidates.begin(), candidates.end());
  return candidates;
}

} // namespace

std::optional<std::vector<std::uint8_t>>
load_file_bytes(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    return std::nullopt;
  }

  input.seekg(0, std::ios::end);
  const auto end_pos = input.tellg();
  if (end_pos < 0) {
    return std::nullopt;
  }
  const auto size = static_cast<std::size_t>(end_pos);
  input.seekg(0, std::ios::beg);
  if (!input) {
    return std::nullopt;
  }

  std::vector<std::uint8_t> bytes(size);
  if (size > 0) {
    input.read(reinterpret_cast<char *>(bytes.data()),
               static_cast<std::streamsize>(size));
    if (!input) {
      return std::nullopt;
    }
  }
  return bytes;
}

SignedRleResult decode_signed_rle(std::span<const std::uint8_t> encoded) {
  SignedRleResult out;
  std::size_t i = 0;
  while (i < encoded.size()) {
    const auto control = static_cast<std::int8_t>(encoded[i++]);
    if (control == 0) {
      out.consumed = i;
      return out;
    }

    if (control < 0) {
      if (i >= encoded.size()) {
        throw std::runtime_error("signed-rle truncated run value");
      }
      const auto value = encoded[i++];
      const auto count = static_cast<std::size_t>(-control);
      if (out.bytes.size() > kMaxDecodedBytes - count) {
        throw std::runtime_error("signed-rle decoded output too large");
      }
      out.bytes.insert(out.bytes.end(), count, value);
      continue;
    }

    const auto count = static_cast<std::size_t>(control);
    if (i + count > encoded.size()) {
      throw std::runtime_error("signed-rle truncated literal");
    }
    if (out.bytes.size() > kMaxDecodedBytes - count) {
      throw std::runtime_error("signed-rle decoded output too large");
    }
    out.bytes.insert(out.bytes.end(),
                     encoded.begin() + static_cast<std::ptrdiff_t>(i),
                     encoded.begin() + static_cast<std::ptrdiff_t>(i + count));
    i += count;
  }

  throw std::runtime_error("signed-rle missing terminator");
}

Ega4PlaneImage decode_ega_4plane_rle(std::span<const std::uint8_t> encoded,
                                     bool require_full_consumption) {
  if (encoded.size() < sizeof(EgaRle4PlaneHeader)) {
    throw std::runtime_error("ega-rle data too small");
  }

  Ega4PlaneImage image;
  image.row_span_bytes = read_u16(encoded, 0);
  if (image.row_span_bytes == 0) {
    throw std::runtime_error("ega-rle row span cannot be zero");
  }
  // row_span_bytes is u16 (max 65535), so it can never exceed kMaxDecodedBytes
  // (65536). Check for unreasonable values with a u16-safe threshold.
  constexpr std::uint16_t kMaxRowSpan =
      0x7FFF; // 32768 - reasonable upper bound
  if (image.row_span_bytes > kMaxRowSpan) {
    throw std::runtime_error("ega-rle row span too large");
  }

  std::size_t i = sizeof(EgaRle4PlaneHeader);
  for (std::size_t plane = 0; plane < image.planes.size(); ++plane) {
    auto &out = image.planes[plane];
    out.reserve(image.row_span_bytes);
    while (out.size() < image.row_span_bytes) {
      if (i >= encoded.size()) {
        throw std::runtime_error("ega-rle truncated packet stream");
      }

      const std::uint8_t packet = encoded[i++];
      if ((packet & 0x80U) == 0) {
        const auto count = static_cast<std::size_t>(packet);
        if (i + count > encoded.size()) {
          throw std::runtime_error("ega-rle truncated literal packet");
        }
        if (out.size() > kMaxDecodedBytes - count) {
          throw std::runtime_error("ega-rle decoded output too large");
        }
        out.insert(out.end(), encoded.begin() + static_cast<std::ptrdiff_t>(i),
                   encoded.begin() + static_cast<std::ptrdiff_t>(i + count));
        i += count;
      } else {
        const auto count = static_cast<std::size_t>(packet & 0x7FU);
        if (i >= encoded.size()) {
          throw std::runtime_error("ega-rle truncated run packet");
        }
        const auto value = encoded[i++];
        if (out.size() > kMaxDecodedBytes - count) {
          throw std::runtime_error("ega-rle decoded output too large");
        }
        out.insert(out.end(), count, value);
      }

      if (out.size() > image.row_span_bytes) {
        throw std::runtime_error("ega-rle plane overrun");
      }
    }
  }

  if (require_full_consumption && i != encoded.size()) {
    throw std::runtime_error("ega-rle trailing bytes after 4-plane decode");
  }

  return image;
}

BootstrapLoadSummary
load_initial_bootstrap_resources(RuntimeState &state,
                                 const std::filesystem::path &root) {
  BootstrapLoadSummary summary{};

  const std::array metadata_candidates = {
      root / "FRDATA.0",
      root / "FRDATA.1",
  };
  const std::array sprite_candidates = {
      root / "FRPAK.001", root / "FRPAK.002", root / "FRPAK.003",
      root / "FRPAK.004", root / "FRPAK.005", root / "FRPAK.006",
      root / "FRPAK.007",
  };

  for (const auto &candidate : metadata_candidates) {
    ++summary.metadata_files_tried;
    if (!path_exists(candidate)) {
      continue;
    }

    try {
      const auto bytes = load_file_bytes(candidate);
      if (!bytes.has_value()) {
        continue;
      }

      state.level_metadata_bytes = *bytes;
      state.room_resource_bytes = *bytes;
      // The FRDATA payload feeds both the metadata snapshot and the room
      // resource bytes in this bootstrap path.
      if (load_room_tilemap_from_resource_file(state, candidate, *bytes, 0,
                                               0)) {
        summary.room_grid_loaded = true;
        break;
      }
    } catch (const std::exception &) {
      // Keep the bootstrap non-fatal and fall through to the next candidate.
    }
  }

  if (!summary.room_grid_loaded) {
    const auto room_candidates = discover_room_payload_candidates(root);
    for (const auto &candidate : room_candidates) {
      try {
        const auto bytes = load_file_bytes(candidate);
        if (!bytes.has_value()) {
          continue;
        }

        if (load_room_tilemap_from_resource_file(state, candidate, *bytes, 0,
                                                 0)) {
          summary.room_grid_loaded = true;
          break;
        }
      } catch (const std::exception &) {
        // Keep the bootstrap tolerant of partial room-table payloads.
      }
    }
  }

  for (const auto &candidate : sprite_candidates) {
    ++summary.sprite_files_tried;
    if (!path_exists(candidate)) {
      continue;
    }

    try {
      const auto bytes = load_file_bytes(candidate);
      if (!bytes.has_value()) {
        continue;
      }

      state.sprite_resource_bytes.insert(state.sprite_resource_bytes.end(),
                                         bytes->begin(), bytes->end());
    } catch (const std::exception &) {
      // Missing or unreadable FRPAK payloads should not stop bootstrap.
    }
  }

  return summary;
}

} // namespace comic2
