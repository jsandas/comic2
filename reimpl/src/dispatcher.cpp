#include "comic2/dispatcher.hpp"

namespace comic2 {

DispatchStage GameDispatcher::choose_stage(const RuntimeState& state) const {
    const auto& flags = state.flags;
    const auto& player = state.player;

    if (flags.level_transition_pending) {
        return DispatchStage::LevelTransition;
    }
    if (flags.special_logic1_active) {
        return DispatchStage::SpecialLogic1;
    }
    if (flags.special_logic2_active) {
        return DispatchStage::SpecialLogic2;
    }
    if (player.is_airborne) {
        return DispatchStage::AirbornePhysics;
    }
    if (flags.timed_overlay_pending) {
        return DispatchStage::TimedOverlay;
    }
    if (player.is_physics_active) {
        return DispatchStage::GroundedPhysics;
    }
    if (player.is_animation_active) {
        return DispatchStage::PlayerAnimation;
    }
    if (player.is_attack_active) {
        return DispatchStage::AttackAnimation;
    }
    if (flags.distance_interaction_active) {
        return DispatchStage::DistanceInteraction;
    }
    if (flags.tile_hazard_triggered) {
        return DispatchStage::TileHazard;
    }
    if (flags.player_special_state_active) {
        return DispatchStage::PlayerSpecialState;
    }
    return DispatchStage::InputHandling;
}

DispatchResult GameDispatcher::run_tick(RuntimeState& state) const {
    const auto stage = choose_stage(state);
    bool mutated = false;

    switch (stage) {
        case DispatchStage::LevelTransition:
            mutated = call_hook(level_transition_hook_, state);
            break;
        case DispatchStage::SpecialLogic1:
            mutated = call_hook(special_logic1_hook_, state);
            break;
        case DispatchStage::SpecialLogic2:
            mutated = call_hook(special_logic2_hook_, state);
            break;
        case DispatchStage::AirbornePhysics:
            mutated = call_hook(airborne_physics_hook_, state);
            break;
        case DispatchStage::TimedOverlay:
            mutated = call_hook(timed_overlay_hook_, state);
            break;
        case DispatchStage::GroundedPhysics:
            mutated = call_hook(grounded_physics_hook_, state);
            break;
        case DispatchStage::PlayerAnimation:
            mutated = call_hook(player_animation_hook_, state);
            break;
        case DispatchStage::AttackAnimation:
            mutated = call_hook(attack_animation_hook_, state);
            break;
        case DispatchStage::DistanceInteraction:
            mutated = call_hook(distance_interaction_hook_, state);
            break;
        case DispatchStage::TileHazard:
            mutated = call_hook(tile_hazard_hook_, state);
            break;
        case DispatchStage::PlayerSpecialState:
            mutated = call_hook(player_special_state_hook_, state);
            break;
        case DispatchStage::InputHandling:
            mutated = call_hook(input_handling_hook_, state);
            break;
    }

    return DispatchResult{.stage = stage, .state_mutated = mutated};
}

bool GameDispatcher::call_hook(const StageHook& hook, RuntimeState& state) {
    if (!hook) {
        return false;
    }
    hook(state);
    return true;
}

const char* to_string(DispatchStage stage) noexcept {
    switch (stage) {
        case DispatchStage::LevelTransition:
            return "LevelTransition";
        case DispatchStage::SpecialLogic1:
            return "SpecialLogic1";
        case DispatchStage::SpecialLogic2:
            return "SpecialLogic2";
        case DispatchStage::AirbornePhysics:
            return "AirbornePhysics";
        case DispatchStage::TimedOverlay:
            return "TimedOverlay";
        case DispatchStage::GroundedPhysics:
            return "GroundedPhysics";
        case DispatchStage::PlayerAnimation:
            return "PlayerAnimation";
        case DispatchStage::AttackAnimation:
            return "AttackAnimation";
        case DispatchStage::DistanceInteraction:
            return "DistanceInteraction";
        case DispatchStage::TileHazard:
            return "TileHazard";
        case DispatchStage::PlayerSpecialState:
            return "PlayerSpecialState";
        case DispatchStage::InputHandling:
            return "InputHandling";
    }
    return "Unknown";
}

}  // namespace comic2
