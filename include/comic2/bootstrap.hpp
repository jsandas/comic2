#pragma once

#include <chrono>
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

struct FrameLoopSummary {
  int frames_rendered = 0;
  int ticks_processed = 0;
  DispatchStage last_stage = DispatchStage::InputHandling;
};

bool read_bootstrap_bool_env(const char *name);
int read_bootstrap_tick_budget(int default_ticks = 2);
int run_bootstrap_entry(const std::filesystem::path &root);
FrameLoopSummary run_render_loop(RuntimeState &state, GameDispatcher &dispatcher,
                                 IFramePresenter &presenter,
                                 int frame_budget = 60,
                                 std::chrono::milliseconds frame_interval =
                                     std::chrono::milliseconds(0));

void poll_bootstrap_input(RuntimeState &state);
void render_bootstrap_frame(IFramePresenter &presenter,
                            const RuntimeState &state);
BootstrapLoadSummary load_initial_bootstrap_resources(RuntimeState &state,
                                                      const std::filesystem::path &root);
BootstrapTickSummary run_bootstrap_tick(RuntimeState &state,
                                        GameDispatcher &dispatcher,
                                        IFramePresenter &presenter);

} // namespace comic2
