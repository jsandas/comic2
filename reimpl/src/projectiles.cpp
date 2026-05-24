#include "comic2/projectiles.hpp"

namespace comic2 {

void spawn_projectile(std::vector<ProjectileState>& projectiles,
                      std::int16_t x,
                      std::int16_t y,
                      std::int16_t x_vel,
                      std::int16_t y_vel) {
    projectiles.push_back(ProjectileState{.x = x, .y = y, .x_vel = x_vel, .y_vel = y_vel, .active = true});
}

void update_projectiles(std::vector<ProjectileState>& projectiles, const ProjectileBounds& bounds) {
    for (auto& projectile : projectiles) {
        if (!projectile.active) {
            continue;
        }

        projectile.x = static_cast<std::int16_t>(projectile.x + projectile.x_vel);
        projectile.y = static_cast<std::int16_t>(projectile.y + projectile.y_vel);

        if (projectile.x < bounds.min_x || projectile.x > bounds.max_x ||
            projectile.y < bounds.min_y || projectile.y > bounds.max_y) {
            projectile.active = false;
        }
    }
}

}  // namespace comic2
