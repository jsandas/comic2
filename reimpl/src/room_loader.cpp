#include "comic2/room_loader.hpp"

#include <cstddef>

namespace comic2 {

namespace {

constexpr std::size_t kRoomRowPointerMapOffset = 0x2A0;

}  // namespace

bool decode_frdata_room_entry(const std::vector<std::uint8_t>& bytes,
                              std::size_t offset,
                              FrdataRoomEntry* out_entry) {
    constexpr std::size_t kFrdataRoomEntrySize = 6;

    if (out_entry == nullptr) {
        return false;
    }

    if (offset > bytes.size() || (bytes.size() - offset) < kFrdataRoomEntrySize) {
        return false;
    }

    out_entry->tile_w = static_cast<std::uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
    out_entry->tile_h = static_cast<std::uint16_t>(bytes[offset + 2] | (bytes[offset + 3] << 8));
    out_entry->rle_data_off = static_cast<std::uint16_t>(bytes[offset + 4] | (bytes[offset + 5] << 8));
    return true;
}

bool build_room_row_pointer_table(const std::vector<std::uint8_t>& decoded_room_bytes,
                                  std::uint16_t tile_h,
                                  std::vector<std::uint16_t>* out_row_pointers) {
    if (out_row_pointers == nullptr) {
        return false;
    }

    const std::size_t table_size = static_cast<std::size_t>(tile_h) * 2;
    if (kRoomRowPointerMapOffset > decoded_room_bytes.size() ||
        decoded_room_bytes.size() - kRoomRowPointerMapOffset < table_size) {
        return false;
    }

    out_row_pointers->assign(tile_h, 0);
    for (std::uint16_t y = 0; y < tile_h; ++y) {
        const std::size_t off = kRoomRowPointerMapOffset + static_cast<std::size_t>(y) * 2;
        (*out_row_pointers)[y] =
            static_cast<std::uint16_t>(decoded_room_bytes[off] | (decoded_room_bytes[off + 1] << 8));
    }
    return true;
}

bool build_room_tile_grid(const std::vector<std::uint8_t>& decoded_room_bytes,
                         const FrdataRoomEntry& room_entry,
                         RoomTileGrid* out_grid) {
    if (out_grid == nullptr) {
        return false;
    }

    std::vector<std::uint16_t> row_pointers;
    if (!build_room_row_pointer_table(decoded_room_bytes, room_entry.tile_h, &row_pointers)) {
        return false;
    }

    for (std::uint16_t y = 0; y < room_entry.tile_h; ++y) {
        const std::size_t row_base = row_pointers[y];
        if (row_base > decoded_room_bytes.size() ||
            decoded_room_bytes.size() - row_base < room_entry.tile_w) {
            return false;
        }
    }

    out_grid->tile_w = room_entry.tile_w;
    out_grid->tile_h = room_entry.tile_h;
    out_grid->row_pointers = std::move(row_pointers);
    out_grid->tile_data = decoded_room_bytes;
    return true;
}

}  // namespace comic2
