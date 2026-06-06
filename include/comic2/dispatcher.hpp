#pragma once

#include <functional>
#include <utility>
#include <vector>

#include "comic2/game_state.hpp"

namespace comic2 {

enum class DispatchStage {
  LevelTransition,
  SpecialLogic1,
  SpecialLogic2,
  AirbornePhysics,
  TimedOverlay,
  GroundedPhysics,
  PlayerAnimation,
  AttackAnimation,
  DistanceInteraction,
  TileHazard,
  PlayerSpecialState,
  InputHandling,
};

struct DispatchResult {
  DispatchStage stage = DispatchStage::InputHandling;
  bool hook_executed = false;
};

using StageHook = std::function<void(RuntimeState &)>;

class GameDispatcher {
public:
  static DispatchStage choose_stage(const RuntimeState &state);
  DispatchResult run_tick(RuntimeState &state) const;

  void set_trace_enabled(bool enabled) { trace_enabled_ = enabled; }
  void clear_trace() { trace_log_.clear(); }
  const std::vector<DispatchStage> &trace_log() const { return trace_log_; }

  void set_level_transition_hook(StageHook hook) {
    level_transition_hook_ = std::move(hook);
  }
  void set_special_logic1_hook(StageHook hook) {
    special_logic1_hook_ = std::move(hook);
  }
  void set_special_logic2_hook(StageHook hook) {
    special_logic2_hook_ = std::move(hook);
  }
  void set_airborne_physics_hook(StageHook hook) {
    airborne_physics_hook_ = std::move(hook);
  }
  void set_timed_overlay_hook(StageHook hook) {
    timed_overlay_hook_ = std::move(hook);
  }
  void set_grounded_physics_hook(StageHook hook) {
    grounded_physics_hook_ = std::move(hook);
  }
  void set_player_animation_hook(StageHook hook) {
    player_animation_hook_ = std::move(hook);
  }
  void set_attack_animation_hook(StageHook hook) {
    attack_animation_hook_ = std::move(hook);
  }
  void set_distance_interaction_hook(StageHook hook) {
    distance_interaction_hook_ = std::move(hook);
  }
  void set_tile_hazard_hook(StageHook hook) {
    tile_hazard_hook_ = std::move(hook);
  }
  void set_player_special_state_hook(StageHook hook) {
    player_special_state_hook_ = std::move(hook);
  }
  void set_input_handling_hook(StageHook hook) {
    input_handling_hook_ = std::move(hook);
  }

private:
  static bool call_hook(const StageHook &hook, RuntimeState &state);

  void record_stage(DispatchStage stage) const;

  StageHook level_transition_hook_;
  StageHook special_logic1_hook_;
  StageHook special_logic2_hook_;
  StageHook airborne_physics_hook_;
  StageHook timed_overlay_hook_;
  StageHook grounded_physics_hook_;
  StageHook player_animation_hook_;
  StageHook attack_animation_hook_;
  StageHook distance_interaction_hook_;
  StageHook tile_hazard_hook_;
  StageHook player_special_state_hook_;
  StageHook input_handling_hook_;

  mutable std::vector<DispatchStage> trace_log_;
  bool trace_enabled_ = false;
};

const char *to_string(DispatchStage stage) noexcept;

} // namespace comic2
