#include "comic2/player_controller.hpp"

#include <algorithm>

namespace comic2 {

void apply_input_tick(RuntimeState& state, const PlayerMotionConfig& motion) {
    const bool move_left = state.input.left_pressed;
    const bool move_right = state.input.right_pressed;

    if (move_left && !move_right) {
        state.player.x_vel = static_cast<std::int16_t>(-motion.walk_step);
        state.player.x += state.player.x_vel;
    } else if (move_right && !move_left) {
        state.player.x_vel = motion.walk_step;
        state.player.x += state.player.x_vel;
    } else {
        state.player.x_vel = 0;
    }

    if (state.input.jump_pressed && state.player.jump_counter > 0) {
        state.player.is_physics_active = true;
        state.player.y_vel = motion.jump_impulse;
        --state.player.jump_counter;
    }
}

void apply_physics_tick(RuntimeState& state,
                        const PlayerMotionConfig& motion,
                        const TileCollisionConfig& collision) {
    state.player.y += state.player.y_vel;
    state.player.y_vel = static_cast<std::int16_t>(
        std::min<std::int32_t>(state.player.y_vel + motion.gravity_per_tick, motion.max_fall_speed));

    if (resolve_ground_contact(state, collision)) {
        return;
    }

    state.player.is_airborne = true;
    state.player.is_physics_active = true;
}

}  // namespace comic2
