#include "comic2/entity_runtime.hpp"

#include <algorithm>

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

}  // namespace comic2
