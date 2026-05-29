#include "comic2/entity_runtime.hpp"

#include <algorithm>
#include <cstring>

namespace comic2 {

bool is_runtime_slot_active(const RuntimeEntitySlot32& slot) {
    return slot.mapped_object_ptr != 0;
}

void deactivate_runtime_slot(RuntimeEntitySlot32& slot) {
    slot.mapped_object_ptr = 0;
    slot.behavior_state = 0;
    slot.type_flags = 0;
}

void prune_inactive_runtime_slots(std::vector<RuntimeEntitySlot32>& slots) {
    slots.erase(std::remove_if(slots.begin(),
                               slots.end(),
                               [](const RuntimeEntitySlot32& slot) {
                                   return !is_runtime_slot_active(slot);
                               }),
                slots.end());
}

void ent_build_room_entity_list(const std::vector<MappedObject12>& mapped_objects,
                                 std::uint16_t room_x,
                                 std::uint16_t room_y,
                                 std::vector<ActiveEntity8>& active_entities,
                                 EntityActivationState& activation_state) {
    active_entities.clear();
    activation_state = EntityActivationState{};

    for (const auto& obj : mapped_objects) {
        if (obj.room_x == room_x && obj.room_y == room_y) {
            ActiveEntity8 entity;
            entity.x = static_cast<std::int16_t>(obj.world_x);
            entity.y = static_cast<std::int16_t>(obj.world_y);
            entity.flags_or_slot = obj.state_flags;
            entity.sprite_or_obj = obj.descriptor_ptr;
            active_entities.push_back(entity);
        }
    }

    std::memset(activation_state.slot_data, 0xFF, sizeof(activation_state.slot_data));
    activation_state.first_active_slot = 0;
    activation_state.active_count = static_cast<std::uint8_t>(active_entities.size());
}

void ent_build_runtime_slots_for_viewport(const std::vector<ActiveEntity8>& active_entities,
                                           const std::vector<MappedObject12>& mapped_objects,
                                           std::vector<RuntimeEntitySlot32>& runtime_slots,
                                           const EntityViewportBounds& viewport_bounds,
                                           EntityActivationState& activation_state) {
    runtime_slots.clear();
    runtime_slots.resize(6, RuntimeEntitySlot32{});

    std::uint8_t slot_index = 0;
    std::uint16_t activation_toggle = 0;

    for (const auto& entity : active_entities) {
        if (slot_index >= 6) {
            break;
        }

        const std::int16_t entity_min_x = entity.x;
        const std::int16_t entity_max_x = entity.x + 16;
        const std::int16_t entity_min_y = entity.y;
        const std::int16_t entity_max_y = entity.y + 16;

        const bool intersects_viewport =
            (entity_max_x > viewport_bounds.min_x) &&
            (entity_min_x < viewport_bounds.max_x) &&
            (entity_max_y > viewport_bounds.min_y) &&
            (entity_min_y < viewport_bounds.max_y);

        if (intersects_viewport) {
            const std::uint16_t descriptor_idx = entity.sprite_or_obj;
            if (descriptor_idx < mapped_objects.size()) {
                ent_copy_descriptor_to_runtime_slot(mapped_objects[descriptor_idx],
                                                     runtime_slots[slot_index],
                                                     descriptor_idx,
                                                     activation_toggle);
                slot_index++;
            }
        }
    }

    for (std::size_t i = slot_index; i < 6; ++i) {
        runtime_slots[i].mapped_object_ptr = 0;
    }

    activation_state.first_active_slot = 0;
    activation_state.active_count = slot_index;
}

void ent_copy_descriptor_to_runtime_slot(const MappedObject12& descriptor,
                                          RuntimeEntitySlot32& slot,
                                          std::uint16_t behavior_ptr,
                                          std::uint16_t& activation_toggle) {
    slot.hitbox_w = descriptor.room_x;
    slot.hitbox_h = descriptor.room_y;
    slot.type_flags = descriptor.descriptor_ptr;
    slot.behavior_state = descriptor.state_flags;
    slot.anim_span = descriptor.world_x;
    slot.anim_period = descriptor.world_y;
    slot.unknown_0c = 0;
    slot.unknown_0e = 0;
    slot.x = 0;
    slot.y = 0;
    slot.param_a = 0;
    slot.param_b = 0;
    slot.dir_toggle = 0;
    slot.anim_tick = 0;
    slot.unknown_1c = 0;
    slot.mapped_object_ptr = behavior_ptr;

    slot.x = static_cast<std::int16_t>(descriptor.world_x);
    slot.y = static_cast<std::int16_t>(descriptor.world_y);

    slot.param_a = 0;
    slot.param_b = 0;
    slot.dir_toggle = 0;

    slot.behavior_state = activation_toggle & 3;
    activation_toggle ^= 3;
}

}  // namespace comic2
