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

#include "comic2/entity_runtime.hpp"
#include "comic2/resource_loader.hpp"

namespace comic2 {

namespace {

constexpr std::size_t kRoomRowPointerMapOffset = 0x2A0;
constexpr std::size_t kRoomTableOffset = 0x04;
constexpr std::size_t kRoomEntrySize = 6;
constexpr std::uint16_t kSentinelEntry = 0xFFFFu;
constexpr std::size_t kMappedObjectCountOffset = 0x2B0;
constexpr std::size_t kMappedObjectTableOffset = 0x2B2;
constexpr std::size_t kMappedObjectRecordSize = 12;

std::uint16_t read_u16(std::span<const std::uint8_t> bytes, std::size_t off) {
  return static_cast<std::uint16_t>(bytes[off] | (bytes[off + 1] << 8));
}

void hydrate_entity_runtime_for_room(RuntimeState &state) {
  state.active_entities.clear();
  state.runtime_slots.clear();
  state.activation_state = EntityActivationState{};

  if (state.mapped_objects.empty()) {
    return;
  }

  ent_build_room_entity_list(state.mapped_objects, state.current_room,
                             state.current_level, state.active_entities,
                             state.activation_state);

  const EntityViewportBounds viewport{};
  ent_build_runtime_slots_for_viewport(
      state.active_entities, state.mapped_objects, state.runtime_slots,
      viewport, state.activation_state, state.activation_toggle);
}

bool is_room_payload_candidate_name(std::string name) {
  std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
    return static_cast<char>(std::toupper(c));
  });

  if (name.size() != 7 || name.rfind("FR", 0) != 0 || name[5] != '.') {
    return false;
  }

  const bool room_index_is_digits =
      std::isdigit(static_cast<unsigned char>(name[2])) != 0 &&
      std::isdigit(static_cast<unsigned char>(name[3])) != 0 &&
      std::isdigit(static_cast<unsigned char>(name[4])) != 0;
  const bool variant_is_digit =
      std::isdigit(static_cast<unsigned char>(name[6])) != 0;
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

  for (std::filesystem::directory_iterator it(root, ec), end; !ec && it != end;
       it.increment(ec)) {
    const auto &entry = *it;
    if (!entry.is_regular_file(ec) || ec) {
      ec.clear();
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
                       std::span<const std::uint8_t> bytes, std::uint16_t level,
                       std::uint16_t room, ResourceAssetKind asset_kind) {
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
  spec.resource_offset =
      asset_kind == ResourceAssetKind::RoomTable
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

std::optional<std::vector<MappedObject12>>
decode_room_mapped_objects(std::span<const std::uint8_t> decoded_room_bytes) {
  if (decoded_room_bytes.size() < kMappedObjectCountOffset + 2) {
    return std::vector<MappedObject12>{};
  }

  const std::uint16_t count =
      read_u16(decoded_room_bytes, kMappedObjectCountOffset);
  if (count == 0 || count == kSentinelEntry) {
    return std::vector<MappedObject12>{};
  }

  const std::size_t needed_size =
      kMappedObjectTableOffset +
      static_cast<std::size_t>(count) * kMappedObjectRecordSize;
  if (needed_size > decoded_room_bytes.size()) {
    return std::nullopt;
  }

  std::vector<MappedObject12> mapped_objects;
  mapped_objects.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    const std::size_t off =
        kMappedObjectTableOffset + i * kMappedObjectRecordSize;

    MappedObject12 obj{};
    obj.room_x = read_u16(decoded_room_bytes, off + 0);
    obj.room_y = read_u16(decoded_room_bytes, off + 2);
    obj.descriptor_ptr = read_u16(decoded_room_bytes, off + 4);
    obj.state_flags = read_u16(decoded_room_bytes, off + 6);
    obj.world_x = read_u16(decoded_room_bytes, off + 8);
    obj.world_y = read_u16(decoded_room_bytes, off + 10);

    if (obj.room_x == kSentinelEntry && obj.room_y == kSentinelEntry &&
        obj.descriptor_ptr == kSentinelEntry &&
        obj.state_flags == kSentinelEntry && obj.world_x == kSentinelEntry &&
        obj.world_y == kSentinelEntry) {
      continue;
    }

    mapped_objects.push_back(obj);
  }

  return mapped_objects;
}

bool load_room_tilemap_from_resource_buffer(RuntimeState &state,
                                            std::span<const std::uint8_t> bytes,
                                            std::uint16_t level,
                                            std::uint16_t room) {
  const std::optional<RoomLoadSpec> spec = resolve_room_load_spec(
      {}, bytes, level, room, ResourceAssetKind::RoomPayload);
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
  std::vector<std::uint8_t> resource_bytes(bytes.begin(), bytes.end());
  state.room_resource_bytes = std::move(resource_bytes);

  const auto mapped_objects = decode_room_mapped_objects(decoded.bytes);
  state.mapped_objects = mapped_objects.value_or(std::vector<MappedObject12>{});
  hydrate_entity_runtime_for_room(state);

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
  std::vector<std::uint8_t> resource_bytes(bytes.begin(), bytes.end());
  state.room_resource_bytes = std::move(resource_bytes);

  const auto mapped_objects = decode_room_mapped_objects(decoded.bytes);
  state.mapped_objects = mapped_objects.value_or(std::vector<MappedObject12>{});
  hydrate_entity_runtime_for_room(state);

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
