#pragma once

#include <cstdint>
#include <limits>
#include <optional>

#include "comic2/game_state.hpp"

namespace comic2 {

struct TileCollisionConfig {
  std::uint8_t solid_tile_threshold = 0x01;
  std::uint8_t hazard_tile_min = 0xF0;
  std::uint8_t hazard_tile_max = 0xFF;
};

std::optional<std::uint8_t> get_tile_at_pixels(const RoomTileGrid &grid,
                                               std::int16_t pixel_x,
                                               std::int16_t pixel_y);
bool tile_meets_threshold(std::uint8_t tile_id, std::uint8_t threshold);
bool is_hazard_tile(std::uint8_t tile_id, const TileCollisionConfig &config);
bool update_player_hazard_state(RuntimeState &state,
                                const TileCollisionConfig &config);

bool has_floor_support(const RuntimeState &state,
                       const TileCollisionConfig &config);
bool resolve_ground_contact(RuntimeState &state,
                            const TileCollisionConfig &config);

} // namespace comic2
