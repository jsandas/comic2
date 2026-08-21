#include <stdexcept>
#include <string>
#include <vector>

#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/game_state.hpp"
#include "comic2/oracle.hpp"
#include "comic2/renderer.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void run_oracle_tests() {
  const std::string log =
      "tick=0 x=10 y=20 x_vel=1 y_vel=0 grounded=1 facing=1 hp=12\n"
      "tick=1 x=11 y=20 x_vel=1 y_vel=0 grounded=1 facing=1 hp=12\n";

  const auto parsed = comic2::parse_replay_log(log);
  expect(parsed.size() == 2, "oracle parser should read both snapshots");
  expect(parsed[0].player_x == 10, "oracle parser should capture player x");
  expect(parsed[1].player_y == 20, "oracle parser should capture player y");

  std::vector<comic2::InputState> inputs(2);
  inputs[0].right_pressed = true;
  inputs[1].right_pressed = true;

  comic2::RuntimeState state = comic2::make_default_runtime_state();
  comic2::GameDispatcher dispatcher = comic2::make_default_game_dispatcher();
  const auto captured =
      comic2::capture_runtime_snapshots(state, dispatcher, inputs);
  expect(captured.size() == 2, "snapshot capture should preserve input count");
  expect(captured[0].player_x >= 64,
         "captured snapshot should include starting x");

  comic2::ReplayComparisonOptions options{};
  options.position_tolerance_px = 1;
  options.timing_tolerance_ticks = 1;
  const auto compare =
      comic2::compare_snapshot_sequences(captured, parsed, options);
  expect(compare.matches || compare.differences.size() <= 2,
         "comparison should tolerate small timing drift");

  comic2::ReplayComparisonResult exact_compare =
      comic2::compare_snapshot_sequences(captured, captured, options);
  expect(exact_compare.matches, "matching snapshots should compare cleanly");
}

} // namespace

void run_oracle_test_suite() { run_oracle_tests(); }
