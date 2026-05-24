#pragma once

#include <cstdint>

#include "comic2/game_state.hpp"
#include "comic2/tile_collision.hpp"

namespace comic2 {

struct PlayerMotionConfig {
    std::int16_t walk_step = 2;
    std::int16_t jump_impulse = -5;
    std::int16_t gravity_per_tick = 1;
    std::int16_t max_fall_speed = 5;
};

void apply_input_tick(RuntimeState& state, const PlayerMotionConfig& motion);
void apply_physics_tick(RuntimeState& state,
                        const PlayerMotionConfig& motion,
                        const TileCollisionConfig& collision);

}  // namespace comic2
