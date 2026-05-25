#include <stdexcept>
#include <vector>

#include "comic2/default_handlers.hpp"
#include "comic2/dispatcher.hpp"

namespace {

void expect(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_priority_order() {
    comic2::RuntimeState state;
    comic2::GameDispatcher dispatcher;

    state.flags.level_transition_pending = true;
    state.player.is_airborne = true;
    state.player.is_physics_active = true;
    expect(dispatcher.choose_stage(state) == comic2::DispatchStage::LevelTransition,
           "level transition must have highest priority");

    state.flags.level_transition_pending = false;
    expect(dispatcher.choose_stage(state) == comic2::DispatchStage::AirbornePhysics,
           "airborne should beat grounded physics");

    state.player.is_airborne = false;
    expect(dispatcher.choose_stage(state) == comic2::DispatchStage::GroundedPhysics,
           "grounded physics should trigger when active");
}

void test_input_fallback() {
    comic2::RuntimeState state;
    comic2::GameDispatcher dispatcher;

    expect(dispatcher.choose_stage(state) == comic2::DispatchStage::InputHandling,
           "dispatcher must fall back to input handling");
}

void test_stage_hook_execution() {
    comic2::RuntimeState state;
    comic2::GameDispatcher dispatcher;

    dispatcher.set_input_handling_hook([](comic2::RuntimeState& s) { s.player.jump_counter = 7; });

    const auto result = dispatcher.run_tick(state);
    expect(result.stage == comic2::DispatchStage::InputHandling, "run_tick should select input stage");
    expect(result.hook_executed, "run_tick should report hook execution");
    expect(state.player.jump_counter == 7, "input hook should mutate state");
}

void test_default_handlers_basic_movement_and_jump() {
    comic2::RuntimeState state;
    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);

    state.player.jump_counter = 2;
    state.input.right_pressed = true;
    state.input.jump_pressed = true;

    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::InputHandling, "first tick should handle input");
    expect(state.player.x == 2, "right input should advance x");
    expect(state.player.y_vel == -5, "jump should apply impulse");
    expect(state.player.is_physics_active, "jump should enable physics");

    state.input.right_pressed = false;
    state.input.jump_pressed = false;

    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::GroundedPhysics || second.stage == comic2::DispatchStage::AirbornePhysics,
           "second tick should execute physics");
    expect(state.player.y < 0, "physics step should move player upward first");
}

void test_deterministic_tick_replay() {
    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);

    auto run_sequence = [&](const std::vector<comic2::InputState>& inputs) {
        comic2::RuntimeState state;
        state.player.jump_counter = 3;
        for (const auto& input : inputs) {
            state.input = input;
            dispatcher.run_tick(state);
        }
        return state;
    };

    const std::vector<comic2::InputState> sequence = {
        comic2::InputState{.jump_pressed = true, .left_pressed = false, .right_pressed = true, .down_pressed = false},
        comic2::InputState{},
        comic2::InputState{},
        comic2::InputState{.jump_pressed = false, .left_pressed = true, .right_pressed = false, .down_pressed = false},
        comic2::InputState{},
        comic2::InputState{},
    };

    const auto a = run_sequence(sequence);
    const auto b = run_sequence(sequence);
    expect(a == b, "same input sequence should produce identical runtime state");
}

void test_dispatcher_trace_log() {
    comic2::GameDispatcher dispatcher;
    comic2::RuntimeState state;

    dispatcher.set_trace_enabled(true);

    state.flags.level_transition_pending = true;
    dispatcher.run_tick(state);

    state.flags.level_transition_pending = false;
    state.player.is_physics_active = true;
    dispatcher.run_tick(state);

    state.player.is_physics_active = false;
    dispatcher.run_tick(state);

    const std::vector<comic2::DispatchStage> expected = {
        comic2::DispatchStage::LevelTransition,
        comic2::DispatchStage::GroundedPhysics,
        comic2::DispatchStage::InputHandling,
    };
    expect(dispatcher.trace_log() == expected, "trace log should record selected stage per tick");

    dispatcher.clear_trace();
    expect(dispatcher.trace_log().empty(), "clear_trace should empty recorded stages");
}

void test_default_stage_hook_coverage() {
    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);

    const auto expect_stage = [&](comic2::RuntimeState& state, comic2::DispatchStage expected_stage) {
        const auto result = dispatcher.run_tick(state);
        expect(result.stage == expected_stage, "stage selection mismatch while checking default hook coverage");
        expect(result.hook_executed, "default hook should be installed for every dispatch stage");
    };

    {
        comic2::RuntimeState state;
        state.flags.level_transition_pending = true;
        expect_stage(state, comic2::DispatchStage::LevelTransition);
    }
    {
        comic2::RuntimeState state;
        state.flags.special_logic1_active = true;
        expect_stage(state, comic2::DispatchStage::SpecialLogic1);
    }
    {
        comic2::RuntimeState state;
        state.flags.special_logic2_active = true;
        expect_stage(state, comic2::DispatchStage::SpecialLogic2);
    }
    {
        comic2::RuntimeState state;
        state.player.is_airborne = true;
        expect_stage(state, comic2::DispatchStage::AirbornePhysics);
    }
    {
        comic2::RuntimeState state;
        state.flags.timed_overlay_pending = true;
        expect_stage(state, comic2::DispatchStage::TimedOverlay);
    }
    {
        comic2::RuntimeState state;
        state.player.is_physics_active = true;
        expect_stage(state, comic2::DispatchStage::GroundedPhysics);
    }
    {
        comic2::RuntimeState state;
        state.player.is_animation_active = true;
        expect_stage(state, comic2::DispatchStage::PlayerAnimation);
    }
    {
        comic2::RuntimeState state;
        state.player.is_attack_active = true;
        expect_stage(state, comic2::DispatchStage::AttackAnimation);
    }
    {
        comic2::RuntimeState state;
        state.flags.distance_interaction_active = true;
        expect_stage(state, comic2::DispatchStage::DistanceInteraction);
    }
    {
        comic2::RuntimeState state;
        state.flags.tile_hazard_triggered = true;
        expect_stage(state, comic2::DispatchStage::TileHazard);
    }
    {
        comic2::RuntimeState state;
        state.flags.player_special_state_active = true;
        expect_stage(state, comic2::DispatchStage::PlayerSpecialState);
    }
    {
        comic2::RuntimeState state;
        expect_stage(state, comic2::DispatchStage::InputHandling);
    }
}

    void test_tile_hazard_stage_consumes_hp() {
        comic2::GameDispatcher dispatcher;
        comic2::install_default_stage_hooks(dispatcher);

        comic2::RuntimeState state;
        state.player.hp = 3;
        state.player.is_physics_active = true;

        state.room_grid.tile_w = 1;
        state.room_grid.tile_h = 1;
        state.room_grid.row_pointers = {0};
        state.room_grid.tile_data = {0xF4};

        const auto first = dispatcher.run_tick(state);
        expect(first.stage == comic2::DispatchStage::GroundedPhysics,
            "physics stage should run before hazard stage is selected");
        expect(state.flags.tile_hazard_triggered,
            "grounded physics hook should set tile hazard when on a hazard tile");

        const auto second = dispatcher.run_tick(state);
        expect(second.stage == comic2::DispatchStage::TileHazard,
            "hazard flag should route dispatcher to tile hazard stage");
        expect(state.player.hp == 2, "tile hazard handler should decrement hp once");
        expect(!state.flags.tile_hazard_triggered,
            "tile hazard handler should clear hazard flag after handling");
    }

}  // namespace

void run_dispatcher_tests() {
    test_priority_order();
    test_input_fallback();
    test_stage_hook_execution();
    test_default_handlers_basic_movement_and_jump();
    test_deterministic_tick_replay();
    test_dispatcher_trace_log();
    test_default_stage_hook_coverage();
    test_tile_hazard_stage_consumes_hp();
}
