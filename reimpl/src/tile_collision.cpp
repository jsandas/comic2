#include "comic2/tile_collision.hpp"

namespace comic2 {

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
