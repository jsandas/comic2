#include "comic2/oracle.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace comic2 {
namespace {

std::string trim(const std::string &value) {
  std::size_t begin = 0;
  while (begin < value.size() &&
         std::isspace(static_cast<unsigned char>(value[begin]))) {
    ++begin;
  }
  std::size_t end = value.size();
  while (end > begin &&
         std::isspace(static_cast<unsigned char>(value[end - 1]))) {
    --end;
  }
  return value.substr(begin, end - begin);
}

std::int32_t parse_int_token(const std::string &text) {
  return std::stoi(text);
}

} // namespace

std::vector<ReplaySnapshot> parse_replay_log(const std::string &log_text) {
  std::vector<ReplaySnapshot> snapshots;
  std::stringstream stream(log_text);
  std::string line;
  while (std::getline(stream, line)) {
    const std::string trimmed = trim(line);
    if (trimmed.empty()) {
      continue;
    }

    ReplaySnapshot snapshot{};
    std::stringstream tokens(trimmed);
    std::string token;
    while (std::getline(tokens, token, ' ')) {
      const std::string clean = trim(token);
      if (clean.empty()) {
        continue;
      }
      const std::size_t pos = clean.find('=');
      if (pos == std::string::npos) {
        continue;
      }
      const std::string key = trim(clean.substr(0, pos));
      const std::string value = trim(clean.substr(pos + 1));
      if (key == "tick") {
        snapshot.tick = static_cast<std::uint32_t>(parse_int_token(value));
      } else if (key == "x") {
        snapshot.player_x = parse_int_token(value);
      } else if (key == "y") {
        snapshot.player_y = parse_int_token(value);
      } else if (key == "x_vel") {
        snapshot.player_x_vel = parse_int_token(value);
      } else if (key == "y_vel") {
        snapshot.player_y_vel = parse_int_token(value);
      } else if (key == "grounded") {
        snapshot.grounded = parse_int_token(value) != 0;
      } else if (key == "facing") {
        snapshot.facing_right = parse_int_token(value) != 0;
      } else if (key == "hp") {
        snapshot.hp = static_cast<std::uint32_t>(parse_int_token(value));
      } else if (key == "score") {
        snapshot.score = static_cast<std::uint32_t>(parse_int_token(value));
      } else if (key == "gems") {
        snapshot.gems = static_cast<std::uint32_t>(parse_int_token(value));
      } else if (key == "lives") {
        snapshot.lives = static_cast<std::uint32_t>(parse_int_token(value));
      }
    }
    snapshots.push_back(snapshot);
  }

  return snapshots;
}

std::vector<ReplaySnapshot>
capture_runtime_snapshots(RuntimeState &state, const GameDispatcher &dispatcher,
                          const std::vector<InputState> &inputs) {
  std::vector<ReplaySnapshot> snapshots;
  snapshots.reserve(inputs.size());

  for (const auto &input : inputs) {
    state.input = input;
    const auto result = dispatcher.run_tick(state);
    (void)result;
    ReplaySnapshot snapshot{};
    snapshot.tick = static_cast<std::uint32_t>(snapshots.size());
    snapshot.player_x = state.player.x;
    snapshot.player_y = state.player.y;
    snapshot.player_x_vel = state.player.x_vel;
    snapshot.player_y_vel = state.player.y_vel;
    snapshot.grounded = !state.player.is_airborne;
    snapshot.facing_right = state.player.facing_right;
    snapshot.hp = state.player.hp;
    snapshot.score = state.player.score;
    snapshot.gems = state.player.gems;
    snapshot.lives = state.player.lives;
    snapshots.push_back(snapshot);
  }

  return snapshots;
}

ReplayComparisonResult
compare_snapshot_sequences(const std::vector<ReplaySnapshot> &actual,
                           const std::vector<ReplaySnapshot> &expected,
                           const ReplayComparisonOptions &options) {
  ReplayComparisonResult result{};
  result.compared_count = std::min(actual.size(), expected.size());

  const auto compare_and_record = [&](std::size_t index) {
    const auto &a = actual[index];
    const auto &e = expected[index];

    const bool position_matches =
        std::abs(a.player_x - e.player_x) <= options.position_tolerance_px &&
        std::abs(a.player_y - e.player_y) <= options.position_tolerance_px;
    const bool velocity_matches = std::abs(a.player_x_vel - e.player_x_vel) <=
                                      options.position_tolerance_px &&
                                  std::abs(a.player_y_vel - e.player_y_vel) <=
                                      options.position_tolerance_px;
    const bool flags_match = a.grounded == e.grounded &&
                             a.facing_right == e.facing_right && a.hp == e.hp &&
                             a.score == e.score && a.gems == e.gems &&
                             a.lives == e.lives;
    const bool timing_matches = std::abs(static_cast<std::int32_t>(a.tick) -
                                         static_cast<std::int32_t>(e.tick)) <=
                                options.timing_tolerance_ticks;

    if (position_matches && velocity_matches && flags_match && timing_matches) {
      return;
    }

    std::ostringstream oss;
    oss << "tick=" << index << " pos(" << a.player_x << "," << a.player_y
        << ") vs (" << e.player_x << "," << e.player_y << ") vel("
        << a.player_x_vel << "," << a.player_y_vel << ") vs (" << e.player_x_vel
        << "," << e.player_y_vel << ") flags(" << a.grounded << ","
        << a.facing_right << "," << a.hp << ") vs (" << e.grounded << ","
        << e.facing_right << "," << e.hp << ")";
    result.differences.push_back(oss.str());
  };

  const std::size_t common = std::min(actual.size(), expected.size());
  for (std::size_t i = 0; i < common; ++i) {
    compare_and_record(i);
  }

  if (actual.size() != expected.size()) {
    result.differences.push_back("sequence length mismatch");
  }

  result.matches = result.differences.empty();
  return result;
}

} // namespace comic2
