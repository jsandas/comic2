#pragma once

#include <cstdint>
#include <vector>

#include "comic2/types.hpp"

namespace comic2 {

struct EntityViewportBounds {
    std::int16_t min_x = 0;
    std::int16_t max_x = 319;
    std::int16_t min_y = 0;
    std::int16_t max_y = 199;
};

struct EntityActivationState {
    std::uint8_t first_active_slot = 0;
    std::uint8_t active_count = 0;
    std::uint8_t slot_data[9] = {};
    bool operator==(const EntityActivationState&) const = default;
};

bool is_runtime_slot_active(const RuntimeEntitySlot32& slot);
void deactivate_runtime_slot(RuntimeEntitySlot32& slot);
void prune_inactive_runtime_slots(std::vector<RuntimeEntitySlot32>& slots);

void ent_build_room_entity_list(const std::vector<MappedObject12>& mapped_objects,
                                 std::uint16_t room_x,
                                 std::uint16_t room_y,
                                 std::vector<ActiveEntity8>& active_entities,
                                 EntityActivationState& activation_state);

void ent_build_runtime_slots_for_viewport(const std::vector<ActiveEntity8>& active_entities,
                                           const std::vector<MappedObject12>& mapped_objects,
                                           std::vector<RuntimeEntitySlot32>& runtime_slots,
                                           const EntityViewportBounds& viewport_bounds,
                                           EntityActivationState& activation_state,
                                           std::uint16_t& activation_toggle);

void ent_copy_descriptor_to_runtime_slot(const MappedObject12& descriptor,
                                          RuntimeEntitySlot32& slot,
                                          std::uint16_t behavior_ptr,
                                          std::uint16_t& activation_toggle);

void ent_deactivate_runtime_slot(RuntimeEntitySlot32& slot);

}  // namespace comic2
