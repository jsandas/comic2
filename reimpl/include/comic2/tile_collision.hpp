#pragma once

#include <cstdint>
#include <limits>

#include "comic2/game_state.hpp"

namespace comic2 {

struct TileCollisionConfig {
    std::int16_t ground_y = 0;
    std::int16_t support_min_x = std::numeric_limits<std::int16_t>::min();
    std::int16_t support_max_x = std::numeric_limits<std::int16_t>::max();
};

bool has_floor_support(const RuntimeState& state, const TileCollisionConfig& config);
bool resolve_ground_contact(RuntimeState& state, const TileCollisionConfig& config);

}  // namespace comic2
