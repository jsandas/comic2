#include "comic2/room_loader.hpp"

#include <cstddef>
#include <optional>

namespace comic2 {

namespace {

constexpr std::size_t kRoomRowPointerMapOffset = 0x2A0;

}  // namespace

std::optional<FrdataRoomEntry> decode_frdata_room_entry(
    const std::vector<std::uint8_t>& bytes,
    std::size_t offset) {
    constexpr std::size_t kFrdataRoomEntrySize = 6;

    if (offset > bytes.size() || (bytes.size() - offset) < kFrdataRoomEntrySize) {
        return std::nullopt;
    }

    FrdataRoomEntry entry;
    entry.tile_w = static_cast<std::uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
    entry.tile_h = static_cast<std::uint16_t>(bytes[offset + 2] | (bytes[offset + 3] << 8));
    entry.rle_data_off = static_cast<std::uint16_t>(bytes[offset + 4] | (bytes[offset + 5] << 8));
    return entry;
}

std::optional<std::vector<std::uint16_t>> build_room_row_pointer_table(
    const std::vector<std::uint8_t>& decoded_room_bytes,
    std::uint16_t tile_h) {
    const std::size_t table_size = static_cast<std::size_t>(tile_h) * 2;
    if (kRoomRowPointerMapOffset > decoded_room_bytes.size() ||
        decoded_room_bytes.size() - kRoomRowPointerMapOffset < table_size) {
        return std::nullopt;
    }

    std::vector<std::uint16_t> row_pointers(tile_h, 0);
    for (std::uint16_t y = 0; y < tile_h; ++y) {
        const std::size_t off = kRoomRowPointerMapOffset + static_cast<std::size_t>(y) * 2;
        row_pointers[y] =
            static_cast<std::uint16_t>(decoded_room_bytes[off] | (decoded_room_bytes[off + 1] << 8));
    }
    return row_pointers;
}

std::optional<RoomTileGrid> build_room_tile_grid(
    const std::vector<std::uint8_t>& decoded_room_bytes,
    const FrdataRoomEntry& room_entry) {
    const std::optional<std::vector<std::uint16_t>> row_pointers =
        build_room_row_pointer_table(decoded_room_bytes, room_entry.tile_h);
    if (!row_pointers.has_value()) {
        return std::nullopt;
    }

    for (std::uint16_t y = 0; y < room_entry.tile_h; ++y) {
        const std::size_t row_base = (*row_pointers)[y];
        if (row_base > decoded_room_bytes.size() ||
            decoded_room_bytes.size() - row_base < room_entry.tile_w) {
            return std::nullopt;
        }
    }

    RoomTileGrid grid;
    grid.tile_w = room_entry.tile_w;
    grid.tile_h = room_entry.tile_h;
    grid.row_pointers = std::move(*row_pointers);
    grid.tile_data = decoded_room_bytes;
    return grid;
}

}  // namespace comic2
