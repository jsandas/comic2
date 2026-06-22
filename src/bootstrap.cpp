#include "comic2/bootstrap.hpp"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <thread>

#include "comic2/default_handlers.hpp"
#include "comic2/resource_loader.hpp"

namespace comic2 {

namespace {

constexpr int kDefaultBootstrapTicks = 2;

} // namespace

bool read_bootstrap_bool_env(const char *name) {
  const char *value = std::getenv(name);
  return value != nullptr && (*value == '1' || *value == 't' || *value == 'T' ||
                              *value == 'y' || *value == 'Y');
}

int read_bootstrap_tick_budget(int default_ticks) {
  const char *value = std::getenv("COMIC2_BOOTSTRAP_TICKS");
  if (value == nullptr || *value == '\0') {
    return default_ticks > 0 ? default_ticks : kDefaultBootstrapTicks;
  }

  try {
    const int parsed = std::stoi(value);
    return parsed > 0
               ? parsed
               : (default_ticks > 0 ? default_ticks : kDefaultBootstrapTicks);
  } catch (const std::exception &) {
    return default_ticks > 0 ? default_ticks : kDefaultBootstrapTicks;
  }
}

void poll_bootstrap_input(RuntimeState &state) {
  state.input.jump_pressed = read_bootstrap_bool_env("COMIC2_INPUT_JUMP");
  state.input.left_pressed = read_bootstrap_bool_env("COMIC2_INPUT_LEFT");
  state.input.right_pressed = read_bootstrap_bool_env("COMIC2_INPUT_RIGHT");
  state.input.down_pressed = read_bootstrap_bool_env("COMIC2_INPUT_DOWN");
}

void render_bootstrap_frame(IFramePresenter &presenter,
                            const RuntimeState &state) {
  EgaPlanarSurface frame(320, 200);
  // Synthetic bootstrap pattern for the startup smoke test.
  frame.clear(static_cast<std::uint8_t>((state.current_level * 17u +
                                         state.current_room * 13u +
                                         state.player.hp * 5u) &
                                        0x0F));

  for (std::size_t y = 0; y < frame.height_rows(); ++y) {
    const std::size_t x = (y % 16U) * 2U;
    frame.set_plane_byte(0, x / 8U, y,
                         static_cast<std::uint8_t>(0x80U >> (x % 8U)));
    frame.set_plane_byte(1, (x + 4U) / 8U, y,
                         static_cast<std::uint8_t>(0x80U >> ((x + 4U) % 8U)));
  }

  presenter.present(frame);
}

FrameLoopSummary run_render_loop(RuntimeState &state, GameDispatcher &dispatcher,
                                 IFramePresenter &presenter,
                                 int frame_budget,
                                 std::chrono::milliseconds frame_interval) {
  FrameLoopSummary summary{};
  if (frame_budget <= 0) {
    return summary;
  }

  for (int frame = 0; frame < frame_budget; ++frame) {
    poll_bootstrap_input(state);
    const auto result = dispatcher.run_tick(state);
    render_bootstrap_frame(presenter, state);

    summary.frames_rendered += 1;
    summary.last_stage = result.stage;

    if (frame_interval.count() > 0) {
      std::this_thread::sleep_for(frame_interval);
    }
  }

  return summary;
}

int run_bootstrap_entry(const std::filesystem::path &root) {
  std::cout << "Starting comic2 bootstrap from: " << root.string() << "\n";

  auto state = make_default_runtime_state();
  const auto bootstrap = load_initial_bootstrap_resources(state, root);

  if (!bootstrap.room_grid_loaded) {
    std::cerr << "WARNING: no bootstrap room grid loaded from " << root.string()
              << " (room loader remains provisional)\n";
  }

  std::cout << "Bootstrap resources: metadata_files="
            << bootstrap.metadata_files_tried
            << " sprite_files=" << bootstrap.sprite_files_tried
            << " room_grid_loaded=" << std::boolalpha
            << bootstrap.room_grid_loaded << std::noboolalpha << "\n";
  std::cout << "Loaded metadata bytes=" << state.level_metadata_bytes.size()
            << " room_bytes=" << state.room_resource_bytes.size()
            << " sprite_bytes=" << state.sprite_resource_bytes.size() << "\n";

  auto dispatcher = make_default_game_dispatcher();
  dispatcher.set_trace_enabled(true);

  MemoryFramePresenter presenter;
  const int tick_budget = read_bootstrap_tick_budget();
  const auto loop_summary = run_render_loop(state, dispatcher, presenter,
                                             tick_budget,
                                             std::chrono::milliseconds(0));

  std::cout << "Bootstrap loop frames=" << loop_summary.frames_rendered
            << " last_stage=" << to_string(loop_summary.last_stage)
            << " input_left=" << state.input.left_pressed
            << " input_right=" << state.input.right_pressed
            << " input_jump=" << state.input.jump_pressed
            << " input_down=" << state.input.down_pressed << "\n";

  std::cout << "Bootstrap ready: level=" << state.current_level
            << " room=" << state.current_room
            << " frame_captured=" << std::boolalpha << presenter.has_frame()
            << std::noboolalpha << "\n";

  return 0;
}

BootstrapTickSummary run_bootstrap_tick(RuntimeState &state,
                                        GameDispatcher &dispatcher,
                                        IFramePresenter &presenter) {

  poll_bootstrap_input(state);
  const auto result = dispatcher.run_tick(state);
  render_bootstrap_frame(presenter, state);

  return BootstrapTickSummary{
      .stage = result.stage, .input_captured = true, .frame_presented = true};
}

} // namespace comic2
