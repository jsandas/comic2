#pragma once

#include <cstdint>
#include <vector>

#include "comic2/entity_runtime.hpp"
#include "comic2/projectiles.hpp"
#include "comic2/types.hpp"

namespace comic2 {

struct InputState {
    bool jump_pressed = false;
    bool left_pressed = false;
    bool right_pressed = false;
    bool down_pressed = false;

    bool operator==(const InputState&) const = default;
};

struct PlayerState {
    std::int16_t x = 0;
    std::int16_t y = 0;
    std::int16_t x_vel = 0;
    std::int16_t y_vel = 0;

    bool is_airborne = false;
    bool is_physics_active = false;
    bool is_animation_active = false;
    bool is_attack_active = false;
    bool facing_right = true;

    std::uint8_t jump_counter = 0;
    std::uint8_t hp = 12;
    std::uint8_t firepower = 1;

    bool operator==(const PlayerState&) const = default;
};

struct DispatcherFlags {
    bool level_transition_pending = false;
    bool special_logic1_active = false;
    bool special_logic2_active = false;
    bool timed_overlay_pending = false;
    bool distance_interaction_active = false;
    bool player_special_state_active = false;
    bool tile_hazard_triggered = false;

    bool operator==(const DispatcherFlags&) const = default;
};

struct RuntimeState {
    std::uint16_t current_level = 0;
    std::uint16_t current_room = 0;

    RoomTileGrid room_grid;

    PlayerState player;
    InputState input;
    DispatcherFlags flags;

    std::vector<MappedObject12> mapped_objects;
    std::vector<ActiveEntity8> active_entities;
    std::vector<RuntimeEntitySlot32> runtime_slots;
    EntityActivationState activation_state;

    std::vector<ProjectileState> projectiles;

    bool operator==(const RuntimeState&) const = default;
};

}  // namespace comic2
