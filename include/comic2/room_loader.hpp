#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

#include "comic2/game_state.hpp"
#include "comic2/resource_loader.hpp"
#include "comic2/types.hpp"

namespace comic2 {

struct RoomLoadSpec {
  std::uint16_t level = 0;
  std::uint16_t room = 0;
  FrdataRoomEntry room_entry{};

  bool operator==(const RoomLoadSpec &) const = default;
};

std::optional<FrdataRoomEntry>
decode_frdata_room_entry(std::span<const std::uint8_t> bytes,
                         std::size_t offset);

std::optional<std::vector<std::uint16_t>>
build_room_row_pointer_table(std::span<const std::uint8_t> decoded_room_bytes,
                             std::uint16_t tile_h);

bool load_room_tilemap_from_resource_buffer(RuntimeState &state,
                                            std::span<const std::uint8_t> bytes,
                                            std::uint16_t level,
                                            std::uint16_t room);

} // namespace comic2
