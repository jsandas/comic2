#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>

#include "comic2/dispatcher.hpp"
#include "comic2/game_state.hpp"
#include "comic2/renderer.hpp"

namespace comic2 {

struct BootstrapLoadSummary {
  std::size_t metadata_files_tried = 0;
  std::size_t sprite_files_tried = 0;
  bool room_grid_loaded = false;
};

struct BootstrapTickSummary {
  DispatchStage stage = DispatchStage::InputHandling;
  bool input_captured = false;
  bool frame_presented = false;
};

bool read_bootstrap_bool_env(const char *name);
int read_bootstrap_tick_budget(int default_ticks = 2);
int run_bootstrap_entry(const std::filesystem::path &root);

void poll_bootstrap_input(RuntimeState &state);
void render_bootstrap_frame(IFramePresenter &presenter,
                            const RuntimeState &state);
BootstrapTickSummary run_bootstrap_tick(RuntimeState &state,
                                        GameDispatcher &dispatcher,
                                        IFramePresenter &presenter);

} // namespace comic2
