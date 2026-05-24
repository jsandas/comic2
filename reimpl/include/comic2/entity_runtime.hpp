#pragma once

#include <vector>

#include "comic2/types.hpp"

namespace comic2 {

bool is_runtime_slot_active(const RuntimeEntitySlot32& slot);
void deactivate_runtime_slot(RuntimeEntitySlot32& slot);
void prune_inactive_runtime_slots(std::vector<RuntimeEntitySlot32>& slots);

}  // namespace comic2
