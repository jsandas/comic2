#include <algorithm>
#include <stdexcept>
#include <vector>

#include "comic2/default_handlers.hpp"
#include "comic2/dispatcher.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

std::vector<std::uint8_t>
encode_literal_signed_rle(const std::vector<std::uint8_t> &bytes) {
  std::vector<std::uint8_t> encoded;
  std::size_t offset = 0;
  while (offset < bytes.size()) {
    const std::size_t chunk = std::min<std::size_t>(127, bytes.size() - offset);
    encoded.push_back(static_cast<std::uint8_t>(chunk));
    encoded.insert(encoded.end(),
                   bytes.begin() + static_cast<std::ptrdiff_t>(offset),
                   bytes.begin() + static_cast<std::ptrdiff_t>(offset + chunk));
    offset += chunk;
  }
  encoded.push_back(0x00);
  return encoded;
}

void test_priority_order() {
  comic2::RuntimeState state;
  comic2::GameDispatcher dispatcher;

  state.flags.level_transition_pending = true;
  state.player.is_airborne = true;
  state.player.is_physics_active = true;
  expect(dispatcher.choose_stage(state) ==
             comic2::DispatchStage::LevelTransition,
         "level transition must have highest priority");

  state.flags.level_transition_pending = false;
  expect(dispatcher.choose_stage(state) ==
             comic2::DispatchStage::AirbornePhysics,
         "airborne should beat grounded physics");

  state.player.is_airborne = false;
  expect(dispatcher.choose_stage(state) ==
             comic2::DispatchStage::GroundedPhysics,
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

  dispatcher.set_input_handling_hook(
      [](comic2::RuntimeState &s) { s.player.jump_counter = 7; });

  const auto result = dispatcher.run_tick(state);
  expect(result.stage == comic2::DispatchStage::InputHandling,
         "run_tick should select input stage");
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
  expect(first.stage == comic2::DispatchStage::InputHandling,
         "first tick should handle input");
  expect(state.player.x == 8, "right input should advance x");
  expect(state.player.y_vel == -5, "jump should apply impulse");
  expect(state.player.is_physics_active, "jump should enable physics");

  state.input.right_pressed = false;
  state.input.jump_pressed = false;

  const auto second = dispatcher.run_tick(state);
  expect(second.stage == comic2::DispatchStage::GroundedPhysics ||
             second.stage == comic2::DispatchStage::AirbornePhysics,
         "second tick should execute physics");
  expect(state.player.y < 0, "physics step should move player upward first");
}

void test_deterministic_tick_replay() {
  auto run_sequence = [](const std::vector<comic2::InputState> &inputs) {
    // Create fresh dispatcher and state for each run
    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);

    comic2::RuntimeState state;
    state.player.jump_counter = 3;
    for (const auto &input : inputs) {
      state.input = input;
      dispatcher.run_tick(state);
    }
    return state;
  };

  const std::vector<comic2::InputState> sequence = {
      comic2::InputState{.jump_pressed = true,
                         .left_pressed = false,
                         .right_pressed = true,
                         .down_pressed = false},
      comic2::InputState{},
      comic2::InputState{},
      comic2::InputState{.jump_pressed = false,
                         .left_pressed = true,
                         .right_pressed = false,
                         .down_pressed = false},
      comic2::InputState{},
      comic2::InputState{},
  };

  // Run the sequence and verify specific expected outcomes
  const auto result = run_sequence(sequence);
  expect(result.player.x != 0,
         "player should have moved horizontally after input sequence");
  expect(result.player.y != 0,
         "player should have moved vertically after jump and physics");

  // Verify determinism: same inputs with fresh dispatcher produce identical
  // state
  const auto replay = run_sequence(sequence);
  expect(result.player.x == replay.player.x, "x mismatch");
  expect(result.player.y == replay.player.y, "y mismatch");
  expect(result.player.x_vel == replay.player.x_vel, "x_vel mismatch");
  expect(result.player.y_vel == replay.player.y_vel, "y_vel mismatch");
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
  expect(dispatcher.trace_log() == expected,
         "trace log should record selected stage per tick");

  dispatcher.clear_trace();
  expect(dispatcher.trace_log().empty(),
         "clear_trace should empty recorded stages");
}

void test_default_stage_hook_coverage() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  const auto expect_stage = [&](comic2::RuntimeState &state,
                                comic2::DispatchStage expected_stage) {
    const auto result = dispatcher.run_tick(state);
    expect(result.stage == expected_stage,
           "stage selection mismatch while checking default hook coverage");
    expect(result.hook_executed,
           "default hook should be installed for every dispatch stage");
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

void test_tile_hazard_stage_instantly_kills_player() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.player.hp = 3;
  state.player.y = -16;
  state.player.is_physics_active = true;

  state.room_grid.tile_w = 1;
  state.room_grid.tile_h = 2;
  state.room_grid.row_pointers = {0, 1};
  state.room_grid.tile_data = {0xF4, 0xF4};

  const auto first = dispatcher.run_tick(state);
  expect(first.stage == comic2::DispatchStage::GroundedPhysics,
         "physics stage should run before hazard stage is selected");
  expect(state.flags.tile_hazard_triggered,
         "grounded physics hook should set tile hazard when on a hazard tile");

  const auto second = dispatcher.run_tick(state);
  expect(second.stage == comic2::DispatchStage::TileHazard,
         "hazard flag should route dispatcher to tile hazard stage");
  expect(state.player.hp == 0,
         "tile hazard handler should kill the player immediately");
  expect(state.flags.player_special_state_active,
         "tile hazard handler should enter the death/special state path");
  expect(!state.flags.tile_hazard_triggered,
         "tile hazard handler should clear hazard flag after handling");
}

void test_stage_flags_are_consumed_by_default_handlers() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  {
    comic2::RuntimeState state;
    state.flags.special_logic1_active = true;
    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::SpecialLogic1,
           "special logic1 stage should run when flagged");
    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::InputHandling,
           "special logic1 flag should be consumed after handler");
  }

  {
    comic2::RuntimeState state;
    state.flags.special_logic2_active = true;
    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::SpecialLogic2,
           "special logic2 stage should run when flagged");
    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::InputHandling,
           "special logic2 flag should be consumed after handler");
  }

  {
    comic2::RuntimeState state;
    state.flags.timed_overlay_pending = true;
    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::TimedOverlay,
           "timed overlay stage should run when flagged");
    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::InputHandling,
           "timed overlay flag should be consumed after handler");
  }

  {
    comic2::RuntimeState state;
    state.player.is_animation_active = true;
    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::PlayerAnimation,
           "player animation stage should run when flagged");
    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::InputHandling,
           "player animation flag should be consumed after handler");
  }

  {
    comic2::RuntimeState state;
    state.player.is_attack_active = true;
    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::AttackAnimation,
           "attack animation stage should run when flagged");
    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::InputHandling,
           "attack animation flag should be consumed after handler");
  }

  {
    comic2::RuntimeState state;
    state.flags.distance_interaction_active = true;
    const auto first = dispatcher.run_tick(state);
    expect(first.stage == comic2::DispatchStage::DistanceInteraction,
           "distance interaction stage should run when flagged");
    const auto second = dispatcher.run_tick(state);
    expect(second.stage == comic2::DispatchStage::InputHandling,
           "distance interaction flag should be consumed after handler");
  }
}

void test_input_fallback_arms_grounded_physics_for_next_tick() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.player.is_airborne = false;
  state.player.is_physics_active = false;

  const auto first = dispatcher.run_tick(state);
  expect(first.stage == comic2::DispatchStage::InputHandling,
         "first tick should run input fallback");
  expect(state.player.is_physics_active,
         "input fallback should arm grounded physics for next tick");

  const auto second = dispatcher.run_tick(state);
  expect(second.stage == comic2::DispatchStage::GroundedPhysics,
         "second tick should execute grounded physics after arming");
}

void test_level_transition_loads_room_tilemap() {
  comic2::RuntimeState state;
  state.current_level = 1;
  state.current_room = 0;
  state.flags.level_transition_pending = true;

  std::vector<std::uint8_t> decoded_room_bytes(0x2C4, 0x00);
  decoded_room_bytes[0x2A0] = 0x00;
  decoded_room_bytes[0x2A1] = 0x00;
  decoded_room_bytes[0x2A2] = 0x04;
  decoded_room_bytes[0x2A3] = 0x00;
  decoded_room_bytes[0x2A4] = 0x08;
  decoded_room_bytes[0x2A5] = 0x00;

  const std::vector<std::uint8_t> encoded_room_bytes =
      encode_literal_signed_rle(decoded_room_bytes);

  state.room_resource_bytes.resize(0x20 + encoded_room_bytes.size(), 0x00);
  state.room_resource_bytes[2] = 0x01;
  state.room_resource_bytes[3] = 0x00;
  state.room_resource_bytes[0x04] = 0x04;
  state.room_resource_bytes[0x05] = 0x00;
       state.room_resource_bytes[0x06] = 0x03;
  state.room_resource_bytes[0x07] = 0x00;
  state.room_resource_bytes[0x08] = 0x20;
  state.room_resource_bytes[0x09] = 0x00;
  std::copy(encoded_room_bytes.begin(), encoded_room_bytes.end(),
            state.room_resource_bytes.begin() + 0x20);

  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  const auto result = dispatcher.run_tick(state);
  expect(result.stage == comic2::DispatchStage::LevelTransition,
         "level transition should select level transition stage");
  expect(!state.flags.level_transition_pending,
         "level transition handler should clear its pending flag");
  expect(state.room_grid.tile_w == 4,
         "level transition should load the new room tile width");
  expect(state.room_grid.tile_h == 3,
         "level transition should load the new room tile height");
  expect(state.room_grid.row_pointers == std::vector<std::uint16_t>{0, 4, 8},
         "level transition should build the row pointer table");
}

comic2::RoomTileGrid make_projectile_floor_grid(std::uint8_t tile_id) {
  comic2::RoomTileGrid grid;
  grid.tile_w = 2;
  grid.tile_h = 2;
  grid.row_pointers = {0, 2};
  grid.tile_data = {0x00, 0x00, tile_id, tile_id};
  return grid;
}

void test_projectile_scripted_tick_updates_deterministically() {
  const auto run_sequence = [](const std::vector<comic2::InputState> &inputs) {
    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);

    comic2::RuntimeState state;
    state.projectiles.push_back(comic2::ProjectileState{.x = 10,
                                                        .y = 10,
                                                        .x_vel = 1,
                                                        .y_vel = 0,
                                                        .anim_frame = 0,
                                                        .active = true});

    for (const auto &input : inputs) {
      state.input = input;
      dispatcher.run_tick(state);
    }

    return state.projectiles;
  };

  const std::vector<comic2::InputState> sequence = {
      comic2::InputState{},
      comic2::InputState{},
      comic2::InputState{},
  };

  const auto first = run_sequence(sequence);
  const auto replay = run_sequence(sequence);

  expect(first.size() == 1, "scripted projectile tick should keep one entry");
  expect(first[0].x == 13, "projectile should advance one pixel per tick");
  expect(first[0].anim_frame == 3,
         "projectile animation frame should advance modulo 8 each tick");
  expect(replay.size() == 1,
         "replayed scripted projectile tick should keep one entry");
  expect(replay[0].x == 13,
         "replayed projectile should advance one pixel per tick");
  expect(
      replay[0].anim_frame == 3,
      "replayed projectile animation frame should advance modulo 8 each tick");
}

void test_projectile_scripted_tick_deactivates_on_tile_collision() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.room_grid = make_projectile_floor_grid(0x01);
  state.projectiles.push_back(comic2::ProjectileState{.x = 13,
                                                      .y = 13,
                                                      .x_vel = 0,
                                                      .y_vel = 0,
                                                      .anim_frame = 0,
                                                      .active = true});

  const auto result = dispatcher.run_tick(state);
  expect(result.stage == comic2::DispatchStage::InputHandling,
         "projectile tick should run through input handling stage");
  expect(!state.projectiles[0].active,
         "projectile should deactivate on tile collision during scripted tick");
}

} // namespace

void run_dispatcher_tests() {
  test_priority_order();
  test_input_fallback();
  test_stage_hook_execution();
  test_default_handlers_basic_movement_and_jump();
  test_deterministic_tick_replay();
  test_dispatcher_trace_log();
  test_default_stage_hook_coverage();
  test_tile_hazard_stage_instantly_kills_player();
  test_stage_flags_are_consumed_by_default_handlers();
  test_input_fallback_arms_grounded_physics_for_next_tick();
  test_level_transition_loads_room_tilemap();
  test_projectile_scripted_tick_updates_deterministically();
  test_projectile_scripted_tick_deactivates_on_tile_collision();
}
