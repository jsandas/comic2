#pragma once

#include "comic2/dispatcher.hpp"

namespace comic2 {

void handle_level_transition(RuntimeState& state);
void handle_special_logic1(RuntimeState& state);
void handle_special_logic2(RuntimeState& state);
void handle_airborne_movement(RuntimeState& state);
void handle_timed_overlay(RuntimeState& state);
void handle_grounded_physics(RuntimeState& state);
void handle_player_animation(RuntimeState& state);
void handle_attack_animation(RuntimeState& state);
void handle_distance_interaction(RuntimeState& state);
void handle_tile_hazard(RuntimeState& state);
void handle_player_special_state(RuntimeState& state);
void handle_input_fallback(RuntimeState& state);

void install_default_stage_hooks(GameDispatcher& dispatcher);

}  // namespace comic2
