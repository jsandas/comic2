#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/game_state.hpp"
#include "comic2/renderer.hpp"
#include "comic2/resource_loader.hpp"

namespace {

void set_test_env(const char *name, const char *value, int overwrite) {
#ifdef _WIN32
  // Windows doesn't have setenv; use _putenv_s instead.
  (void)overwrite;
  _putenv_s(name, value);
#else
  if (value == nullptr || *value == '\0') {
    unsetenv(name);
  } else {
    setenv(name, value, overwrite);
  }
#endif
}

void check(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

bool require_original_assets() {
  const char *value = std::getenv("COMIC2_REQUIRE_ORIGINAL_ASSETS");
  if (value == nullptr || *value == '\0') {
    return false;
  }

  std::string normalized(value);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  return normalized == "1" || normalized == "true" || normalized == "yes" ||
         normalized == "on";
}

std::optional<std::filesystem::path> find_reference_assets_root() {
  std::vector<std::filesystem::path> seeds;

  const std::filesystem::path file_path(__FILE__);
  if (file_path.is_absolute()) {
    seeds.push_back(file_path.parent_path().parent_path());
  }

  seeds.push_back(std::filesystem::current_path());

  if (const char *github_workspace = std::getenv("GITHUB_WORKSPACE");
      github_workspace != nullptr && *github_workspace != '\0') {
    seeds.emplace_back(github_workspace);
  }

  for (const auto &seed : seeds) {
    std::filesystem::path cursor = seed;
    for (;;) {
      const auto candidate = cursor / "reference" / "original";
      if (std::filesystem::exists(candidate) &&
          std::filesystem::is_directory(candidate)) {
        for (const auto &entry :
             std::filesystem::directory_iterator(candidate)) {
          const auto name = entry.path().filename().string();
          if (name.rfind("FRDATA.", 0) == 0 && entry.is_regular_file()) {
            return candidate;
          }
        }
      }

      const auto parent = cursor.parent_path();
      if (parent == cursor || parent.empty()) {
        break;
      }
      cursor = parent;
    }
  }

  return std::nullopt;
}

struct RecordingPresenter : comic2::IFramePresenter {
  int last_width = 0;
  int last_height = 0;
  int present_calls = 0;

  void present(const comic2::EgaPlanarSurface &frame) override {
    ++present_calls;
    last_width = frame.width_pixels();
    last_height = frame.height_rows();
  }
};

std::uint8_t read_color_index(const comic2::EgaPlanarSurface &frame,
                              std::size_t x, std::size_t y) {
  const std::size_t x_byte = x / 8;
  const std::uint8_t bit = static_cast<std::uint8_t>(7 - (x % 8));
  std::uint8_t index = 0;
  for (std::size_t plane = 0; plane < comic2::EgaPlanarSurface::kPlaneCount;
       ++plane) {
    const auto value = frame.get_plane_byte(plane, x_byte, y);
    if (value & static_cast<std::uint8_t>(1U << bit)) {
      index = static_cast<std::uint8_t>(index | (1U << plane));
    }
  }
  return index;
}

void test_bootstrap_entry_runs_without_crashing() {
  const auto empty_root =
      std::filesystem::temp_directory_path() / "comic2_bootstrap_empty";
  std::filesystem::remove_all(empty_root);
  std::filesystem::create_directories(empty_root);

  set_test_env("COMIC2_BOOTSTRAP_TICKS", "1", 1);
  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_QUIT", "0", 1);
  set_test_env("COMIC2_INPUT_LEFT", "0", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  const int exit_code = comic2::run_bootstrap_entry(empty_root);

  check(exit_code == 0, "bootstrap entry should continue even when bootstrap "
                        "resources are missing");

  std::filesystem::remove_all(empty_root);
}

void test_bootstrap_tick_wires_input_dispatch_and_render() {
  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_QUIT", "0", 1);
  set_test_env("COMIC2_INPUT_LEFT", "1", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "1", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  auto state = comic2::make_default_runtime_state();
  state.player.is_airborne = false;
  state.player.is_physics_active = true;

  auto dispatcher = comic2::make_default_game_dispatcher();
  RecordingPresenter presenter;

  const auto summary = comic2::run_bootstrap_tick(state, dispatcher, presenter);

  check(summary.input_captured, "bootstrap tick should capture input flags");
  check(summary.stage == comic2::DispatchStage::GroundedPhysics,
        "bootstrap tick should enter grounded physics stage when grounded");
  check(summary.frame_presented, "bootstrap tick should render a frame");
  check(presenter.present_calls == 1,
        "bootstrap tick should invoke the presenter once");
  check(state.input.left_pressed, "left input should be captured");
  check(state.input.right_pressed, "right input should be captured");
  check(!state.input.jump_pressed,
        "jump input should remain false when env says 0");
}

void test_integrated_loop_exits_cleanly_when_quit_is_requested() {
  const auto empty_root =
      std::filesystem::temp_directory_path() / "comic2_integrated_quit";
  std::filesystem::remove_all(empty_root);
  std::filesystem::create_directories(empty_root);

  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_QUIT", "1", 1);
  set_test_env("COMIC2_INPUT_LEFT", "0", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  RecordingPresenter presenter;
  const auto summary = comic2::run_integrated_bootstrap_loop(
      empty_root, presenter, 5, std::chrono::milliseconds(0));

  check(
      !summary.bootstrap.room_grid_loaded,
      "integrated loop should stay in placeholder mode when assets are absent");
  check(summary.loop.quit_requested,
        "integrated loop should report a quit request");
  check(summary.loop.frames_rendered == 0,
        "integrated loop should stop before rendering frames when quit is "
        "requested");

  std::filesystem::remove_all(empty_root);
}

void test_dispatcher_moves_player_when_left_input_is_active() {
  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_QUIT", "0", 1);
  set_test_env("COMIC2_INPUT_LEFT", "1", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  auto state = comic2::make_default_runtime_state();
  state.player.is_airborne = false;
  state.player.is_physics_active = true;

  auto dispatcher = comic2::make_default_game_dispatcher();
  comic2::poll_bootstrap_input(state);
  const auto result = dispatcher.run_tick(state);

  check(result.stage == comic2::DispatchStage::GroundedPhysics,
        "dispatcher should run the grounded physics stage for the player");
  check(state.player.x < 64,
        "left input should move the player left through the dispatcher path");
  check(state.player.x_vel < 0,
        "left input should set a negative horizontal velocity");
}

void test_render_loop_renders_multiple_frames() {
  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_QUIT", "0", 1);
  set_test_env("COMIC2_INPUT_LEFT", "0", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  auto state = comic2::make_default_runtime_state();
  auto dispatcher = comic2::make_default_game_dispatcher();
  RecordingPresenter presenter;

  const auto summary = comic2::run_render_loop(state, dispatcher, presenter, 3,
                                               std::chrono::milliseconds(0));

  check(summary.frames_rendered == 3,
        "render loop should render the requested frame count");
  check(summary.ticks_processed == 3,
        "render loop should process one dispatcher tick per frame");
  check(presenter.present_calls == 3,
        "render loop should invoke the presenter for each frame");
}

void test_render_loop_updates_state_while_presenting_frames() {
  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_QUIT", "0", 1);
  set_test_env("COMIC2_INPUT_LEFT", "0", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "1", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  auto state = comic2::make_default_runtime_state();
  state.player.is_airborne = false;
  state.player.is_physics_active = true;
  const std::int16_t initial_x = state.player.x;

  auto dispatcher = comic2::make_default_game_dispatcher();
  RecordingPresenter presenter;

  const auto summary = comic2::run_render_loop(state, dispatcher, presenter, 4,
                                               std::chrono::milliseconds(0));

  check(summary.frames_rendered == 4,
        "render loop should render continuously until frame budget is reached");
  check(summary.ticks_processed == 4,
        "render loop should process one game tick per presented frame");
  check(presenter.present_calls == 4,
        "render loop should present each frame while running continuously");
  check(state.player.x > initial_x,
        "runtime state should update while frames are being presented");
}

void test_render_bootstrap_frame_uses_room_tile_data() {
  auto state = comic2::make_default_runtime_state();
  state.player.x = 300;
  state.player.y = 180;
  state.room_grid.tile_w = 1;
  state.room_grid.tile_h = 1;
  state.room_grid.row_pointers = {0};
  state.room_grid.tile_data = {0x0A};

  comic2::MemoryFramePresenter presenter;
  comic2::render_bootstrap_frame(presenter, state);

  check(presenter.has_frame(), "render should present a frame");
  const auto &frame_a = presenter.last_frame();
  const auto color_a = read_color_index(frame_a, 8, 8);

  state.room_grid.tile_data = {0x02};
  comic2::render_bootstrap_frame(presenter, state);
  const auto &frame_b = presenter.last_frame();
  const auto color_b = read_color_index(frame_b, 8, 8);

  check(color_a != color_b,
        "rendered tile color should change when room tile data changes");

  const auto top_left_edge = read_color_index(frame_b, 0, 0);
  const auto center_fill = read_color_index(frame_b, 8, 8);
  const auto bottom_right_edge = read_color_index(frame_b, 15, 15);

  check(top_left_edge == 0x04,
        "tile border should use the accent color derived from tile data");
  check(center_fill == 0x02,
        "tile interior should use the base color derived from tile data");
  check(bottom_right_edge == 0x04,
        "tile border should remain accented at the far edge");
}

void test_bootstrap_loader_reads_reference_room_data() {
  const std::optional<std::filesystem::path> reference_root =
      find_reference_assets_root();
  if (!reference_root.has_value()) {
    if (require_original_assets()) {
      throw std::runtime_error(
          "COMIC2_REQUIRE_ORIGINAL_ASSETS is set, but reference/original "
          "assets containing FRDATA.* were not found");
    }
    std::cerr << "Skipping FRDATA bootstrap load test: reference/original "
                 "assets are not present.\n";
    return;
  }

  auto state = comic2::make_default_runtime_state();

  const auto summary =
      comic2::load_initial_bootstrap_resources(state, reference_root.value());

  check(summary.room_grid_loaded, "bootstrap loader should load the reference "
                                  "room grid from FRDATA assets");
  check(state.room_grid.tile_w > 0 && state.room_grid.tile_h > 0,
        "bootstrap loader should populate the room grid dimensions");
}

void test_scene_bootstrap_discovers_reference_assets_from_repo_root() {
  const std::optional<std::filesystem::path> reference_root =
      find_reference_assets_root();
  if (!reference_root.has_value()) {
    if (require_original_assets()) {
      throw std::runtime_error(
          "COMIC2_REQUIRE_ORIGINAL_ASSETS is set, but reference/original "
          "assets containing FRDATA.* were not found");
    }
    std::cerr << "Skipping scene bootstrap discovery test: reference/original "
                 "assets are not present.\n";
    return;
  }

  const std::filesystem::path repo_root =
      reference_root->parent_path().parent_path();
  auto state = comic2::make_default_runtime_state();
  state.player.x = 10000;
  state.player.y = 10000;

  const auto summary = comic2::initialize_runtime_scene(state, repo_root);

  check(summary.room_grid_loaded,
        "scene bootstrap should load room data from discovered asset roots");
  check(!summary.using_placeholder,
        "scene bootstrap should not use placeholder mode when assets exist");
  check(!summary.assets_root_used.empty(),
        "scene bootstrap should report the selected assets root");
  check(state.room_grid.tile_w > 0 && state.room_grid.tile_h > 0,
        "scene bootstrap should populate room grid dimensions");
  check(state.player.x >= 0 && state.player.x <= 319,
        "scene bootstrap should clamp player x into visible bounds");
  check(state.player.y >= 0 && state.player.y <= 199,
        "scene bootstrap should clamp player y into visible bounds");
}

void test_scene_bootstrap_does_not_commit_failed_candidate_state() {
  const auto scratch_root =
      std::filesystem::temp_directory_path() / "comic2_scene_bootstrap_scratch";
  std::filesystem::remove_all(scratch_root);
  std::filesystem::create_directories(scratch_root);

  const std::vector<std::uint8_t> failing_sprite_bytes = {0xDE, 0xAD, 0xBE,
                                                          0xEF};
  {
    std::ofstream output(scratch_root / "FRPAK.001", std::ios::binary);
    output.write(reinterpret_cast<const char *>(failing_sprite_bytes.data()),
                 static_cast<std::streamsize>(failing_sprite_bytes.size()));
  }

  auto state = comic2::make_default_runtime_state();
  const auto summary = comic2::initialize_runtime_scene(state, scratch_root);

  check(!summary.room_grid_loaded,
        "scene bootstrap should remain in placeholder mode when no candidate "
        "succeeds");
  check(summary.using_placeholder,
        "scene bootstrap should explicitly mark placeholder fallback mode");
  check(
      state.sprite_resource_bytes.empty(),
      "failed candidate sprite bytes should not be committed to runtime state");
  check(state.level_metadata_bytes.empty(),
        "failed candidate metadata bytes should not be committed to runtime "
        "state");
  check(state.room_resource_bytes.empty(),
        "failed candidate room bytes should not be committed to runtime state");

  std::filesystem::remove_all(scratch_root);
}

void test_scene_bootstrap_falls_back_with_missing_assets() {
  const auto empty_root =
      std::filesystem::temp_directory_path() / "comic2_scene_bootstrap_empty";
  std::filesystem::remove_all(empty_root);
  std::filesystem::create_directories(empty_root);

  auto state = comic2::make_default_runtime_state();
  const auto summary = comic2::initialize_runtime_scene(state, empty_root);

  check(!summary.room_grid_loaded,
        "scene bootstrap should not report room load success without assets");
  check(summary.using_placeholder,
        "scene bootstrap should explicitly mark placeholder fallback mode");
  check(state.player.is_physics_active,
        "scene bootstrap fallback should keep physics stage active");

  std::filesystem::remove_all(empty_root);
}

} // namespace

void run_bootstrap_wiring_tests() {
  test_bootstrap_entry_runs_without_crashing();
  test_bootstrap_tick_wires_input_dispatch_and_render();
  test_integrated_loop_exits_cleanly_when_quit_is_requested();
  test_dispatcher_moves_player_when_left_input_is_active();
  test_render_loop_renders_multiple_frames();
  test_render_loop_updates_state_while_presenting_frames();
  test_render_bootstrap_frame_uses_room_tile_data();
  test_bootstrap_loader_reads_reference_room_data();
  test_scene_bootstrap_discovers_reference_assets_from_repo_root();
  test_scene_bootstrap_does_not_commit_failed_candidate_state();
  test_scene_bootstrap_falls_back_with_missing_assets();
}
