#include "comic2/default_handlers.hpp"

#include <limits>

#include "comic2/player_controller.hpp"
#include "comic2/tile_collision.hpp"

namespace comic2 {

namespace {

constexpr PlayerMotionConfig kDefaultMotion{};
constexpr TileCollisionConfig kDefaultCollision{
    .ground_y = 0,
    .solid_tile_threshold = 0x01,
    .hazard_tile_min = 0xF0,
    .hazard_tile_max = 0xFF,
};

void apply_default_airborne_physics(RuntimeState& state) {
    apply_airborne_physics_tick(state, kDefaultMotion, kDefaultCollision);
    update_player_hazard_state(state, kDefaultCollision);
}

void apply_default_grounded_physics(RuntimeState& state) {
    apply_grounded_physics_tick(state, kDefaultMotion, kDefaultCollision);
    update_player_hazard_state(state, kDefaultCollision);
}

}  // namespace

void handle_level_transition(RuntimeState& state) {
    (void)state;
}

void handle_special_logic1(RuntimeState& state) {
    (void)state;
}

void handle_special_logic2(RuntimeState& state) {
    (void)state;
}

void handle_airborne_movement(RuntimeState& state) {
    apply_default_airborne_physics(state);
}

void handle_timed_overlay(RuntimeState& state) {
    (void)state;
}

void handle_grounded_physics(RuntimeState& state) {
    apply_default_grounded_physics(state);
}

void handle_player_animation(RuntimeState& state) {
    (void)state;
}

void handle_attack_animation(RuntimeState& state) {
    (void)state;
}

void handle_distance_interaction(RuntimeState& state) {
    (void)state;
}

void handle_tile_hazard(RuntimeState& state) {
    if (state.player.hp > 0) {
        --state.player.hp;
    }
    state.flags.tile_hazard_triggered = false;
}

void handle_player_special_state(RuntimeState& state) {
    (void)state;
}

void handle_input_fallback(RuntimeState& state) {
    apply_input_tick(state, kDefaultMotion);
}

void install_default_stage_hooks(GameDispatcher& dispatcher) {
    dispatcher.set_level_transition_hook([](RuntimeState& state) { handle_level_transition(state); });
    dispatcher.set_special_logic1_hook([](RuntimeState& state) { handle_special_logic1(state); });
    dispatcher.set_special_logic2_hook([](RuntimeState& state) { handle_special_logic2(state); });
    dispatcher.set_airborne_physics_hook([](RuntimeState& state) { handle_airborne_movement(state); });
    dispatcher.set_timed_overlay_hook([](RuntimeState& state) { handle_timed_overlay(state); });
    dispatcher.set_grounded_physics_hook([](RuntimeState& state) { handle_grounded_physics(state); });
    dispatcher.set_player_animation_hook([](RuntimeState& state) { handle_player_animation(state); });
    dispatcher.set_attack_animation_hook([](RuntimeState& state) { handle_attack_animation(state); });
    dispatcher.set_distance_interaction_hook([](RuntimeState& state) { handle_distance_interaction(state); });
    dispatcher.set_tile_hazard_hook([](RuntimeState& state) { handle_tile_hazard(state); });
    dispatcher.set_player_special_state_hook([](RuntimeState& state) { handle_player_special_state(state); });
    dispatcher.set_input_handling_hook([](RuntimeState& state) { handle_input_fallback(state); });
}

}  // namespace comic2
