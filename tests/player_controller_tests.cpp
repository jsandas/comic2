#include <stdexcept>

#include "comic2/player_controller.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

comic2::RoomTileGrid make_flat_floor_grid() {
  comic2::RoomTileGrid grid;
  grid.tile_w = 2;
  grid.tile_h = 2;
  grid.row_pointers = {0, 2};
  grid.tile_data = {
      0x00,
      0x00,
      0x02,
      0x02,
  };
  return grid;
}

comic2::RoomTileGrid make_three_row_floor_grid() {
  comic2::RoomTileGrid grid;
  grid.tile_w = 2;
  grid.tile_h = 4;
  grid.row_pointers = {0, 2, 4, 6};
  grid.tile_data = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  };
  return grid;
}

void test_apply_input_tick_moves_right() {
  comic2::RuntimeState state;
  state.input.right_pressed = true;

  comic2::apply_input_tick(state, comic2::PlayerMotionConfig{});

  expect(state.player.x == 8, "right input should move x by walk step");
  expect(state.player.x_vel == 8, "right input should set x_vel");
}

void test_apply_input_tick_consumes_jump_counter() {
  comic2::RuntimeState state;
  state.input.jump_pressed = true;
  state.player.jump_counter = 1;

  comic2::apply_input_tick(state, comic2::PlayerMotionConfig{});

  expect(state.player.y_vel == -5, "jump input should apply jump impulse");
  expect(state.player.jump_counter == 0,
         "jump input should consume jump counter");
  expect(state.player.is_physics_active, "jump input should activate physics");
  expect(state.player.is_airborne, "jump input should enter airborne state");
}

void test_apply_physics_tick_lands_on_ground() {
  comic2::RuntimeState state;
  state.room_grid = make_flat_floor_grid();
  state.player.y = -3;
  state.player.y_vel = 2;
  state.player.is_airborne = true;
  state.player.is_physics_active = true;

  comic2::apply_physics_tick(state, comic2::PlayerMotionConfig{},
                             comic2::TileCollisionConfig{});

  expect(state.player.y == -16,
         "physics tick should snap player to tile floor");
  expect(state.player.y_vel == 0,
         "physics tick should zero vertical velocity on ground");
  expect(!state.player.is_airborne,
         "physics tick should clear airborne on ground contact");
  expect(!state.player.is_physics_active,
         "physics tick should clear physics active on ground contact");
}

void test_jump_while_moving_left_and_right() {
  comic2::RuntimeState left_state;
  left_state.input.left_pressed = true;
  left_state.input.jump_pressed = true;
  left_state.player.jump_counter = 1;

  comic2::apply_input_tick(left_state, comic2::PlayerMotionConfig{});

  expect(left_state.player.x == -8, "left+jump should move left by walk step");
  expect(left_state.player.x_vel == -8,
         "left+jump should set negative x velocity");
  expect(left_state.player.y_vel == -5, "left+jump should apply jump impulse");
  expect(left_state.player.jump_counter == 0,
         "left+jump should consume jump counter");

  comic2::RuntimeState right_state;
  right_state.input.right_pressed = true;
  right_state.input.jump_pressed = true;
  right_state.player.jump_counter = 1;

  comic2::apply_input_tick(right_state, comic2::PlayerMotionConfig{});

  expect(right_state.player.x == 8,
         "right+jump should move right by walk step");
  expect(right_state.player.x_vel == 8,
         "right+jump should set positive x velocity");
  expect(right_state.player.y_vel == -5,
         "right+jump should apply jump impulse");
  expect(right_state.player.jump_counter == 0,
         "right+jump should consume jump counter");
}

void test_short_hop_vs_full_jump_counter_usage() {
  comic2::RuntimeState short_hop;
  short_hop.player.jump_counter = 2;
  short_hop.input.jump_pressed = true;

  comic2::apply_input_tick(short_hop, comic2::PlayerMotionConfig{});
  short_hop.input.jump_pressed = false;
  comic2::apply_airborne_physics_tick(short_hop, comic2::PlayerMotionConfig{},
                                      comic2::TileCollisionConfig{});

  expect(short_hop.player.jump_counter == 1,
         "short-hop should consume exactly one jump counter unit");

  comic2::RuntimeState full_jump;
  full_jump.player.jump_counter = 2;
  full_jump.input.jump_pressed = true;

  comic2::apply_input_tick(full_jump, comic2::PlayerMotionConfig{});
  comic2::apply_airborne_physics_tick(full_jump, comic2::PlayerMotionConfig{},
                                      comic2::TileCollisionConfig{});

  expect(full_jump.player.jump_counter == 0,
         "holding jump during airborne rise should consume additional jump "
         "counter");
}

void test_ledge_walk_off_transitions_into_fall() {
  comic2::RuntimeState state;
  state.player.x = 4;
  state.player.y = 0;
  state.player.is_airborne = false;
  state.player.is_physics_active = true;
  state.input.right_pressed = true;

  const comic2::TileCollisionConfig collision{};

  comic2::apply_input_tick(state, comic2::PlayerMotionConfig{});
  expect(state.player.x == 12,
         "input tick should move player past platform edge");

  comic2::apply_grounded_physics_tick(state, comic2::PlayerMotionConfig{},
                                      collision);

  expect(state.player.is_airborne,
         "walking off ledge should enter airborne state");
  expect(state.player.is_physics_active,
         "walking off ledge should keep physics active");
  expect(state.player.y_vel > 0,
         "walking off ledge should start downward fall velocity");
}

void test_grounded_physics_respects_tile_height_without_ground_y() {
  comic2::RuntimeState state;
  state.room_grid = make_three_row_floor_grid();
  state.player.x = 0;
  state.player.y = 18;
  state.player.y_vel = 0;
  state.player.is_airborne = false;
  state.player.is_physics_active = true;

  const comic2::TileCollisionConfig collision{
      .solid_tile_threshold = 2,
  };

  comic2::apply_grounded_physics_tick(state, comic2::PlayerMotionConfig{},
                                      collision);

  expect(state.player.y == 16,
         "grounded physics should align player to the actual tile top");
  expect(state.player.y_vel == 0,
         "grounded physics should zero vertical velocity after snapping");
  expect(!state.player.is_airborne,
         "grounded physics should clear airborne state when supported");
  expect(!state.player.is_physics_active,
         "grounded physics should clear physics-active state when supported");
}

void test_grounded_physics_does_not_snap_when_rising() {
  comic2::RuntimeState state;
  state.room_grid = make_three_row_floor_grid();
  state.player.x = 0;
  state.player.y = 0;
  state.player.y_vel = -3;
  state.player.is_airborne = false;
  state.player.is_physics_active = true;

  const comic2::TileCollisionConfig collision{
      .solid_tile_threshold = 2,
  };

  comic2::apply_grounded_physics_tick(state, comic2::PlayerMotionConfig{},
                                      collision);

  expect(state.player.y == 0,
         "rising motion should preserve player y when not landing");
  expect(state.player.y_vel == -3,
         "rising motion should preserve upward velocity");
  expect(state.player.is_airborne,
         "rising motion should remain in airborne state");
  expect(state.player.is_physics_active,
         "rising motion should keep physics active");
}

} // namespace

void run_player_controller_tests() {
  test_apply_input_tick_moves_right();
  test_apply_input_tick_consumes_jump_counter();
  test_apply_physics_tick_lands_on_ground();
  test_jump_while_moving_left_and_right();
  test_short_hop_vs_full_jump_counter_usage();
  test_ledge_walk_off_transitions_into_fall();
  test_grounded_physics_respects_tile_height_without_ground_y();
  test_grounded_physics_does_not_snap_when_rising();
}
