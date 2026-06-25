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
constexpr std::int16_t kTileSizePixels = 16;

void set_pixel(EgaPlanarSurface &surface, std::int32_t x, std::int32_t y,
               std::uint8_t color_index) {
  if (x < 0 || y < 0 || x >= surface.width_pixels() ||
      y >= surface.height_rows()) {
    return;
  }

  const auto x_byte = static_cast<std::size_t>(x / 8);
  const auto bit = static_cast<std::uint8_t>(7 - (x % 8));
  const auto y_row = static_cast<std::size_t>(y);
  const auto mask = static_cast<std::uint8_t>(1U << bit);

  for (std::size_t plane = 0; plane < EgaPlanarSurface::kPlaneCount; ++plane) {
    auto value = surface.get_plane_byte(plane, x_byte, y_row);
    if ((color_index >> plane) & 0x1U) {
      value = static_cast<std::uint8_t>(value | mask);
    } else {
      value =
          static_cast<std::uint8_t>(value & static_cast<std::uint8_t>(~mask));
    }
    surface.set_plane_byte(plane, x_byte, y_row, value);
  }
}

std::uint8_t read_room_tile(const RuntimeState &state, std::size_t tile_x,
                            std::size_t tile_y) {
  if (tile_x >= state.room_grid.tile_w || tile_y >= state.room_grid.tile_h ||
      tile_y >= state.room_grid.row_pointers.size()) {
    return 0;
  }

  const std::size_t row_base = state.room_grid.row_pointers[tile_y];
  const std::size_t off = row_base + tile_x;
  if (off >= state.room_grid.tile_data.size()) {
    return 0;
  }
  return state.room_grid.tile_data[off];
}

void draw_fallback_background(EgaPlanarSurface &frame,
                              const RuntimeState &state) {
  frame.clear(static_cast<std::uint8_t>(
      (state.current_level * 5u + state.current_room * 3u + state.player.hp) &
      0x0F));
  for (std::size_t y = 0; y < frame.height_rows(); ++y) {
    const auto x = static_cast<std::int32_t>((y % 20U) * 16U);
    set_pixel(frame, x, static_cast<std::int32_t>(y), 0x0F);
    set_pixel(frame, x + 1, static_cast<std::int32_t>(y), 0x07);
  }
}

void draw_room_tilemap(EgaPlanarSurface &frame, const RuntimeState &state) {
  const std::size_t visible_tiles_x =
      static_cast<std::size_t>(frame.width_pixels()) / kTileSizePixels;
  const std::size_t visible_tiles_y =
      static_cast<std::size_t>(frame.height_rows()) / kTileSizePixels;

  for (std::size_t tile_y = 0;
       tile_y < state.room_grid.tile_h && tile_y < visible_tiles_y; ++tile_y) {
    for (std::size_t tile_x = 0;
         tile_x < state.room_grid.tile_w && tile_x < visible_tiles_x;
         ++tile_x) {
      const std::uint8_t tile_id = read_room_tile(state, tile_x, tile_y);
      const std::uint8_t base_color = static_cast<std::uint8_t>(tile_id & 0x0F);
      const std::uint8_t accent_color =
          static_cast<std::uint8_t>((base_color + 2U) & 0x0F);
      const auto px0 = static_cast<std::int32_t>(tile_x * kTileSizePixels);
      const auto py0 = static_cast<std::int32_t>(tile_y * kTileSizePixels);

      for (std::int32_t py = 0; py < kTileSizePixels; ++py) {
        for (std::int32_t px = 0; px < kTileSizePixels; ++px) {
          const bool edge = (px == 0 || py == 0 || px == kTileSizePixels - 1 ||
                             py == kTileSizePixels - 1);
          set_pixel(frame, px0 + px, py0 + py,
                    edge ? accent_color : base_color);
        }
      }
    }
  }
}

void draw_player_marker(EgaPlanarSurface &frame, const RuntimeState &state) {
  const std::int32_t px0 = state.player.x;
  const std::int32_t py0 = state.player.y;
  const std::uint8_t body_color = state.player.is_airborne ? 0x0E : 0x0C;

  for (std::int32_t py = 0; py < 16; ++py) {
    for (std::int32_t px = 0; px < 8; ++px) {
      set_pixel(frame, px0 + px, py0 + py, body_color);
    }
  }
}

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
  const bool has_room_grid = state.room_grid.tile_w > 0 &&
                             state.room_grid.tile_h > 0 &&
                             !state.room_grid.row_pointers.empty() &&
                             !state.room_grid.tile_data.empty();

  if (has_room_grid) {
    draw_room_tilemap(frame, state);
  } else {
    draw_fallback_background(frame, state);
  }

  draw_player_marker(frame, state);

  presenter.present(frame);
}

FrameLoopSummary run_render_loop(RuntimeState &state,
                                 GameDispatcher &dispatcher,
                                 IFramePresenter &presenter, int frame_budget,
                                 std::chrono::milliseconds frame_interval) {
  FrameLoopSummary summary{};
  if (frame_budget <= 0) {
    return summary;
  }

  auto next_tick = std::chrono::steady_clock::now();

  for (int frame = 0; frame < frame_budget; ++frame) {
    if (frame_interval.count() > 0) {
      const auto now = std::chrono::steady_clock::now();
      if (now < next_tick) {
        std::this_thread::sleep_until(next_tick);
      }
    }

    poll_bootstrap_input(state);
    const auto result = dispatcher.run_tick(state);
    render_bootstrap_frame(presenter, state);

    summary.frames_rendered += 1;
    summary.ticks_processed += 1;
    summary.last_stage = result.stage;

    if (frame_interval.count() > 0) {
      next_tick += frame_interval;
      const auto now = std::chrono::steady_clock::now();
      if (next_tick < now) {
        next_tick = now;
      }
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
  const auto loop_summary = run_render_loop(
      state, dispatcher, presenter, tick_budget, std::chrono::milliseconds(0));

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
