#include "comic2/room_loader.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "comic2/resource_loader.hpp"

namespace comic2 {

namespace {

constexpr std::size_t kRoomRowPointerMapOffset = 0x2A0;
constexpr std::size_t kRoomTableOffset = 0x04;
constexpr std::size_t kRoomEntrySize = 6;
constexpr std::uint16_t kSentinelEntry = 0xFFFFu;

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

std::optional<RoomLoadSpec>
resolve_room_load_spec(const std::filesystem::path &source_path,
                       std::span<const std::uint8_t> bytes,
                       std::uint16_t level, std::uint16_t room,
                       ResourceAssetKind asset_kind) {
  if (bytes.size() < kRoomTableOffset + kRoomEntrySize) {
    return std::nullopt;
  }

  const std::uint16_t file_level = static_cast<std::uint16_t>(
      bytes[2] | (static_cast<std::uint16_t>(bytes[3]) << 8));
  if (file_level != level && file_level != 0) {
    return std::nullopt;
  }

  const std::size_t room_entry_offset =
      kRoomTableOffset + static_cast<std::size_t>(room) * kRoomEntrySize;
  if (room_entry_offset > bytes.size() - kRoomEntrySize) {
    return std::nullopt;
  }

  const std::optional<FrdataRoomEntry> room_entry =
      decode_frdata_room_entry(bytes, room_entry_offset);
  if (!room_entry.has_value() || room_entry->tile_w == 0 ||
      room_entry->tile_h == 0 || room_entry->tile_w == kSentinelEntry ||
      room_entry->tile_h == kSentinelEntry ||
      room_entry->rle_data_off == kSentinelEntry) {
    return std::nullopt;
  }

  RoomLoadSpec spec;
  spec.source_path = source_path;
  spec.level = level;
  spec.room = room;
  spec.asset_kind = asset_kind;
  spec.table_offset = kRoomTableOffset;
  spec.room_entry_offset = room_entry_offset;
  spec.resource_offset = asset_kind == ResourceAssetKind::RoomTable
                             ? room_entry_offset
                             : static_cast<std::size_t>(room_entry->rle_data_off);
  spec.room_entry = *room_entry;

  if (asset_kind == ResourceAssetKind::RoomPayload &&
      spec.resource_offset >= bytes.size()) {
    return std::nullopt;
  }

  return spec;
}

std::optional<FrdataRoomEntry>
decode_frdata_room_entry(std::span<const std::uint8_t> bytes,
                         std::size_t offset) {
  constexpr std::size_t kFrdataRoomEntrySize = 6;

  if (bytes.size() < kFrdataRoomEntrySize ||
      offset > bytes.size() - kFrdataRoomEntrySize) {
    return std::nullopt;
  }

  FrdataRoomEntry entry;
  entry.tile_w =
      static_cast<std::uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
  entry.tile_h =
      static_cast<std::uint16_t>(bytes[offset + 2] | (bytes[offset + 3] << 8));
  entry.rle_data_off =
      static_cast<std::uint16_t>(bytes[offset + 4] | (bytes[offset + 5] << 8));
  return entry;
}

std::optional<std::vector<std::uint16_t>>
build_room_row_pointer_table(std::span<const std::uint8_t> decoded_room_bytes,
                             std::uint16_t tile_h) {
  const std::size_t table_size = static_cast<std::size_t>(tile_h) * 2;
  if (kRoomRowPointerMapOffset > decoded_room_bytes.size() ||
      decoded_room_bytes.size() - kRoomRowPointerMapOffset < table_size) {
    return std::nullopt;
  }

  std::vector<std::uint16_t> row_pointers(tile_h, 0);
  for (std::uint16_t y = 0; y < tile_h; ++y) {
    const std::size_t off =
        kRoomRowPointerMapOffset + static_cast<std::size_t>(y) * 2;
    row_pointers[y] = static_cast<std::uint16_t>(
        decoded_room_bytes[off] | (decoded_room_bytes[off + 1] << 8));
  }
  return row_pointers;
}

bool load_room_tilemap_from_resource_buffer(RuntimeState &state,
                                            std::span<const std::uint8_t> bytes,
                                            std::uint16_t level,
                                            std::uint16_t room) {
  const std::optional<RoomLoadSpec> spec =
      resolve_room_load_spec({}, bytes, level, room,
                             ResourceAssetKind::RoomPayload);
  if (!spec.has_value()) {
    return false;
  }

  SignedRleResult decoded;
  try {
    decoded = decode_signed_rle(bytes.subspan(spec->resource_offset));
  } catch (...) {
    return false;
  }
  const std::optional<std::vector<std::uint16_t>> row_pointers =
      build_room_row_pointer_table(decoded.bytes, spec->room_entry.tile_h);
  if (!row_pointers.has_value()) {
    return false;
  }

  state.current_level = level;
  state.current_room = room;
  state.room_entry = spec->room_entry;
  state.room_grid.tile_w = spec->room_entry.tile_w;
  state.room_grid.tile_h = spec->room_entry.tile_h;
  state.room_grid.row_pointers = *row_pointers;
  state.room_grid.tile_data = decoded.bytes;
  return true;
}

bool load_room_tilemap_from_resource_file(
    RuntimeState &state, const std::filesystem::path &source_path,
    std::span<const std::uint8_t> bytes, std::uint16_t level,
    std::uint16_t room) {
  const std::optional<RoomLoadSpec> spec = resolve_room_load_spec(
      source_path, bytes, level, room, ResourceAssetKind::RoomPayload);
  if (!spec.has_value()) {
    return false;
  }

  SignedRleResult decoded;
  try {
    decoded = decode_signed_rle(bytes.subspan(spec->resource_offset));
  } catch (...) {
    return false;
  }
  const std::optional<std::vector<std::uint16_t>> row_pointers =
      build_room_row_pointer_table(decoded.bytes, spec->room_entry.tile_h);
  if (!row_pointers.has_value()) {
    return false;
  }

  state.current_level = level;
  state.current_room = room;
  state.room_entry = spec->room_entry;
  state.room_grid.tile_w = spec->room_entry.tile_w;
  state.room_grid.tile_h = spec->room_entry.tile_h;
  state.room_grid.row_pointers = *row_pointers;
  state.room_grid.tile_data = decoded.bytes;
  return true;
}

bool load_room_tilemap_from_asset_root(RuntimeState &state,
                                       const std::filesystem::path &root,
                                       std::uint16_t level,
                                       std::uint16_t room) {
  const auto candidates = discover_room_payload_candidates(root);
  for (const auto &candidate : candidates) {
    const auto bytes = load_file_bytes(candidate);
    if (!bytes.has_value()) {
      continue;
    }

    RuntimeState candidate_state = state;
    if (load_room_tilemap_from_resource_file(candidate_state, candidate, *bytes,
                                             level, room)) {
      candidate_state.room_resource_bytes = *bytes;
      state = std::move(candidate_state);
      return true;
    }
  }

  return false;
}

} // namespace comic2
