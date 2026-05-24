#pragma once

#include <cstdint>

#include "comic2/game_state.hpp"

namespace comic2 {

struct TileCollisionConfig {
    std::int16_t ground_y = 0;
};

bool resolve_ground_contact(RuntimeState& state, const TileCollisionConfig& config);

}  // namespace comic2
