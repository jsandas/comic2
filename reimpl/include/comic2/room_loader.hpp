#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "comic2/types.hpp"

namespace comic2 {

struct RoomLoadSpec {
    std::uint16_t level = 0;
    std::uint16_t room = 0;
    FrdataRoomEntry room_entry{};

    bool operator==(const RoomLoadSpec&) const = default;
};

bool decode_frdata_room_entry(const std::vector<std::uint8_t>& bytes,
                              std::size_t offset,
                              FrdataRoomEntry* out_entry);

bool build_room_row_pointer_table(const std::vector<std::uint8_t>& decoded_room_bytes,
                                  std::uint16_t tile_h,
                                  std::vector<std::uint16_t>* out_row_pointers);

bool build_room_tile_grid(const std::vector<std::uint8_t>& decoded_room_bytes,
                         const FrdataRoomEntry& room_entry,
                         RoomTileGrid* out_grid);

}  // namespace comic2
