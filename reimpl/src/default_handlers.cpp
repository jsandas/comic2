#include "comic2/default_handlers.hpp"

#include "comic2/player_controller.hpp"
#include "comic2/tile_collision.hpp"

namespace comic2 {

void handle_basic_input(RuntimeState& state) {
    static constexpr PlayerMotionConfig kDefaultMotion{};
    apply_input_tick(state, kDefaultMotion);
}

void step_grounded_physics(RuntimeState& state) {
    static constexpr PlayerMotionConfig kDefaultMotion{};
    static constexpr TileCollisionConfig kDefaultCollision{};
    apply_physics_tick(state, kDefaultMotion, kDefaultCollision);
}

void install_default_stage_hooks(GameDispatcher& dispatcher) {
    dispatcher.set_input_handling_hook([](RuntimeState& state) { handle_basic_input(state); });
    dispatcher.set_grounded_physics_hook([](RuntimeState& state) { step_grounded_physics(state); });
    dispatcher.set_airborne_physics_hook([](RuntimeState& state) { step_grounded_physics(state); });
}

}  // namespace comic2
