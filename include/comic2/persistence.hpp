#pragma once

#include <cstdint>
#include <vector>

#include "comic2/game_state.hpp"

namespace comic2 {

void cfg_load_options_or_defaults(RuntimeState &state,
                                  const std::vector<std::uint8_t> &bytes);
void cfg_save_options(const RuntimeState &state,
                      std::vector<std::uint8_t> &bytes);
void savegame_write_snapshot(const RuntimeState &state,
                             std::vector<std::uint8_t> &bytes);
void savegame_read_snapshot(RuntimeState &state,
                            const std::vector<std::uint8_t> &bytes);

} // namespace comic2
