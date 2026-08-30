#pragma once

#include <cstdint>

#include "comic2/game_state.hpp"
#include "comic2/tile_collision.hpp"

namespace comic2 {

inline constexpr std::uint8_t kModeSpeedBoost = 0x01U;
inline constexpr std::uint8_t kModeInvulnerability = 0x02U;
inline constexpr std::uint8_t kModeJumpBoost = 0x03U;

struct PlayerMotionConfig {
  std::int16_t walk_step = 8;
  std::int16_t jump_impulse = -5;
  std::int16_t air_drift_step = 1;
  std::int16_t gravity_per_tick = 1;
  std::int16_t max_fall_speed = 5;
  std::int16_t fall_start_velocity = 1;
};

PlayerMotionConfig
get_effective_motion_config(const RuntimeState &state,
                            const PlayerMotionConfig &motion);
void apply_input_tick(RuntimeState &state, const PlayerMotionConfig &motion);
void apply_grounded_physics_tick(RuntimeState &state,
                                 const PlayerMotionConfig &motion,
                                 const TileCollisionConfig &collision);
void apply_airborne_physics_tick(RuntimeState &state,
                                 const PlayerMotionConfig &motion,
                                 const TileCollisionConfig &collision);
void apply_physics_tick(RuntimeState &state, const PlayerMotionConfig &motion,
                        const TileCollisionConfig &collision);

} // namespace comic2
