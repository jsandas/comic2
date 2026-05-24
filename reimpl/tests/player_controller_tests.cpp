#include <stdexcept>

#include "comic2/player_controller.hpp"

namespace {

void expect(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_apply_input_tick_moves_right() {
    comic2::RuntimeState state;
    state.input.right_pressed = true;

    comic2::apply_input_tick(state, comic2::PlayerMotionConfig{});

    expect(state.player.x == 2, "right input should move x by walk step");
    expect(state.player.x_vel == 2, "right input should set x_vel");
}

void test_apply_input_tick_consumes_jump_counter() {
    comic2::RuntimeState state;
    state.input.jump_pressed = true;
    state.player.jump_counter = 1;

    comic2::apply_input_tick(state, comic2::PlayerMotionConfig{});

    expect(state.player.y_vel == -5, "jump input should apply jump impulse");
    expect(state.player.jump_counter == 0, "jump input should consume jump counter");
    expect(state.player.is_physics_active, "jump input should activate physics");
}

void test_apply_physics_tick_lands_on_ground() {
    comic2::RuntimeState state;
    state.player.y = -1;
    state.player.y_vel = 2;
    state.player.is_airborne = true;
    state.player.is_physics_active = true;

    comic2::apply_physics_tick(state, comic2::PlayerMotionConfig{}, comic2::TileCollisionConfig{});

    expect(state.player.y == 0, "physics tick should clamp y to ground");
    expect(state.player.y_vel == 0, "physics tick should zero vertical velocity on ground");
    expect(!state.player.is_airborne, "physics tick should clear airborne on ground contact");
    expect(!state.player.is_physics_active, "physics tick should clear physics active on ground contact");
}

}  // namespace

void run_player_controller_tests() {
    test_apply_input_tick_moves_right();
    test_apply_input_tick_consumes_jump_counter();
    test_apply_physics_tick_lands_on_ground();
}
