#include <stdexcept>
#include <vector>

#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/game_state.hpp"
#include "comic2/oracle.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void run_end_to_end_smoke_tests() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.current_level = 1;
  state.current_room = 0;
  state.player.x = 32;
  state.player.y = 96;
  state.player.hp = 12;
  state.player.gems = 1;
  state.player.lives = 3;

  comic2::GameDispatcher dispatcher = comic2::make_default_game_dispatcher();
  std::vector<comic2::InputState> inputs(6);
  inputs[0].right_pressed = true;
  inputs[1].right_pressed = true;
  inputs[2].jump_pressed = true;
  inputs[3].down_pressed = true;
  inputs[4].left_pressed = true;
  inputs[5].left_pressed = true;

  const auto snapshots =
      comic2::capture_runtime_snapshots(state, dispatcher, inputs);
  expect(snapshots.size() == 6, "smoke flow should capture each scripted tick");
  expect(snapshots.back().player_x != snapshots.front().player_x ||
             snapshots.back().player_y != snapshots.front().player_y,
         "smoke flow should advance player state");

  comic2::ReplayComparisonOptions options{};
  options.position_tolerance_px = 1;
  options.timing_tolerance_ticks = 1;
  const auto compare =
      comic2::compare_snapshot_sequences(snapshots, snapshots, options);
  expect(compare.matches, "self-comparison should match exactly");
}

} // namespace

void run_end_to_end_test_suite() { run_end_to_end_smoke_tests(); }
