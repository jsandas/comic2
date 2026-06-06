#include "comic2/projectiles.hpp"

#include "comic2/tile_collision.hpp"

namespace comic2 {

void spawn_projectile(std::vector<ProjectileState> &projectiles, std::int16_t x,
                      std::int16_t y, std::int16_t x_vel, std::int16_t y_vel) {
  projectiles.push_back(ProjectileState{
      .x = x, .y = y, .x_vel = x_vel, .y_vel = y_vel, .active = true});
}

void spawn_player_projectile(std::vector<ProjectileState> &projectiles,
                             const ProjectileSpawnParams &params) {
  std::int16_t x_vel = params.facing_right ? 0x0010 : -0x0010;
  if (!params.facing_right && params.is_airborne) {
    x_vel += 0x0010;
  }

  std::int16_t y = params.y + 4;
  if (!params.is_airborne) {
    y += 4;
  }
  std::int16_t y_vel = -0x000C;

  ProjectileState projectile;
  projectile.x = params.x + 4;
  projectile.y = y;
  projectile.x_vel = x_vel;
  projectile.y_vel = y_vel;
  projectile.anim_frame = 0;
  projectile.active = true;

  projectiles.push_back(projectile);
}

bool check_projectile_tile_collision(const ProjectileState &projectile,
                                     const RoomTileGrid &room_grid) {
  const std::int16_t probe_x = projectile.x + 3;
  const std::int16_t probe_y = projectile.y + 3;

  const auto tile_id = get_tile_at_pixels(room_grid, probe_x, probe_y);
  if (!tile_id.has_value()) {
    return false;
  }

  return tile_id.value() != 0;
}

void update_projectiles(std::vector<ProjectileState> &projectiles,
                        const ProjectileBounds &bounds,
                        const RoomTileGrid &room_grid,
                        std::int16_t viewport_min_x,
                        std::int16_t viewport_min_y) {
  const std::int16_t viewport_max_x = viewport_min_x + 200;
  const std::int16_t viewport_max_y = viewport_min_y + 152;

  for (auto &projectile : projectiles) {
    if (!projectile.active) {
      continue;
    }

    projectile.x = static_cast<std::int16_t>(projectile.x + projectile.x_vel);
    projectile.y = static_cast<std::int16_t>(projectile.y + projectile.y_vel);

    projectile.anim_frame =
        static_cast<std::int16_t>((projectile.anim_frame + 1) % 8);

    const bool outside_viewport = (projectile.x < viewport_min_x - 8) ||
                                  (projectile.x > viewport_max_x + 8) ||
                                  (projectile.y < viewport_min_y - 8) ||
                                  (projectile.y > viewport_max_y + 8);

    if (outside_viewport) {
      projectile.active = false;
      continue;
    }

    if (check_projectile_tile_collision(projectile, room_grid)) {
      projectile.active = false;
      continue;
    }

    if (projectile.x < bounds.min_x || projectile.x > bounds.max_x ||
        projectile.y < bounds.min_y || projectile.y > bounds.max_y) {
      projectile.active = false;
    }
  }
}

} // namespace comic2
