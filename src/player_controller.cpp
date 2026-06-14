#include "comic2/player_controller.hpp"

#include <algorithm>

namespace comic2 {

namespace detail {

void apply_horizontal_movement(RuntimeState &state,
                               const PlayerMotionConfig &motion) {
  const bool move_left = state.input.left_pressed;
  const bool move_right = state.input.right_pressed;

  if (move_left && !move_right) {
    state.player.x_vel = static_cast<std::int16_t>(-motion.walk_step);
    state.player.x += state.player.x_vel;
  } else if (move_right && !move_left) {
    state.player.x_vel = motion.walk_step;
    state.player.x += state.player.x_vel;
  } else {
    state.player.x_vel = 0;
  }
}

bool trigger_fall_if_no_support(RuntimeState &state,
                                const TileCollisionConfig &collision,
                                const PlayerMotionConfig &motion) {
  if (has_floor_support(state, collision)) {
    return false;
  }

  state.player.is_airborne = true;
  state.player.is_physics_active = true;
  if (state.player.y_vel < motion.fall_start_velocity) {
    state.player.y_vel = motion.fall_start_velocity;
  }
  return true;
}

} // namespace detail

void apply_input_tick(RuntimeState &state, const PlayerMotionConfig &motion) {
  detail::apply_horizontal_movement(state, motion);

  if (state.input.jump_pressed && state.player.jump_counter > 0) {
    state.player.is_airborne = true;
    state.player.is_physics_active = true;
    state.player.y_vel = motion.jump_impulse;
    --state.player.jump_counter;
  }
}

void apply_grounded_physics_tick(RuntimeState &state,
                                 const PlayerMotionConfig &motion,
                                 const TileCollisionConfig &collision) {
  if (state.input.jump_pressed && state.player.jump_counter > 0) {
    state.player.is_airborne = true;
    state.player.is_physics_active = true;
    state.player.y_vel = motion.jump_impulse;
    --state.player.jump_counter;
    return;
  }

  if (detail::trigger_fall_if_no_support(state, collision, motion)) {
    return;
  }

  if (resolve_ground_contact(state, collision)) {
    return;
  }

  state.player.y = collision.ground_y;
  state.player.y_vel = 0;
  state.player.is_airborne = false;
  state.player.is_physics_active = false;
}

void apply_airborne_physics_tick(RuntimeState &state,
                                 const PlayerMotionConfig &motion,
                                 const TileCollisionConfig &collision) {
  if (state.input.jump_pressed && state.player.y_vel < 0 &&
      state.player.jump_counter > 0) {
    state.player.y_vel = motion.jump_impulse;
    --state.player.jump_counter;
  }

  const bool move_left = state.input.left_pressed;
  const bool move_right = state.input.right_pressed;

  if (move_left && !move_right) {
    state.player.x -= motion.air_drift_step;
    state.player.x_vel = static_cast<std::int16_t>(-motion.air_drift_step);
  } else if (move_right && !move_left) {
    state.player.x += motion.air_drift_step;
    state.player.x_vel = motion.air_drift_step;
  } else {
    state.player.x_vel = 0;
  }

  state.player.y += state.player.y_vel;
  state.player.y_vel = static_cast<std::int16_t>(std::min<std::int32_t>(
      state.player.y_vel + motion.gravity_per_tick, motion.max_fall_speed));

  if (resolve_ground_contact(state, collision)) {
    return;
  }

  state.player.is_airborne = true;
  state.player.is_physics_active = true;
}

void apply_physics_tick(RuntimeState &state, const PlayerMotionConfig &motion,
                        const TileCollisionConfig &collision) {
  if (state.player.is_airborne) {
    apply_airborne_physics_tick(state, motion, collision);
  } else {
    apply_grounded_physics_tick(state, motion, collision);
  }
}

} // namespace comic2
