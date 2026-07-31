#include "comic2/resource_loader.hpp"

#include <array>
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
constexpr std::uint16_t kFrpakMinPakId = 1;
constexpr std::uint16_t kFrpakMaxPakId = 999;

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

std::optional<std::uint16_t>
parse_frpak_id_from_path(const std::filesystem::path &path) {
  const std::string ext = path.extension().string();
  if (ext.size() != 4 || ext[0] != '.') {
    return std::nullopt;
  }

  if (ext[1] < '0' || ext[1] > '9' || ext[2] < '0' || ext[2] > '9' ||
      ext[3] < '0' || ext[3] > '9') {
    return std::nullopt;
  }

  const auto pak_id = static_cast<std::uint16_t>(
      (ext[1] - '0') * 100 + (ext[2] - '0') * 10 + (ext[3] - '0'));
  if (pak_id < kFrpakMinPakId || pak_id > kFrpakMaxPakId) {
    return std::nullopt;
  }

  return pak_id;
}

const FrpakCatalogFile *find_frpak_catalog_file(const FrpakCatalog &catalog,
                                                std::uint16_t pak_id) {
  for (const auto &file : catalog.files) {
    if (file.pak_id == pak_id) {
      return &file;
    }
  }
  return nullptr;
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

std::optional<FrpakCatalogFile>
build_frpak_catalog_file(const std::filesystem::path &source_path,
                         std::span<const std::uint8_t> bytes,
                         std::uint16_t pak_id) {
  if (bytes.size() < sizeof(EgaRle4PlaneHeader)) {
    return std::nullopt;
  }

  const std::uint16_t row_span = read_u16(bytes, 0);
  if (row_span == 0) {
    return std::nullopt;
  }
  constexpr std::uint16_t kMaxRowSpan = 0x7FFF;
  if (row_span > kMaxRowSpan) {
    return std::nullopt;
  }

  FrpakCatalogFile file;
  file.pak_id = pak_id;
  file.source_path = source_path;
  file.file_size = bytes.size();
  file.records.push_back(FrpakCatalogRecord{
      .pak_id = pak_id,
      .record_id = 0,
      .data_offset = 0,
      .data_size = bytes.size(),
      .row_span_bytes = row_span,
  });
  return file;
}

std::optional<FrpakCatalogRecord>
find_frpak_catalog_record(const FrpakCatalog &catalog, std::uint16_t pak_id,
                          std::uint16_t record_id) {
  for (const auto &file : catalog.files) {
    if (file.pak_id != pak_id) {
      continue;
    }

    for (const auto &record : file.records) {
      if (record.record_id == record_id) {
        return record;
      }
    }
  }

  return std::nullopt;
}

bool validate_frpak_catalog_record_bounds(const FrpakCatalogRecord &record,
                                          std::size_t file_size) {
  if (record.data_offset > file_size) {
    return false;
  }
  return record.data_size <= (file_size - record.data_offset);
}

std::optional<Ega4PlaneImage>
decode_frpak_catalog_record(const RuntimeState &state,
                            const FrpakCatalogRecord &record) {
  const FrpakCatalogFile *file =
      find_frpak_catalog_file(state.frpak_catalog, record.pak_id);
  if (file == nullptr) {
    return std::nullopt;
  }
  if (!validate_frpak_catalog_record_bounds(record, file->file_size)) {
    return std::nullopt;
  }
  if (file->blob_offset > state.sprite_resource_bytes.size()) {
    return std::nullopt;
  }

  const std::size_t blob_bytes_remaining =
      state.sprite_resource_bytes.size() - file->blob_offset;
  if (record.data_offset > blob_bytes_remaining) {
    return std::nullopt;
  }

  const std::size_t global_offset = file->blob_offset + record.data_offset;
  if (record.data_size > state.sprite_resource_bytes.size() - global_offset) {
    return std::nullopt;
  }

  try {
    return decode_ega_4plane_rle(
        std::span<const std::uint8_t>(state.sprite_resource_bytes)
            .subspan(global_offset, record.data_size),
        true);
  } catch (const std::exception &) {
    return std::nullopt;
  }
}

std::optional<Ega4PlaneImage> decode_frpak_record(RuntimeState &state,
                                                  std::uint16_t pak_id,
                                                  std::uint16_t record_id) {
  for (const auto &entry : state.frpak_decode_cache) {
    if (entry.pak_id == pak_id && entry.record_id == record_id) {
      return entry.image;
    }
  }

  const auto record =
      find_frpak_catalog_record(state.frpak_catalog, pak_id, record_id);
  if (!record.has_value()) {
    return std::nullopt;
  }

  const auto decoded = decode_frpak_catalog_record(state, *record);
  if (!decoded.has_value()) {
    return std::nullopt;
  }

  state.frpak_decode_cache.push_back(FrpakDecodedRecordCacheEntry{
      .pak_id = pak_id,
      .record_id = record_id,
      .image = *decoded,
  });
  return decoded;
}

void clear_frpak_decode_cache(RuntimeState &state) {
  state.frpak_decode_cache.clear();
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
    summary.room_grid_loaded =
        load_room_tilemap_from_asset_root(state, root, 0, 0);
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

      const auto pak_id = parse_frpak_id_from_path(candidate);
      if (!pak_id.has_value()) {
        continue;
      }

      const auto catalog_file =
          build_frpak_catalog_file(candidate, *bytes, *pak_id);
      if (!catalog_file.has_value()) {
        continue;
      }

      bool all_records_valid = true;
      for (const auto &record : catalog_file->records) {
        if (!validate_frpak_catalog_record_bounds(record,
                                                  catalog_file->file_size)) {
          all_records_valid = false;
          break;
        }
      }
      if (!all_records_valid) {
        continue;
      }

      FrpakCatalogFile indexed_file = *catalog_file;
      indexed_file.blob_offset = state.sprite_resource_bytes.size();
      state.frpak_catalog.files.push_back(indexed_file);
      state.sprite_resource_bytes.insert(state.sprite_resource_bytes.end(),
                                         bytes->begin(), bytes->end());
    } catch (const std::exception &) {
      // Missing or unreadable FRPAK payloads should not stop bootstrap.
    }
  }

  return summary;
}

} // namespace comic2
