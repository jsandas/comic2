#include <array>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "comic2/default_handlers.hpp"
#include "comic2/dispatcher.hpp"
#include "comic2/projectiles.hpp"
#include "comic2/renderer.hpp"
#include "comic2/renderer_validation.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

struct StateSnapshot {
  std::int16_t x;
  std::int16_t y;
  std::int16_t x_vel;
  std::int16_t y_vel;
  bool is_airborne;
  bool is_physics_active;
  bool tile_hazard_triggered;

  bool operator==(const StateSnapshot &) const = default;
};

comic2::RoomTileGrid make_single_tile_grid(std::uint8_t tile_id) {
  comic2::RoomTileGrid grid;
  grid.tile_w = 1;
  grid.tile_h = 1;
  grid.row_pointers = {0};
  grid.tile_data = {tile_id};
  return grid;
}

comic2::RoomTileGrid make_hazard_floor_grid(std::uint8_t tile_id) {
  comic2::RoomTileGrid grid;
  grid.tile_w = 1;
  grid.tile_h = 2;
  grid.row_pointers = {0, 1};
  grid.tile_data = {tile_id, tile_id};
  return grid;
}

StateSnapshot capture_snapshot(const comic2::RuntimeState &state) {
  return StateSnapshot{
      .x = state.player.x,
      .y = state.player.y,
      .x_vel = state.player.x_vel,
      .y_vel = state.player.y_vel,
      .is_airborne = state.player.is_airborne,
      .is_physics_active = state.player.is_physics_active,
      .tile_hazard_triggered = state.flags.tile_hazard_triggered,
  };
}

std::uint64_t make_room_fixture_alpha_hash() {
  comic2::EgaPlanarSurface frame(320, 200);
  frame.clear(0x00);

  const auto base = comic2::validation::make_8x8_solid_image(0x11, 0x22, 0x44,
                                                              0x88);
  const auto overlay = comic2::validation::make_8x8_solid_image(
      0x0F, 0xF0, 0x33, 0xCC);

  comic2::gfx_rle_blit_opaque_4plane(frame, 0, 0, base);
  comic2::gfx_rle_blit_masked_or_4plane(frame, 16, 4, overlay);

  return comic2::validation::hash_surface_planes(frame);
}

std::uint64_t make_room_fixture_beta_hash() {
  comic2::EgaPlanarSurface frame(320, 200);
  frame.clear(0xAA);

  const auto patterned = comic2::validation::make_16x16_patterned_image();
  const auto overlay = comic2::validation::make_8x8_solid_image(0x01, 0x02,
                                                                 0x04, 0x08);

  comic2::gfx_rle_blit_opaque_4plane(frame, 32, 10, patterned);
  comic2::gfx_rle_blit_masked_or_4plane(frame, 32, 10, overlay);

  return comic2::validation::hash_surface_planes(frame);
}

void expect_hash_eq(std::uint64_t actual, std::uint64_t expected,
                    const char *context) {
  if (actual == expected) {
    return;
  }

  std::ostringstream oss;
  oss << context << " expected hash 0x" << std::hex << expected
      << " but got 0x" << actual;
  throw std::runtime_error(oss.str());
}

void test_gate_b_scripted_trace_matches_oracle() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);
  dispatcher.set_trace_enabled(true);

  comic2::RuntimeState state;
  state.player.jump_counter = 2;

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

  for (const auto &input : sequence) {
    state.input = input;
    dispatcher.run_tick(state);
  }

  const std::vector<comic2::DispatchStage> expected = {
      comic2::DispatchStage::InputHandling,
      comic2::DispatchStage::AirbornePhysics,
      comic2::DispatchStage::AirbornePhysics,
      comic2::DispatchStage::AirbornePhysics,
      comic2::DispatchStage::AirbornePhysics,
      comic2::DispatchStage::AirbornePhysics,
  };
  expect(dispatcher.trace_log() == expected,
         "Gate B oracle trace mismatch for scripted input sequence");

  comic2::GameDispatcher replay_dispatcher;
  comic2::install_default_stage_hooks(replay_dispatcher);
  replay_dispatcher.set_trace_enabled(true);

  comic2::RuntimeState replay_state;
  replay_state.player.jump_counter = 2;
  for (const auto &input : sequence) {
    replay_state.input = input;
    replay_dispatcher.run_tick(replay_state);
  }

  expect(replay_dispatcher.trace_log() == dispatcher.trace_log(),
         "Gate B replay trace must be stable");
}

void test_gate_c_state_vectors_match_snapshots() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.player.jump_counter = 1;

  const std::vector<comic2::InputState> sequence = {
      comic2::InputState{.jump_pressed = true,
                         .left_pressed = false,
                         .right_pressed = true,
                         .down_pressed = false},
      comic2::InputState{},
      comic2::InputState{},
      comic2::InputState{},
      comic2::InputState{},
  };

  std::vector<StateSnapshot> snapshots;
  snapshots.reserve(sequence.size());

  for (const auto &input : sequence) {
    state.input = input;
    dispatcher.run_tick(state);
    snapshots.push_back(capture_snapshot(state));
  }

  const std::vector<StateSnapshot> expected = {
      StateSnapshot{.x = 2,
                    .y = 0,
                    .x_vel = 2,
                    .y_vel = -5,
                    .is_airborne = true,
                    .is_physics_active = true,
                    .tile_hazard_triggered = false},
      StateSnapshot{.x = 2,
                    .y = -5,
                    .x_vel = 0,
                    .y_vel = -4,
                    .is_airborne = true,
                    .is_physics_active = true,
                    .tile_hazard_triggered = false},
      StateSnapshot{.x = 2,
                    .y = -9,
                    .x_vel = 0,
                    .y_vel = -3,
                    .is_airborne = true,
                    .is_physics_active = true,
                    .tile_hazard_triggered = false},
      StateSnapshot{.x = 2,
                    .y = -12,
                    .x_vel = 0,
                    .y_vel = -2,
                    .is_airborne = true,
                    .is_physics_active = true,
                    .tile_hazard_triggered = false},
      StateSnapshot{.x = 2,
                    .y = -14,
                    .x_vel = 0,
                    .y_vel = -1,
                    .is_airborne = true,
                    .is_physics_active = true,
                    .tile_hazard_triggered = false},
  };

  expect(snapshots == expected,
         "Gate C state snapshots do not match expected per-tick vectors");
}

void test_gate_c_hazard_flag_snapshot_progression() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.player.hp = 2;
  state.player.is_physics_active = true;
  state.room_grid = make_hazard_floor_grid(0xF4);

  const auto first = dispatcher.run_tick(state);
  expect(first.stage == comic2::DispatchStage::GroundedPhysics,
         "Gate C hazard setup tick should run grounded physics");
  expect(state.flags.tile_hazard_triggered,
         "Gate C hazard flag should be set after grounded physics tick");

  const auto second = dispatcher.run_tick(state);
  expect(second.stage == comic2::DispatchStage::TileHazard,
         "Gate C hazard resolution tick should route to tile hazard stage");
  expect(state.player.hp == 1, "Gate C hazard tick should decrement hp");
  expect(!state.flags.tile_hazard_triggered,
         "Gate C hazard tick should clear hazard flag");
}

void test_gate_d_room_fixture_alpha_hash_matches_oracle() {
  // Oracle capture hash for the deterministic room fixture alpha.
  constexpr std::uint64_t kOracleHash = 0x16e2201b96f48c65ULL;
  const auto actual = make_room_fixture_alpha_hash();
  expect_hash_eq(actual, kOracleHash, "Gate D alpha room hash");
}

void test_gate_d_room_fixture_beta_hash_matches_oracle() {
  // Oracle capture hash for the deterministic room fixture beta.
  constexpr std::uint64_t kOracleHash = 0x22ecbb9529f6165ULL;
  const auto actual = make_room_fixture_beta_hash();
  expect_hash_eq(actual, kOracleHash, "Gate D beta room hash");
}

void test_gate_e_hazard_death_routes_to_special_state() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.player.hp = 1;
  state.flags.tile_hazard_triggered = true;

  const auto hazard = dispatcher.run_tick(state);
  expect(hazard.stage == comic2::DispatchStage::TileHazard,
         "Gate E should route to tile hazard stage when hazard flag is set");
  expect(state.player.hp == 0, "Gate E hazard should reduce hp to zero");
  expect(state.flags.player_special_state_active,
         "Gate E death should set special state flag");

  const auto special = dispatcher.run_tick(state);
  expect(special.stage == comic2::DispatchStage::PlayerSpecialState,
         "Gate E death path should transition into player special stage");
}

void test_gate_e_room_transition_boundary_sequence() {
  comic2::GameDispatcher dispatcher;
  comic2::install_default_stage_hooks(dispatcher);

  comic2::RuntimeState state;
  state.current_room = 3;
  state.player.x = 319;
  state.input.right_pressed = true;

  const auto input_tick = dispatcher.run_tick(state);
  expect(input_tick.stage == comic2::DispatchStage::InputHandling,
         "Gate E boundary setup tick should execute input stage");
  expect(state.flags.level_transition_pending,
         "Gate E boundary crossing should mark level transition pending");
  expect(state.current_room == 4,
         "Gate E boundary crossing should advance current_room");
  expect(state.player.x == 0,
         "Gate E right boundary crossing should wrap x to left edge");

  state.input = comic2::InputState{};
  const auto transition_tick = dispatcher.run_tick(state);
  expect(transition_tick.stage == comic2::DispatchStage::LevelTransition,
         "Gate E transition tick should execute level transition stage");
  expect(!state.flags.level_transition_pending,
         "Gate E level transition hook should clear pending flag");
}

  void test_gate_e_left_boundary_clamps_at_room_zero() {
    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);

    comic2::RuntimeState state;
    state.current_room = 0;
    state.player.x = 0;
    state.input.left_pressed = true;

    const auto tick = dispatcher.run_tick(state);
    expect(tick.stage == comic2::DispatchStage::InputHandling,
      "Gate E left boundary clamp should run via input stage");
    expect(state.player.x == 0,
      "Gate E left boundary at room zero should clamp player x to zero");
    expect(state.current_room == 0,
      "Gate E left boundary at room zero should not change room index");
    expect(!state.flags.level_transition_pending,
      "Gate E left boundary at room zero should not set transition pending");
  }

void test_gate_e_projectile_collision_outcome() {
  std::vector<comic2::ProjectileState> projectiles;
  comic2::spawn_projectile(projectiles, 0, 0, 0, 0);

  comic2::RoomTileGrid grid = make_single_tile_grid(0x02);

  comic2::update_projectiles(
      projectiles,
      comic2::ProjectileBounds{
          .min_x = 0,
          .max_x = 319,
          .min_y = 0,
          .max_y = 199,
      },
      grid, 0, 0);

  expect(projectiles.size() == 1,
         "Gate E projectile interaction should keep projectile storage stable");
  expect(!projectiles[0].active,
         "Gate E projectile should deactivate on tile collision interaction");
}

} // namespace

void run_integration_gate_tests() {
  test_gate_b_scripted_trace_matches_oracle();

  test_gate_c_state_vectors_match_snapshots();
  test_gate_c_hazard_flag_snapshot_progression();

  test_gate_d_room_fixture_alpha_hash_matches_oracle();
  test_gate_d_room_fixture_beta_hash_matches_oracle();

  test_gate_e_hazard_death_routes_to_special_state();
  test_gate_e_room_transition_boundary_sequence();
  test_gate_e_left_boundary_clamps_at_room_zero();
  test_gate_e_projectile_collision_outcome();
}
