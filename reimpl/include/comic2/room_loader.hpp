#pragma once

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

}  // namespace comic2
