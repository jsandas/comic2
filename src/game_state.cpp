#include "comic2/game_state.hpp"

namespace comic2 {

RuntimeState make_default_runtime_state() {
  RuntimeState state{};

  state.current_level = 0;
  state.current_room = 0;
  state.player.x = 64;
  state.player.y = 96;
  state.player.is_airborne = false;
  state.player.is_physics_active = true;
  state.player.facing_right = true;
  state.player.jump_counter = 0;
  state.player.hp = 12;
  state.player.firepower = 1;
  state.activation_toggle = 1;

  return state;
}

} // namespace comic2
