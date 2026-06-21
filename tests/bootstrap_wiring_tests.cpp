#include <cstdlib>
#include <filesystem>
#include <stdexcept>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/game_state.hpp"
#include "comic2/renderer.hpp"

namespace {

void set_test_env(const char *name, const char *value, int overwrite) {
#ifdef _WIN32
  // Windows doesn't have setenv; use _putenv_s instead.
  (void)overwrite;
  _putenv_s(name, value);
#else
  setenv(name, value, overwrite);
#endif
}

void check(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
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

void test_bootstrap_entry_runs_without_crashing() {
  const auto empty_root =
      std::filesystem::temp_directory_path() / "comic2_bootstrap_empty";
  std::filesystem::remove_all(empty_root);
  std::filesystem::create_directories(empty_root);

  set_test_env("COMIC2_BOOTSTRAP_TICKS", "1", 1);
  set_test_env("COMIC2_INPUT_LEFT", "0", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  const int exit_code =
      comic2::run_bootstrap_entry(empty_root);

  check(exit_code == 2,
        "bootstrap entry should report missing bootstrap resources");

  std::filesystem::remove_all(empty_root);
}

void test_bootstrap_tick_wires_input_dispatch_and_render() {
  set_test_env("COMIC2_INPUT_LEFT", "1", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "1", 1);
  set_test_env("COMIC2_INPUT_JUMP", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  auto state = comic2::make_default_runtime_state();
  state.player.is_airborne = false;
  state.player.is_physics_active = true;

  auto dispatcher = comic2::make_default_game_dispatcher();
  RecordingPresenter presenter;

  const auto summary =
        comic2::run_bootstrap_tick(state, dispatcher, presenter);

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

} // namespace

void run_bootstrap_wiring_tests() {
  test_bootstrap_entry_runs_without_crashing();
  test_bootstrap_tick_wires_input_dispatch_and_render();
}
