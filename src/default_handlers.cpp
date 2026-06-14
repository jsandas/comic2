#include "comic2/default_handlers.hpp"

#include <limits>

#include "comic2/player_controller.hpp"
#include "comic2/room_loader.hpp"
#include "comic2/tile_collision.hpp"

namespace comic2 {

namespace {

constexpr PlayerMotionConfig kDefaultMotion{};
constexpr std::int16_t kViewportWidthPixels = 320;
constexpr TileCollisionConfig kDefaultCollision{
    .solid_tile_threshold = 0x01,
    .hazard_tile_min = 0xF0,
    .hazard_tile_max = 0xFF,
};

void update_room_transition_from_player_bounds(RuntimeState &state) {
  if (state.player.x < 0) {
    if (state.current_room == 0) {
      state.player.x = 0;
      return;
    }

    --state.current_room;
    state.player.x = static_cast<std::int16_t>(kViewportWidthPixels - 1);
    state.flags.level_transition_pending = true;
    return;
  }

  if (state.player.x >= kViewportWidthPixels) {
    ++state.current_room;
    state.player.x = 0;
    state.flags.level_transition_pending = true;
  }
}

void apply_default_airborne_physics(RuntimeState &state) {
  apply_airborne_physics_tick(state, kDefaultMotion, kDefaultCollision);
  update_room_transition_from_player_bounds(state);
  update_player_hazard_state(state, kDefaultCollision);
}

void apply_default_grounded_physics(RuntimeState &state) {
  apply_grounded_physics_tick(state, kDefaultMotion, kDefaultCollision);
  update_room_transition_from_player_bounds(state);
  update_player_hazard_state(state, kDefaultCollision);
}

} // namespace

void handle_level_transition(RuntimeState &state) {
  state.flags.level_transition_pending = false;
  if (!state.room_resource_bytes.empty()) {
    load_room_tilemap_from_resource_buffer(state, state.room_resource_bytes,
                                           state.current_level,
                                           state.current_room);
  }
}

void handle_special_logic1(RuntimeState &state) {
  state.flags.special_logic1_active = false;
}

void handle_special_logic2(RuntimeState &state) {
  state.flags.special_logic2_active = false;
}

void handle_airborne_movement(RuntimeState &state) {
  apply_default_airborne_physics(state);
  state.player.is_physics_active = state.player.is_airborne;
}

void handle_timed_overlay(RuntimeState &state) {
  state.flags.timed_overlay_pending = false;
}

void handle_grounded_physics(RuntimeState &state) {
  apply_default_grounded_physics(state);
  state.player.is_physics_active = state.player.is_airborne;
}

void handle_player_animation(RuntimeState &state) {
  state.player.is_animation_active = false;
}

void handle_attack_animation(RuntimeState &state) {
  state.player.is_attack_active = false;
}

void handle_distance_interaction(RuntimeState &state) {
  state.flags.distance_interaction_active = false;
}

void handle_tile_hazard(RuntimeState &state) {
  state.player.hp = 0;
  state.flags.player_special_state_active = true;
  state.flags.tile_hazard_triggered = false;
}

void handle_player_special_state(RuntimeState &state) {
}

void handle_input_fallback(RuntimeState &state) {
  apply_input_tick(state, kDefaultMotion);
  update_room_transition_from_player_bounds(state);
  update_player_hazard_state(state, kDefaultCollision);

  // Keep grounded physics in the dispatch chain while on-foot so ledge/floor
  // transitions are evaluated via the documented stage priority path.
  if (!state.player.is_airborne) {
    state.player.is_physics_active = true;
  }
}

void install_default_stage_hooks(GameDispatcher &dispatcher) {
  dispatcher.set_level_transition_hook(
      [](RuntimeState &state) { handle_level_transition(state); });
  dispatcher.set_special_logic1_hook(
      [](RuntimeState &state) { handle_special_logic1(state); });
  dispatcher.set_special_logic2_hook(
      [](RuntimeState &state) { handle_special_logic2(state); });
  dispatcher.set_airborne_physics_hook(
      [](RuntimeState &state) { handle_airborne_movement(state); });
  dispatcher.set_timed_overlay_hook(
      [](RuntimeState &state) { handle_timed_overlay(state); });
  dispatcher.set_grounded_physics_hook(
      [](RuntimeState &state) { handle_grounded_physics(state); });
  dispatcher.set_player_animation_hook(
      [](RuntimeState &state) { handle_player_animation(state); });
  dispatcher.set_attack_animation_hook(
      [](RuntimeState &state) { handle_attack_animation(state); });
  dispatcher.set_distance_interaction_hook(
      [](RuntimeState &state) { handle_distance_interaction(state); });
  dispatcher.set_tile_hazard_hook(
      [](RuntimeState &state) { handle_tile_hazard(state); });
  dispatcher.set_player_special_state_hook(
      [](RuntimeState &state) { handle_player_special_state(state); });
  dispatcher.set_input_handling_hook(
      [](RuntimeState &state) { handle_input_fallback(state); });
}

} // namespace comic2
