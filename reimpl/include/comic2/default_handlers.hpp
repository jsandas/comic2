#pragma once

#include "comic2/dispatcher.hpp"

namespace comic2 {

void handle_basic_input(RuntimeState& state);
void step_grounded_physics(RuntimeState& state);

void install_default_stage_hooks(GameDispatcher& dispatcher);

}  // namespace comic2
