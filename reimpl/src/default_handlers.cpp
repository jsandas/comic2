#include "comic2/default_handlers.hpp"

#include <algorithm>

namespace comic2 {

void handle_basic_input(RuntimeState& state) {
    constexpr std::int16_t kWalkStep = 2;
    constexpr std::int16_t kJumpImpulse = -5;

    const bool move_left = state.input.left_pressed;
    const bool move_right = state.input.right_pressed;

    if (move_left && !move_right) {
        state.player.x_vel = -kWalkStep;
        state.player.x += state.player.x_vel;
    } else if (move_right && !move_left) {
        state.player.x_vel = kWalkStep;
        state.player.x += state.player.x_vel;
    } else {
        state.player.x_vel = 0;
    }

    if (state.input.jump_pressed && state.player.jump_counter > 0) {
        state.player.is_physics_active = true;
        state.player.y_vel = kJumpImpulse;
        --state.player.jump_counter;
    }
}

void step_grounded_physics(RuntimeState& state) {
    constexpr std::int16_t kGravityPerTick = 1;
    constexpr std::int16_t kMaxFallSpeed = 5;

    state.player.y += state.player.y_vel;
    state.player.y_vel = static_cast<std::int16_t>(std::min<std::int32_t>(state.player.y_vel + kGravityPerTick, kMaxFallSpeed));

    if (state.player.y >= 0) {
        state.player.y = 0;
        state.player.y_vel = 0;
        state.player.is_airborne = false;
        state.player.is_physics_active = false;
    } else {
        state.player.is_airborne = true;
        state.player.is_physics_active = true;
    }
}

void install_default_stage_hooks(GameDispatcher& dispatcher) {
    dispatcher.set_input_handling_hook([](RuntimeState& state) { handle_basic_input(state); });
    dispatcher.set_grounded_physics_hook([](RuntimeState& state) { step_grounded_physics(state); });
    dispatcher.set_airborne_physics_hook([](RuntimeState& state) { step_grounded_physics(state); });
}

}  // namespace comic2
