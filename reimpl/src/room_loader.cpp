#include "comic2/room_loader.hpp"

namespace comic2 {

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

}  // namespace comic2
