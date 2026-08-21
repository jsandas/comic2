#include "comic2/default_handlers.hpp"

#include <limits>
#include <utility>

#include "comic2/audio.hpp"
#include "comic2/player_controller.hpp"
#include "comic2/renderer.hpp"
#include "comic2/room_loader.hpp"
#include "comic2/tile_collision.hpp"

namespace comic2 {

namespace {

constexpr PlayerMotionConfig kDefaultMotion{};
constexpr std::int16_t kViewportWidthPixels = 320;
constexpr std::int16_t kViewportHeightPixels = 200;
constexpr std::int16_t kPlayerHeightPixels = 32;
constexpr TileCollisionConfig kDefaultCollision{
    .solid_tile_threshold = 0x01,
    .hazard_tile_min = 0xF0,
    .hazard_tile_max = 0xFF,
};

bool has_floor_support_data(const RuntimeState &state) {
  const auto &grid = state.room_grid;
  if (grid.tile_w == 0 || grid.tile_h == 0) {
    return false;
  }

  if (grid.row_pointers.size() < grid.tile_h) {
    return false;
  }

  const std::size_t last_row =
      static_cast<std::size_t>(grid.row_pointers[grid.tile_h - 1]);
  const std::size_t min_required = last_row + grid.tile_w;
  return grid.tile_data.size() >= min_required;
}

void clamp_player_vertical_bounds(RuntimeState &state) {
  const std::int16_t max_y =
      static_cast<std::int16_t>(kViewportHeightPixels - kPlayerHeightPixels);

  if (state.player.y > max_y) {
    state.player.y = max_y;
    state.player.y_vel = 0;
    state.player.is_airborne = false;
    state.player.is_physics_active = false;
  }
}

void update_room_transition_from_player_bounds(RuntimeState &state) {
  if (state.player.x < 0) {
    if (state.current_room == 0) {
      state.player.x = 0;
      return;
    }

    state.pending_room_transition = PendingRoomTransition{
        .target_room = static_cast<std::uint16_t>(state.current_room - 1),
        .target_player_x = static_cast<std::int16_t>(kViewportWidthPixels - 1)};
    state.flags.level_transition_pending = true;
    return;
  }

  if (state.player.x >= kViewportWidthPixels) {
    state.pending_room_transition = PendingRoomTransition{
        .target_room = static_cast<std::uint16_t>(state.current_room + 1),
        .target_player_x = 0};
    state.flags.level_transition_pending = true;
  }
}

void advance_transition_state(RuntimeState &state) {
  auto &transition = state.transition_state;
  if (!transition.active) {
    transition.player_frozen = false;
    return;
  }

  transition.player_frozen = true;
  transition.frame_index =
      static_cast<std::uint16_t>(transition.frame_index + 1U);
  transition.tick_count =
      static_cast<std::uint16_t>(transition.tick_count + 1U);
  camera_update_y_follow_comic_clamped(
      state, kViewportHeightPixels,
      std::max<std::int32_t>(0, state.room_grid.tile_h * 16));
}

void apply_default_airborne_physics(RuntimeState &state) {
  apply_airborne_physics_tick(state, kDefaultMotion, kDefaultCollision);
  if (!has_floor_support_data(state)) {
    clamp_player_vertical_bounds(state);
  }
  update_room_transition_from_player_bounds(state);
  update_player_hazard_state(state, kDefaultCollision);
}

void apply_default_grounded_physics(RuntimeState &state) {
  apply_grounded_physics_tick(state, kDefaultMotion, kDefaultCollision);
  if (!has_floor_support_data(state)) {
    clamp_player_vertical_bounds(state);
  }
  update_room_transition_from_player_bounds(state);
  update_player_hazard_state(state, kDefaultCollision);
}

} // namespace

void handle_level_transition(RuntimeState &state) {
  const auto pending = state.pending_room_transition;
  state.flags.level_transition_pending = false;
  state.pending_room_transition.reset();
  if (!pending.has_value()) {
    return;
  }

  RuntimeState candidate_state = state;
  candidate_state.current_room = pending->target_room;
  candidate_state.player.x = pending->target_player_x;
  candidate_state.transition_state.active = true;
  candidate_state.transition_state.player_frozen = true;
  candidate_state.transition_state.effect_type = static_cast<std::uint16_t>(
      (candidate_state.current_room + candidate_state.current_level) % 2U);
  candidate_state.transition_state.frame_index = 0;
  candidate_state.transition_state.tick_count = 0;
  room_transition_player_entry_sequence(candidate_state);
  camera_update_y_follow_comic_clamped(
      candidate_state, kViewportHeightPixels,
      std::max<std::int32_t>(0, candidate_state.room_grid.tile_h * 16));

  state.transition_state = candidate_state.transition_state;

  bool loaded = false;
  if (!candidate_state.assets_root.empty()) {
    loaded = load_room_tilemap_from_asset_root(
        candidate_state, candidate_state.assets_root,
        candidate_state.current_level, candidate_state.current_room);
  }
  if (!loaded && !candidate_state.room_resource_bytes.empty()) {
    loaded = load_room_tilemap_from_resource_buffer(
        candidate_state, candidate_state.room_resource_bytes,
        candidate_state.current_level, candidate_state.current_room);
  }

  if (loaded) {
    state = std::move(candidate_state);
    queue_audio_event(state, AudioEvent::LevelStart);
  } else {
    state.transition_state.active = true;
    state.transition_state.player_frozen = true;
    state.transition_state.effect_type =
        candidate_state.transition_state.effect_type;
    state.transition_state.frame_index =
        candidate_state.transition_state.frame_index;
    state.transition_state.tick_count =
        candidate_state.transition_state.tick_count;
    state.transition_state.player_entry_offset =
        candidate_state.transition_state.player_entry_offset;
    state.transition_state.player_exit_offset =
        candidate_state.transition_state.player_exit_offset;
  }
}

void handle_special_logic1(RuntimeState &state) {
  state.flags.special_logic1_active = false;
}

void handle_special_logic2(RuntimeState &state) {
  state.flags.special_logic2_active = false;
}

void handle_airborne_physics(RuntimeState &state) {
  update_entity_behaviors(state);
  apply_entity_combat(state);
  apply_default_airborne_physics(state);
  state.player.is_physics_active = state.player.is_airborne;
}

void handle_timed_overlay(RuntimeState &state) {
  state.flags.timed_overlay_pending = false;
}

void handle_grounded_physics(RuntimeState &state) {
  update_entity_behaviors(state);
  apply_entity_combat(state);
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
  queue_audio_event(state, AudioEvent::Death);
  state.flags.player_special_state_active = true;
  state.flags.tile_hazard_triggered = false;
}

void handle_player_special_state(const RuntimeState &state) { (void)state; }

void handle_input_fallback(RuntimeState &state) {
  if (state.transition_state.active) {
    state.transition_state.player_frozen = true;
    advance_transition_state(state);
    room_transition_player_exit_sequence(state);
    return;
  }

  update_entity_behaviors(state);
  apply_entity_combat(state);
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
  // Keep the hook chain in DispatchStage order so the runtime path stays
  // explicit and matches the known game-loop priority sequence.
  dispatcher.set_level_transition_hook(
      [](RuntimeState &state) { handle_level_transition(state); });
  dispatcher.set_special_logic1_hook(
      [](RuntimeState &state) { handle_special_logic1(state); });
  dispatcher.set_special_logic2_hook(
      [](RuntimeState &state) { handle_special_logic2(state); });
  dispatcher.set_airborne_physics_hook(
      [](RuntimeState &state) { handle_airborne_physics(state); });
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
      [](const RuntimeState &state) { handle_player_special_state(state); });
  dispatcher.set_input_handling_hook(
      [](RuntimeState &state) { handle_input_fallback(state); });
}

GameDispatcher make_default_game_dispatcher() {
  GameDispatcher dispatcher;
  install_default_stage_hooks(dispatcher);
  return dispatcher;
}

} // namespace comic2
