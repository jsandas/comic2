#include "comic2/tile_collision.hpp"

namespace comic2 {

bool has_floor_support(const RuntimeState& state, const TileCollisionConfig& config) {
    const std::int16_t x = state.player.x;
    return x >= config.support_min_x && x <= config.support_max_x;
}

bool resolve_ground_contact(RuntimeState& state, const TileCollisionConfig& config) {
    if (state.player.y < config.ground_y) {
        return false;
    }

    state.player.y = config.ground_y;
    state.player.y_vel = 0;
    state.player.is_airborne = false;
    state.player.is_physics_active = false;
    return true;
}

}  // namespace comic2
