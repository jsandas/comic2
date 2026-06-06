#include "comic2/tile_collision.hpp"

#include <cstddef>
#include <optional>

namespace comic2 {

namespace {

constexpr std::int16_t kTileSizePixels = 16;

} // namespace

std::optional<std::uint8_t> get_tile_at_pixels(const RoomTileGrid &grid,
                                               std::int16_t pixel_x,
                                               std::int16_t pixel_y) {
  if (grid.tile_w == 0 || grid.tile_h == 0 ||
      grid.row_pointers.size() < grid.tile_h) {
    return std::nullopt;
  }

  if (pixel_x < 0 || pixel_y < 0) {
    return std::nullopt;
  }

  const std::int32_t tile_x =
      static_cast<std::int32_t>(pixel_x) / kTileSizePixels;
  const std::int32_t tile_y =
      static_cast<std::int32_t>(pixel_y) / kTileSizePixels;

  if (tile_x < 0 || tile_y < 0 || tile_x >= grid.tile_w ||
      tile_y >= grid.tile_h) {
    return std::nullopt;
  }

  const std::size_t row_base =
      grid.row_pointers[static_cast<std::size_t>(tile_y)];
  const std::size_t tile_off = row_base + static_cast<std::size_t>(tile_x);
  if (tile_off >= grid.tile_data.size()) {
    return std::nullopt;
  }

  return grid.tile_data[tile_off];
}

bool tile_meets_threshold(std::uint8_t tile_id, std::uint8_t threshold) {
  return tile_id >= threshold;
}

bool is_hazard_tile(std::uint8_t tile_id, const TileCollisionConfig &config) {
  return tile_id >= config.hazard_tile_min && tile_id <= config.hazard_tile_max;
}

bool update_player_hazard_state(RuntimeState &state,
                                const TileCollisionConfig &config) {
  const std::optional<std::uint8_t> tile_id =
      get_tile_at_pixels(state.room_grid, state.player.x, state.player.y);
  if (!tile_id.has_value()) {
    state.flags.tile_hazard_triggered = false;
    return false;
  }
  state.flags.tile_hazard_triggered = is_hazard_tile(*tile_id, config);
  return state.flags.tile_hazard_triggered;
}

bool has_floor_support(const RuntimeState &state,
                       const TileCollisionConfig &config) {
  const std::int16_t probe_y = static_cast<std::int16_t>(state.player.y + 1);
  const std::optional<std::uint8_t> tile_id =
      get_tile_at_pixels(state.room_grid, state.player.x, probe_y);
  if (tile_id.has_value()) {
    return tile_meets_threshold(*tile_id, config.solid_tile_threshold);
  }
  return false;
}

bool resolve_ground_contact(RuntimeState &state,
                            const TileCollisionConfig &config) {
  if (state.player.y < config.ground_y) {
    return false;
  }

  state.player.y = config.ground_y;
  state.player.y_vel = 0;
  state.player.is_airborne = false;
  state.player.is_physics_active = false;
  return true;
}

} // namespace comic2
