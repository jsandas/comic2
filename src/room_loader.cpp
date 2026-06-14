#include "comic2/room_loader.hpp"

#include <cstddef>
#include <optional>
#include <span>

#include "comic2/resource_loader.hpp"

namespace comic2 {

namespace {

constexpr std::size_t kRoomRowPointerMapOffset = 0x2A0;

} // namespace

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
  constexpr std::size_t kRoomTableOffset = 0x04;
  constexpr std::size_t kRoomEntrySize = 6;

  if (bytes.size() < kRoomTableOffset + kRoomEntrySize) {
    return false;
  }

  const std::uint16_t file_level = static_cast<std::uint16_t>(
      bytes[2] | (static_cast<std::uint16_t>(bytes[3]) << 8));
  if (file_level != level) {
    return false;
  }

  const std::size_t room_entry_offset =
      kRoomTableOffset + static_cast<std::size_t>(room) * kRoomEntrySize;
  const std::optional<FrdataRoomEntry> room_entry =
      decode_frdata_room_entry(bytes, room_entry_offset);
  if (!room_entry.has_value() || room_entry->tile_w == 0 ||
      room_entry->tile_h == 0) {
    return false;
  }

  if (room_entry->rle_data_off >= bytes.size()) {
    return false;
  }

  SignedRleResult decoded;
  try {
    decoded = decode_signed_rle(bytes.subspan(room_entry->rle_data_off));
  } catch (...) {
    return false;
  }
  const std::optional<std::vector<std::uint16_t>> row_pointers =
      build_room_row_pointer_table(decoded.bytes, room_entry->tile_h);
  if (!row_pointers.has_value()) {
    return false;
  }

  state.current_level = level;
  state.current_room = room;
  state.room_entry = *room_entry;
  state.room_grid.tile_w = room_entry->tile_w;
  state.room_grid.tile_h = room_entry->tile_h;
  state.room_grid.row_pointers = *row_pointers;
  state.room_grid.tile_data = decoded.bytes;
  return true;
}

} // namespace comic2
