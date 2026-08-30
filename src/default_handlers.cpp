#include "comic2/default_handlers.hpp"

#include <algorithm>
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

void complete_pending_room_transition(RuntimeState &state) {
  auto &transition = state.transition_state;
  if (transition.completed) {
    return;
  }

  transition.completed = true;
  transition.active = false;
  transition.player_frozen = false;

  state.current_room = transition.target_room;
  state.player.x = transition.target_player_x;
  state.player.y = std::clamp(
      state.player.y, static_cast<std::int16_t>(0),
      static_cast<std::int16_t>(kViewportHeightPixels - kPlayerHeightPixels));
  state.player.x_vel = 0;
  state.player.y_vel = 0;
  state.player.is_airborne = false;
}

void advance_transition_state(RuntimeState &state) {
  auto &transition = state.transition_state;
  if (!transition.active) {
    transition.player_frozen = false;
    return;
  }

  constexpr std::uint16_t kTransitionDurationTicks = 32;

  transition.player_frozen = true;
  transition.frame_index =
      static_cast<std::uint16_t>(transition.frame_index + 1U);
  transition.tick_count =
      static_cast<std::uint16_t>(transition.tick_count + 1U);
  if (transition.tick_count >= kTransitionDurationTicks) {
    transition.active = false;
    transition.player_frozen = false;
    return;
  }

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

void queue_room_event_message(RuntimeState &state) {
  if (!state.flags.room_event_triggered) {
    return;
  }

  if (state.ui.room_event_consumed) {
    state.flags.room_event_triggered = false;
    return;
  }

  state.flags.room_event_triggered = false;
  if (!state.ui.pending_event_message.empty()) {
    return;
  }

  state.ui.room_event_consumed = true;
  state.ui.pending_event_message = "Room Event Triggered";
}

void show_pending_room_event_message(RuntimeState &state) {
  if (state.ui.pending_event_message.empty() || state.ui.modal_active) {
    return;
  }

  state.ui.modal_active = true;
  state.ui.modal_prompt = state.ui.pending_event_message;
  state.ui.pending_event_message.clear();
  state.ui.modal_game_over = false;
  state.ui.level_complete_modal = false;
  state.transition_state.player_frozen = true;
}

void apply_default_grounded_physics(RuntimeState &state) {
  apply_grounded_physics_tick(state, kDefaultMotion, kDefaultCollision);
  if (!has_floor_support_data(state)) {
    clamp_player_vertical_bounds(state);
  }
  update_room_transition_from_player_bounds(state);
  update_player_hazard_state(state, kDefaultCollision);
}

void reset_runtime_for_new_game(RuntimeState &state) {
  state.player = PlayerState{};
  state.player.x = 64;
  state.player.y = 96;
  state.player.is_physics_active = true;
  state.player.facing_right = true;
  state.player.hp = 12;
  state.player.firepower = 1;
  state.player.gems = 0;
  state.player.lives = 3;
  state.player.invuln_ticks = 0;
  state.current_room = 0;
  state.current_level = 0;
  state.level_complete = false;
  state.level_completion_gems_required = 0;
  state.pending_room_transition.reset();
  state.flags = DispatcherFlags{};
  state.ui = UiState{};
  state.progression = ProgressionState{};
  state.transition_state = RoomTransitionState{};
  state.projectiles.clear();
  state.runtime_slots.clear();
  state.active_entities.clear();
  state.mapped_objects.clear();
  state.activation_state = EntityActivationState{};
  state.activation_toggle = 1;
  state.camera_y = 0;
}

} // namespace

void reset_player_respawn_state(RuntimeState &state) {
  state.player.hp = 12;
  state.player.x = 160;
  state.player.y = 160;
  state.player.x_vel = 0;
  state.player.y_vel = 0;
  state.player.is_airborne = false;
  state.player.invuln_ticks = 0;
  state.player.damage_recoil_ticks = 0;
  state.player.death_timer_ticks = 0;
  state.player.animation_state =
      static_cast<std::uint8_t>(PlayerAnimationState::Idle);
  state.player.animation_frame = 0;
  state.player.animation_ticks = 0;
  state.player.is_animation_active = false;
  state.player.is_attack_active = false;
  state.player.attack_overlay_ticks = 0;
  state.player.overlay_active = false;
  state.player.overlay_ticks = 0;
  state.player.overlay_sprite_frame = 0;
  state.player.active_mode_effect = 0;
  state.player.mode_effect_ticks = 0;
  state.player.is_physics_active = true;
  state.transition_state.player_frozen = false;
  state.ui.modal_active = false;
  state.ui.modal_prompt.clear();
  state.ui.modal_game_over = false;
  state.ui.game_over = false;
  state.ui.level_complete_modal = false;
  state.ui.pending_event_message.clear();
  state.level_complete = false;
  state.level_completion_gems_required = 0;
  state.flags.player_special_state_active = false;
}

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
  candidate_state.transition_state.completed = false;
  candidate_state.transition_state.target_room = pending->target_room;
  candidate_state.transition_state.target_player_x = pending->target_player_x;
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

  if (!state.player.overlay_active) {
    if (state.player.overlay_ticks == 0) {
      state.player.overlay_ticks = 30;
    }
    state.player.overlay_active = true;
    return;
  }

  if (state.player.overlay_ticks > 0) {
    state.player.overlay_ticks =
        static_cast<std::uint8_t>(state.player.overlay_ticks - 1U);
  }

  if (state.player.overlay_ticks == 0) {
    state.player.overlay_active = false;
  }
}

void handle_grounded_physics(RuntimeState &state) {
  update_entity_behaviors(state);
  apply_entity_combat(state);
  apply_default_grounded_physics(state);
  state.player.is_physics_active = state.player.is_airborne;
}

void handle_player_animation(RuntimeState &state) {
  state.player.is_animation_active = false;

  const auto animation_state =
      static_cast<PlayerAnimationState>(state.player.animation_state);
  if (animation_state == PlayerAnimationState::WalkCycle) {
    state.player.animation_ticks =
        static_cast<std::uint8_t>(state.player.animation_ticks + 1U);
    state.player.animation_frame =
        static_cast<std::uint8_t>((state.player.animation_frame + 1U) % 4U);
  } else if (state.player.is_airborne) {
    state.player.animation_state =
        static_cast<std::uint8_t>(PlayerAnimationState::JumpFall);
  } else if (state.player.x_vel != 0 || state.player.y_vel != 0 ||
             state.input.left_pressed || state.input.right_pressed) {
    state.player.animation_state =
        static_cast<std::uint8_t>(PlayerAnimationState::WalkCycle);
    state.player.animation_frame =
        static_cast<std::uint8_t>((state.player.animation_frame + 1U) % 4U);
  } else {
    state.player.animation_state =
        static_cast<std::uint8_t>(PlayerAnimationState::Idle);
    state.player.animation_frame = 0;
  }
}

void handle_attack_animation(RuntimeState &state) {
  state.player.is_attack_active = false;

  if (state.player.attack_overlay_ticks > 0) {
    state.player.attack_overlay_ticks =
        static_cast<std::uint8_t>(state.player.attack_overlay_ticks - 1U);
  }

  state.player.animation_state =
      static_cast<std::uint8_t>(PlayerAnimationState::Attack);
  if (state.player.attack_overlay_ticks == 0) {
    state.player.animation_state =
        static_cast<std::uint8_t>(PlayerAnimationState::Idle);
  }
}

void handle_distance_interaction(RuntimeState &state) {
  state.flags.distance_interaction_active = false;
}

void handle_tile_hazard(RuntimeState &state) {
  state.player.hp = 0;
  state.player.death_timer_ticks = 3;
  state.player.animation_state =
      static_cast<std::uint8_t>(PlayerAnimationState::Death);
  state.player.is_animation_active = true;
  state.transition_state.player_frozen = true;
  queue_audio_event(state, AudioEvent::Death);
  state.flags.player_special_state_active = true;
  state.flags.tile_hazard_triggered = false;
}

void handle_player_special_state(RuntimeState &state) {
  state.flags.player_special_state_active = true;

  if (state.level_complete && state.ui.level_complete_modal) {
    if (state.ui.modal_confirmed) {
      state.ui.modal_confirmed = false;
      state.ui.modal_active = false;
      state.ui.modal_prompt.clear();
      state.ui.level_complete_modal = false;
      state.level_complete = false;
      state.level_completion_gems_required = 0;
      reset_runtime_for_new_game(state);
      return;
    }

    if (!state.ui.modal_active) {
      state.ui.modal_active = true;
      state.ui.modal_prompt = "Level Complete!";
      state.ui.modal_game_over = false;
      state.ui.level_complete_modal = true;
    }
    return;
  }

  if (state.ui.game_over) {
    const bool restart_requested =
        state.ui.modal_confirmed || state.input.jump_pressed ||
        state.input.pause_pressed || state.input.right_pressed ||
        state.input.left_pressed;
    if (restart_requested) {
      reset_runtime_for_new_game(state);
      state.flags.player_special_state_active = false;
      return;
    }

    if (!state.ui.modal_active) {
      state.ui.modal_active = true;
      state.ui.modal_prompt = "Game Over";
      state.ui.modal_game_over = true;
    }
    return;
  }

  if (state.ui.modal_active && state.ui.modal_confirmed) {
    state.ui.modal_confirmed = false;
    if (state.ui.modal_game_over) {
      state.ui.game_over = true;
      state.ui.modal_active = true;
      state.ui.modal_prompt = "Game Over";
      return;
    }

    reset_player_respawn_state(state);
    return;
  }

  if (state.player.death_timer_ticks > 0) {
    state.player.death_timer_ticks =
        static_cast<std::uint8_t>(state.player.death_timer_ticks - 1U);
    if (state.player.death_timer_ticks == 0) {
      if (state.player.lives > 0) {
        state.player.lives = static_cast<std::uint8_t>(state.player.lives - 1U);
      }
      if (state.player.lives == 0) {
        state.ui.modal_active = true;
        state.ui.modal_prompt = "Game Over";
        state.ui.modal_game_over = true;
        state.ui.game_over = true;
        return;
      }

      state.ui.modal_active = true;
      state.ui.modal_prompt = "Continue?";
      state.ui.modal_game_over = false;
    }
    return;
  }

  if (state.player.lives == 0) {
    state.ui.modal_active = true;
    state.ui.modal_prompt = "Game Over";
    state.ui.modal_game_over = true;
    state.ui.game_over = true;
  }
}

void update_player_mode_cycle(RuntimeState &state) {
  if (!state.input.down_pressed) {
    return;
  }

  const std::uint8_t inventory_mask = state.progression.mode_inventory_mask;
  if (inventory_mask == 0U) {
    state.ui.active_mode_mask = 0U;
    return;
  }

  constexpr std::uint8_t kModeBits[4] = {0x01U, 0x02U, 0x04U, 0x08U};
  const std::uint8_t current_mask = static_cast<std::uint8_t>(
      state.ui.active_mode_mask & 0x0FU);

  std::size_t current_index = 4U;
  for (std::size_t index = 0U; index < 4U; ++index) {
    if (current_mask == kModeBits[index]) {
      current_index = index;
      break;
    }
  }

  if (current_index == 4U) {
    for (std::size_t index = 0U; index < 4U; ++index) {
      if ((inventory_mask & kModeBits[index]) != 0U) {
        state.ui.active_mode_mask = kModeBits[index];
        return;
      }
    }
    state.ui.active_mode_mask = 0U;
    return;
  }

  for (std::size_t step = 0U; step < 4U; ++step) {
    const std::size_t next_index = (current_index + step + 1U) % 4U;
    const std::uint8_t candidate_mask = kModeBits[next_index];
    if ((inventory_mask & candidate_mask) != 0U) {
      state.ui.active_mode_mask = candidate_mask;
      return;
    }
  }

  state.ui.active_mode_mask = 0U;
}

void update_player_mode_activation(RuntimeState &state) {
  if (!state.input.mode_activate_pressed) {
    return;
  }

  constexpr std::uint8_t kModeBitMask = 0x03U;
  const std::uint8_t selected_mode =
      static_cast<std::uint8_t>(state.ui.active_mode_mask & kModeBitMask);
  if (selected_mode == 0U) {
    return;
  }

  state.ui.active_mode_mask = selected_mode;
  state.player.active_mode_effect = selected_mode;
  state.player.mode_effect_ticks = 30U;
  state.flags.timed_overlay_pending = true;
  state.progression.mode_inventory_mask = static_cast<std::uint8_t>(
      state.progression.mode_inventory_mask | selected_mode);
  state.ui.inventory_mask =
      static_cast<std::uint8_t>(state.ui.inventory_mask | 0x04U);
}

namespace {
void expire_mode_effect_if_due(RuntimeState &state) {
  if (state.player.active_mode_effect == 0U) {
    state.player.mode_effect_ticks = 0U;
    return;
  }

  if (state.player.mode_effect_ticks > 0U) {
    state.player.mode_effect_ticks =
        static_cast<std::uint8_t>(state.player.mode_effect_ticks - 1U);
  }

  if (state.player.mode_effect_ticks == 0U) {
    state.player.active_mode_effect = 0U;
  }
}
} // namespace

void update_player_mode_effect(RuntimeState &state) {
  expire_mode_effect_if_due(state);
}

void update_progression_state(RuntimeState &state) {
  constexpr std::uint8_t kGemsFlag = 0x01U;
  constexpr std::uint8_t kFirepowerFlag = 0x02U;
  constexpr std::uint8_t kLivesFlag = 0x04U;

  if (state.player.gems > 0) {
    state.progression.gems_collected = true;
  }
  if (state.player.firepower > 1) {
    state.progression.firepower_unlocked = true;
  }
  if (state.player.lives > 0) {
    state.progression.lives_available = true;
  }

  state.progression.flags = 0U;
  if (state.progression.gems_collected) {
    state.progression.flags |= kGemsFlag;
  }
  if (state.progression.firepower_unlocked) {
    state.progression.flags |= kFirepowerFlag;
  }
  if (state.progression.lives_available) {
    state.progression.flags |= kLivesFlag;
  }

  if (state.progression.gems_collected) {
    state.ui.inventory_mask =
        static_cast<std::uint8_t>(state.ui.inventory_mask | 0x01U);
  }
  if (state.progression.firepower_unlocked) {
    state.ui.inventory_mask =
        static_cast<std::uint8_t>(state.ui.inventory_mask | 0x02U);
  }
  if (state.progression.lives_available) {
    state.ui.active_mode_mask =
        static_cast<std::uint8_t>(state.ui.active_mode_mask | 0x01U);
  }
  if (state.progression.mode_inventory_mask != 0U) {
    state.ui.inventory_mask =
        static_cast<std::uint8_t>(state.ui.inventory_mask | 0x04U);
  }
}

void handle_input_fallback(RuntimeState &state) {
  if (state.flags.room_event_triggered) {
    queue_room_event_message(state);
    return;
  }

  show_pending_room_event_message(state);
  if (state.ui.modal_active && !state.ui.modal_prompt.empty() &&
      state.ui.pending_event_message.empty() &&
      state.ui.modal_prompt == "Room Event Triggered") {
    return;
  }

  if (state.level_complete) {
    if (!state.ui.level_complete_modal) {
      state.ui.modal_active = true;
      state.ui.modal_prompt = "Level Complete!";
      state.ui.modal_game_over = false;
      state.ui.level_complete_modal = true;
      state.flags.player_special_state_active = true;
      state.transition_state.player_frozen = true;
    }
    return;
  }

  if (state.transition_state.active) {
    state.transition_state.player_frozen = true;
    advance_transition_state(state);
    if (!state.transition_state.active) {
      complete_pending_room_transition(state);
    } else {
      room_transition_player_exit_sequence(state);
    }
    return;
  }

  update_entity_behaviors(state);
  apply_entity_combat(state);
  apply_input_tick(state, kDefaultMotion);
  update_player_mode_cycle(state);
  update_player_mode_activation(state);
  update_player_mode_effect(state);
  update_progression_state(state);
  if (state.player.gems >= state.level_completion_gems_required &&
      state.level_completion_gems_required > 0U) {
    state.level_complete = true;
    state.ui.modal_active = true;
    state.ui.modal_prompt = "Level Complete!";
    state.ui.modal_game_over = false;
    state.ui.level_complete_modal = true;
    state.flags.player_special_state_active = true;
    state.transition_state.player_frozen = true;
  }
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
      [](RuntimeState &state) { handle_player_special_state(state); });
  dispatcher.set_input_handling_hook(
      [](RuntimeState &state) { handle_input_fallback(state); });
}

GameDispatcher make_default_game_dispatcher() {
  GameDispatcher dispatcher;
  install_default_stage_hooks(dispatcher);
  return dispatcher;
}

} // namespace comic2
