#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "comic2/dispatcher.hpp"
#include "comic2/game_state.hpp"

namespace comic2 {

struct ReplaySnapshot {
  std::uint32_t tick = 0;
  std::int32_t player_x = 0;
  std::int32_t player_y = 0;
  std::int32_t player_x_vel = 0;
  std::int32_t player_y_vel = 0;
  bool grounded = true;
  bool facing_right = true;
  std::uint32_t hp = 12;
  std::uint32_t score = 0;
  std::uint32_t gems = 0;
  std::uint32_t lives = 3;
};

struct ReplayComparisonOptions {
  std::int32_t position_tolerance_px = 1;
  std::int32_t timing_tolerance_ticks = 1;
};

struct ReplayComparisonResult {
  bool matches = false;
  std::size_t compared_count = 0;
  std::vector<std::string> differences;
};

std::vector<ReplaySnapshot> parse_replay_log(const std::string &log_text);
std::vector<ReplaySnapshot>
capture_runtime_snapshots(RuntimeState &state, const GameDispatcher &dispatcher,
                          const std::vector<InputState> &inputs);
ReplayComparisonResult
compare_snapshot_sequences(const std::vector<ReplaySnapshot> &actual,
                           const std::vector<ReplaySnapshot> &expected,
                           const ReplayComparisonOptions &options = {});

} // namespace comic2
