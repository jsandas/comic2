#pragma once

#include <cstdint>
#include <vector>

namespace comic2 {

struct ProjectileState {
    std::int16_t x = 0;
    std::int16_t y = 0;
    std::int16_t x_vel = 0;
    std::int16_t y_vel = 0;
    bool active = true;

    bool operator==(const ProjectileState&) const = default;
};

struct ProjectileBounds {
    std::int16_t min_x = 0;
    std::int16_t max_x = 319;
    std::int16_t min_y = 0;
    std::int16_t max_y = 199;
};

void spawn_projectile(std::vector<ProjectileState>& projectiles,
                      std::int16_t x,
                      std::int16_t y,
                      std::int16_t x_vel,
                      std::int16_t y_vel);
void update_projectiles(std::vector<ProjectileState>& projectiles, const ProjectileBounds& bounds);

}  // namespace comic2
