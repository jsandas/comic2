#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <process.h>
#endif

#include "comic2/audio.hpp"
#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/renderer.hpp"

namespace {

void set_test_env(const char *name, const char *value, int overwrite) {
#ifdef _WIN32
  (void)overwrite;
  _putenv_s(name, value);
#else
  setenv(name, value, overwrite);
#endif
}

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

class RecordingAudioBackend final : public comic2::IAudioBackend {
public:
  bool initialize() override {
    ++initialize_calls;
    initialized = true;
    return true;
  }

  void enqueue_event(comic2::AudioEvent event) override {
    events.push_back(event);
  }

  void update() override { ++update_calls; }

  void shutdown() override {
    ++shutdown_calls;
    initialized = false;
  }

  bool is_available() const override { return initialized; }

  std::size_t events_enqueued() const override { return events.size(); }

  bool initialized = false;
  int initialize_calls = 0;
  int update_calls = 0;
  int shutdown_calls = 0;
  std::vector<comic2::AudioEvent> events;
};

class FailingAudioBackend final : public comic2::IAudioBackend {
public:
  bool initialize() override {
    ++initialize_calls;
    return false;
  }

  void enqueue_event(comic2::AudioEvent) override { ++enqueue_calls; }

  void update() override { ++update_calls; }

  void shutdown() override { ++shutdown_calls; }

  bool is_available() const override { return false; }

  std::size_t events_enqueued() const override { return 0; }

  int initialize_calls = 0;
  int enqueue_calls = 0;
  int update_calls = 0;
  int shutdown_calls = 0;
};

void test_run_render_loop_audio_lifecycle_and_startup_event() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  comic2::GameDispatcher dispatcher = comic2::make_default_game_dispatcher();
  comic2::MemoryFramePresenter presenter;
  RecordingAudioBackend audio;

  const auto summary = comic2::run_render_loop(
      state, dispatcher, presenter, 2, std::chrono::milliseconds(0), &audio);

  expect(summary.frames_rendered == 2,
         "render loop should process requested frames with audio attached");
  expect(audio.initialize_calls == 1,
         "audio backend should be initialized once per loop invocation");
  expect(audio.shutdown_calls == 1,
         "audio backend should be shut down once per loop invocation");
  expect(audio.update_calls == 2,
         "audio backend should receive one non-blocking update per frame");
  expect(!audio.events.empty(),
         "audio backend should receive at least a startup event");
  expect(audio.events.front() == comic2::AudioEvent::StartupChime,
         "first enqueued audio event should be the startup chime");
}

void test_run_render_loop_emits_jump_event_on_takeoff() {
  set_test_env("COMIC2_FORCE_ENV_INPUT", "1", 1);
  set_test_env("COMIC2_INPUT_JUMP", "1", 1);
  set_test_env("COMIC2_INPUT_LEFT", "0", 1);
  set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
  set_test_env("COMIC2_INPUT_DOWN", "0", 1);

  struct EnvCleanup {
    ~EnvCleanup() {
      set_test_env("COMIC2_FORCE_ENV_INPUT", "0", 1);
      set_test_env("COMIC2_INPUT_JUMP", "0", 1);
      set_test_env("COMIC2_INPUT_LEFT", "0", 1);
      set_test_env("COMIC2_INPUT_RIGHT", "0", 1);
      set_test_env("COMIC2_INPUT_DOWN", "0", 1);
    }
  } env_cleanup;
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.is_physics_active = false;
  state.player.is_airborne = false;
  state.player.jump_counter = 1;

  comic2::GameDispatcher dispatcher = comic2::make_default_game_dispatcher();
  comic2::MemoryFramePresenter presenter;
  RecordingAudioBackend audio;

  const auto summary = comic2::run_render_loop(
      state, dispatcher, presenter, 1, std::chrono::milliseconds(0), &audio);

  expect(summary.frames_rendered == 1,
         "render loop should process one frame for jump trigger test");

  const bool found_jump = std::any_of(
      audio.events.begin(), audio.events.end(),
      [](const auto event) { return event == comic2::AudioEvent::Jump; });

  expect(
      found_jump,
      "audio backend should receive jump event when player becomes airborne");
}

void test_run_render_loop_skips_audio_when_initialization_fails() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  comic2::GameDispatcher dispatcher = comic2::make_default_game_dispatcher();
  comic2::MemoryFramePresenter presenter;
  FailingAudioBackend audio;

  const auto summary = comic2::run_render_loop(
      state, dispatcher, presenter, 1, std::chrono::milliseconds(0), &audio);

  expect(summary.frames_rendered == 1,
         "render loop should still process frames when audio initialization "
         "fails");
  expect(audio.initialize_calls == 1,
         "audio backend should still receive one initialization attempt");
  expect(audio.enqueue_calls == 0,
         "audio events should not be enqueued when initialization fails");
  expect(audio.update_calls == 0,
         "audio backend update should not run when initialization fails");
  expect(audio.shutdown_calls == 0,
         "audio backend shutdown should not run when initialization fails");
}

void test_null_audio_backend_accepts_events() {
  comic2::NullAudioBackend null_audio;
  expect(null_audio.initialize(),
         "null audio backend initialize should always succeed");
  expect(!null_audio.is_available(),
         "null audio backend should remain unavailable by design");

  null_audio.enqueue_event(comic2::AudioEvent::StartupChime);
  null_audio.enqueue_event(comic2::AudioEvent::Hazard);
  null_audio.update();
  null_audio.shutdown();

  expect(null_audio.events_enqueued() == 2,
         "null backend should still track enqueued events for diagnostics");
}

} // namespace

void run_audio_backend_tests() {
  test_run_render_loop_audio_lifecycle_and_startup_event();
  test_run_render_loop_emits_jump_event_on_takeoff();
  test_run_render_loop_skips_audio_when_initialization_fails();
  test_null_audio_backend_accepts_events();
}
