#include <stdexcept>
#include <vector>

#include "comic2/room_loader.hpp"
#include "comic2/tile_collision.hpp"

namespace {

void expect(bool condition, const char* message) {
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
    std::uint8_t tile_id = 0;

    expect(comic2::get_tile_at_pixels(grid, 0, 0, &tile_id), "query at origin should succeed");
    expect(tile_id == 1, "origin pixel should map to first tile");

    expect(comic2::get_tile_at_pixels(grid, 16, 0, &tile_id), "query at next tile x should succeed");
    expect(tile_id == 2, "x=16 should map to tile_x=1");

    expect(comic2::get_tile_at_pixels(grid, 63, 47, &tile_id), "bottom-right in bounds should succeed");
    expect(tile_id == 12, "bottom-right pixel should map to final tile");
}

void test_get_tile_at_pixels_rejects_out_of_bounds() {
    const comic2::RoomTileGrid grid = make_grid_fixture();
    std::uint8_t tile_id = 0;

    expect(!comic2::get_tile_at_pixels(grid, -1, 0, &tile_id), "negative x should fail");
    expect(!comic2::get_tile_at_pixels(grid, 0, -1, &tile_id), "negative y should fail");
    expect(!comic2::get_tile_at_pixels(grid, 64, 0, &tile_id), "x at width boundary should fail");
    expect(!comic2::get_tile_at_pixels(grid, 0, 48, &tile_id), "y at height boundary should fail");
}

void test_room_loader_builds_row_pointer_table() {
    std::vector<std::uint8_t> bytes(0x2C4, 0);

    bytes[0x2A0] = 0xB0;
    bytes[0x2A1] = 0x02;
    bytes[0x2A2] = 0xC0;
    bytes[0x2A3] = 0x02;

    std::vector<std::uint16_t> row_pointers;
    const bool ok = comic2::build_room_row_pointer_table(bytes, 2, &row_pointers);

    expect(ok, "row pointer table build should succeed");
    expect(row_pointers.size() == 2, "row pointer count should match tile_h");
    expect(row_pointers[0] == 0x02B0, "first row pointer mismatch");
    expect(row_pointers[1] == 0x02C0, "second row pointer mismatch");
}

void test_tile_threshold_and_hazard_checks() {
    expect(comic2::tile_meets_threshold(7, 7), "tile equal to threshold should pass");
    expect(comic2::tile_meets_threshold(8, 7), "tile above threshold should pass");
    expect(!comic2::tile_meets_threshold(6, 7), "tile below threshold should fail");

    comic2::TileCollisionConfig config;
    config.hazard_tile_min = 0xA0;
    config.hazard_tile_max = 0xAF;

    expect(!comic2::is_hazard_tile(0x9F, config), "below hazard band should fail");
    expect(comic2::is_hazard_tile(0xA0, config), "hazard band lower bound should pass");
    expect(comic2::is_hazard_tile(0xA7, config), "inside hazard band should pass");
    expect(comic2::is_hazard_tile(0xAF, config), "hazard band upper bound should pass");
    expect(!comic2::is_hazard_tile(0xB0, config), "above hazard band should fail");
}

}  // namespace

void run_tile_collision_tests() {
    test_get_tile_at_pixels_maps_to_expected_tile_id();
    test_get_tile_at_pixels_rejects_out_of_bounds();
    test_room_loader_builds_row_pointer_table();
    test_tile_threshold_and_hazard_checks();
}
