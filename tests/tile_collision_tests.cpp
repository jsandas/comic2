#include <optional>
#include <stdexcept>
#include <vector>

#include "comic2/room_loader.hpp"
#include "comic2/tile_collision.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

comic2::RoomTileGrid make_grid_fixture() {
  comic2::RoomTileGrid grid;
  grid.tile_w = 4;
  grid.tile_h = 3;
  grid.row_pointers = {10, 20, 30};
  grid.tile_data.assign(64, 0);

  grid.tile_data[10] = 1;
  grid.tile_data[11] = 2;
  grid.tile_data[12] = 3;
  grid.tile_data[13] = 4;

  grid.tile_data[20] = 5;
  grid.tile_data[21] = 6;
  grid.tile_data[22] = 7;
  grid.tile_data[23] = 8;

  grid.tile_data[30] = 9;
  grid.tile_data[31] = 10;
  grid.tile_data[32] = 11;
  grid.tile_data[33] = 12;
  return grid;
}

void test_get_tile_at_pixels_maps_to_expected_tile_id() {
  const comic2::RoomTileGrid grid = make_grid_fixture();
  std::optional<std::uint8_t> tile_id = comic2::get_tile_at_pixels(grid, 0, 0);

  expect(tile_id.has_value(), "query at origin should succeed");
  expect(*tile_id == 1, "origin pixel should map to first tile");

  tile_id = comic2::get_tile_at_pixels(grid, 16, 0);
  expect(tile_id.has_value(), "query at next tile x should succeed");
  expect(*tile_id == 2, "x=16 should map to tile_x=1");

  tile_id = comic2::get_tile_at_pixels(grid, 63, 47);
  expect(tile_id.has_value(), "bottom-right in bounds should succeed");
  expect(*tile_id == 12, "bottom-right pixel should map to final tile");
}

void test_get_tile_at_pixels_rejects_out_of_bounds() {
  const comic2::RoomTileGrid grid = make_grid_fixture();

  expect(!comic2::get_tile_at_pixels(grid, -1, 0).has_value(),
    "negative x should fail");
  expect(!comic2::get_tile_at_pixels(grid, 0, -1).has_value(),
    "negative y should fail");
  expect(!comic2::get_tile_at_pixels(grid, 64, 0).has_value(),
    "x at width boundary should fail");
  expect(!comic2::get_tile_at_pixels(grid, 0, 48).has_value(),
    "y at height boundary should fail");
  expect(!comic2::get_tile_at_pixels(grid, 63, 48).has_value(),
    "bottom-right corner out of bounds should fail");
}

void test_has_floor_support_tile_grid_only() {
  comic2::RuntimeState state;
  state.room_grid = make_grid_fixture();
  state.player.x = 0;
  comic2::TileCollisionConfig config;
  config.solid_tile_threshold = 2;

  // feet probe at y=31 maps to tile row 1, id=5 (solid)
  state.player.y = 16 - 1;
  expect(comic2::has_floor_support(state, config),
    "should have support on solid tile");

  // feet probe at y=47 maps to tile row 2, id=9 (solid)
  state.player.y = 32 - 1;
  expect(comic2::has_floor_support(state, config),
    "should have support on lower solid tile");

  // feet probe at y=0 maps to tile row 0, id=1 (below threshold)
  state.player.y = -16;
  expect(!comic2::has_floor_support(state, config),
    "should not have support below threshold");

  // out of bounds below grid
  state.player.y = 48;
  expect(!comic2::has_floor_support(state, config),
    "should not have support out of bounds");
}

void test_resolve_ground_contact_snaps_to_tile_top() {
  comic2::RuntimeState state;
  state.room_grid = make_grid_fixture();
  state.player.x = 0;
  state.player.y = 2;
  state.player.y_vel = 3;
  state.player.is_airborne = true;
  state.player.is_physics_active = true;

  comic2::TileCollisionConfig config;
  config.solid_tile_threshold = 2;

  const bool landed = comic2::resolve_ground_contact(state, config);
  expect(landed, "resolve_ground_contact should land on solid support");
  expect(state.player.y == 0,
    "resolve_ground_contact should align player to tile top");
  expect(state.player.y_vel == 0,
    "resolve_ground_contact should clear vertical velocity");
  expect(!state.player.is_airborne,
    "resolve_ground_contact should clear airborne state");
  expect(!state.player.is_physics_active,
    "resolve_ground_contact should clear physics-active state");
}

void test_room_loader_builds_row_pointer_table() {
  std::vector<std::uint8_t> bytes(0x2C4, 0);

  bytes[0x2A0] = 0xB0;
  bytes[0x2A1] = 0x02;
  bytes[0x2A2] = 0xC0;
  bytes[0x2A3] = 0x02;

  const std::optional<std::vector<std::uint16_t>> row_pointers =
      comic2::build_room_row_pointer_table(bytes, 2);

  expect(row_pointers.has_value(), "row pointer table build should succeed");
  expect(row_pointers->size() == 2, "row pointer count should match tile_h");
  expect((*row_pointers)[0] == 0x02B0, "first row pointer mismatch");
  expect((*row_pointers)[1] == 0x02C0, "second row pointer mismatch");
}

void test_tile_threshold_and_hazard_checks() {
  expect(comic2::tile_meets_threshold(7, 7),
    "tile equal to threshold should pass");
  expect(comic2::tile_meets_threshold(8, 7),
    "tile above threshold should pass");
  expect(!comic2::tile_meets_threshold(6, 7),
    "tile below threshold should fail");

  comic2::TileCollisionConfig config;
  config.hazard_tile_min = 0xA0;
  config.hazard_tile_max = 0xAF;

  expect(!comic2::is_hazard_tile(0x9F, config),
    "below hazard band should fail");
  expect(comic2::is_hazard_tile(0xA0, config),
    "hazard band lower bound should pass");
  expect(comic2::is_hazard_tile(0xA7, config),
    "inside hazard band should pass");
  expect(comic2::is_hazard_tile(0xAF, config),
    "hazard band upper bound should pass");
  expect(!comic2::is_hazard_tile(0xB0, config),
    "above hazard band should fail");
}

void test_update_player_hazard_state_checks_feet_for_hazards() {
  comic2::RuntimeState state;
  state.room_grid = make_grid_fixture();
  state.player.x = 0;
  state.player.y = 16 - 1;

  comic2::TileCollisionConfig config;
  config.hazard_tile_min = 5;
  config.hazard_tile_max = 5;

  state.room_grid.tile_data[20] = 5;
  state.flags.tile_hazard_triggered = false;

  expect(comic2::update_player_hazard_state(state, config),
    "hazard should be triggered by tile under player feet");
  expect(state.flags.tile_hazard_triggered,
    "tile hazard flag should be set when feet probe is hazardous");
}

} // namespace

void run_tile_collision_tests() {
  test_get_tile_at_pixels_maps_to_expected_tile_id();
  test_get_tile_at_pixels_rejects_out_of_bounds();
  test_room_loader_builds_row_pointer_table();
  test_tile_threshold_and_hazard_checks();
  test_has_floor_support_tile_grid_only();
  test_resolve_ground_contact_snaps_to_tile_top();
}
