#include "comic2/tile_collision.hpp"

#include <cstddef>

namespace comic2 {

namespace {

constexpr std::int16_t kTileSizePixels = 16;

}  // namespace

bool get_tile_at_pixels(const RoomTileGrid& grid,
                        std::int16_t pixel_x,
                        std::int16_t pixel_y,
                        std::uint8_t* out_tile_id) {
    if (out_tile_id == nullptr || grid.tile_w == 0 || grid.tile_h == 0 ||
        grid.row_pointers.size() < grid.tile_h) {
        return false;
    }

    if (pixel_x < 0 || pixel_y < 0) {
        return false;
    }

    const std::int32_t tile_x = static_cast<std::int32_t>(pixel_x) / kTileSizePixels;
    const std::int32_t tile_y = static_cast<std::int32_t>(pixel_y) / kTileSizePixels;

    if (tile_x < 0 || tile_y < 0 || tile_x >= grid.tile_w || tile_y >= grid.tile_h) {
        return false;
    }

    const std::size_t row_base = grid.row_pointers[static_cast<std::size_t>(tile_y)];
    const std::size_t tile_off = row_base + static_cast<std::size_t>(tile_x);
    if (tile_off >= grid.tile_data.size()) {
        return false;
    }

    *out_tile_id = grid.tile_data[tile_off];
    return true;
}

bool tile_meets_threshold(std::uint8_t tile_id, std::uint8_t threshold) {
    return tile_id >= threshold;
}

bool is_hazard_tile(std::uint8_t tile_id, const TileCollisionConfig& config) {
    return tile_id >= config.hazard_tile_min && tile_id <= config.hazard_tile_max;
}

bool has_floor_support(const RuntimeState& state, const TileCollisionConfig& config) {
    if (config.use_room_grid_support) {
        std::uint8_t tile_id = 0;
        const std::int16_t probe_y = static_cast<std::int16_t>(state.player.y + 1);
        if (!get_tile_at_pixels(state.room_grid, state.player.x, probe_y, &tile_id)) {
            return false;
        }
        return tile_meets_threshold(tile_id, config.solid_tile_threshold);
    }

    const std::int16_t x = state.player.x;
    return x >= config.support_min_x && x <= config.support_max_x;
}

bool resolve_ground_contact(RuntimeState& state, const TileCollisionConfig& config) {
    if (state.player.y < config.ground_y) {
        return false;
    }

    state.player.y = config.ground_y;
    state.player.y_vel = 0;
    state.player.is_airborne = false;
    state.player.is_physics_active = false;
    return true;
}

}  // namespace comic2
