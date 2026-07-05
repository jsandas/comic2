#include "comic2/audio.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>

#ifdef COMIC2_USE_SDL2
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL.h>
#else
#error "SDL2 headers not found"
#endif
#endif

namespace comic2 {

namespace {

bool read_env_bool(const char *name) {
  const char *value = std::getenv(name);
  return value != nullptr && (*value == '1' || *value == 't' || *value == 'T' ||
                              *value == 'y' || *value == 'Y');
}

struct ToneSpec {
  int frequency_hz;
  int duration_ms;
};

ToneSpec tone_for_event(AudioEvent event) {
  switch (event) {
  case AudioEvent::StartupChime:
    return ToneSpec{.frequency_hz = 523, .duration_ms = 80};
  case AudioEvent::Jump:
    return ToneSpec{.frequency_hz = 660, .duration_ms = 40};
  case AudioEvent::Hazard:
    return ToneSpec{.frequency_hz = 180, .duration_ms = 120};
  }
  return ToneSpec{.frequency_hz = 440, .duration_ms = 50};
}

#ifdef COMIC2_USE_SDL2
class Sdl2ToneAudioBackend final : public IAudioBackend {
public:
  bool initialize() override {
    if (initialized_) {
      return available_;
    }
    initialized_ = true;

    if (read_env_bool("COMIC2_DISABLE_AUDIO")) {
      return false;
    }

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
      if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        return false;
      }
      owns_sdl_audio_subsystem_ = true;
    }

    SDL_AudioSpec desired{};
    desired.freq = 22050;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = 1024;
    desired.callback = nullptr;

    device_id_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained_spec_, 0);
    if (device_id_ == 0) {
      return false;
    }

    SDL_PauseAudioDevice(device_id_, 0);
    available_ = true;
    return true;
  }

  void enqueue_event(AudioEvent event) override {
    ++events_enqueued_;
    if (!available_ || device_id_ == 0) {
      return;
    }

    const ToneSpec spec = tone_for_event(event);
    const int sample_rate = obtained_spec_.freq > 0 ? obtained_spec_.freq : 22050;
    const int sample_count = (sample_rate * spec.duration_ms) / 1000;
    if (sample_count <= 0) {
      return;
    }

    std::vector<std::int16_t> pcm(static_cast<std::size_t>(sample_count), 0);
    constexpr double kTwoPi = 6.28318530717958647692;
    constexpr double kAmplitude = 12000.0;

    for (int i = 0; i < sample_count; ++i) {
      const double t = static_cast<double>(i) / static_cast<double>(sample_rate);
      const double phase = kTwoPi * static_cast<double>(spec.frequency_hz) * t;
      pcm[static_cast<std::size_t>(i)] =
          static_cast<std::int16_t>(std::sin(phase) * kAmplitude);
    }

    SDL_QueueAudio(device_id_, pcm.data(),
                   static_cast<Uint32>(pcm.size() * sizeof(std::int16_t)));
  }

  void update() override {
    if (!available_ || device_id_ == 0) {
      return;
    }

    // Keep queued data bounded to avoid unbounded audio backlog.
    const Uint32 queued = SDL_GetQueuedAudioSize(device_id_);
    const Uint32 max_queued_bytes =
        static_cast<Uint32>(obtained_spec_.freq * sizeof(std::int16_t));
    if (queued > max_queued_bytes) {
      SDL_ClearQueuedAudio(device_id_);
    }
  }

  void shutdown() override {
    if (device_id_ != 0) {
      SDL_CloseAudioDevice(device_id_);
      device_id_ = 0;
    }
    available_ = false;

    if (owns_sdl_audio_subsystem_) {
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
      owns_sdl_audio_subsystem_ = false;
    }
  }

  bool is_available() const override { return available_; }
  std::size_t events_enqueued() const override { return events_enqueued_; }

  ~Sdl2ToneAudioBackend() override { shutdown(); }

private:
  bool initialized_ = false;
  bool available_ = false;
  bool owns_sdl_audio_subsystem_ = false;
  std::size_t events_enqueued_ = 0;
  SDL_AudioDeviceID device_id_ = 0;
  SDL_AudioSpec obtained_spec_{};
};
#endif

} // namespace

bool NullAudioBackend::initialize() { return true; }

void NullAudioBackend::enqueue_event(AudioEvent event) {
  (void)event;
  ++events_enqueued_;
}

void NullAudioBackend::update() {}

void NullAudioBackend::shutdown() {}

bool NullAudioBackend::is_available() const { return false; }

std::size_t NullAudioBackend::events_enqueued() const { return events_enqueued_; }

std::unique_ptr<IAudioBackend> make_default_audio_backend() {
#ifdef COMIC2_USE_SDL2
  auto sdl_backend = std::make_unique<Sdl2ToneAudioBackend>();
  if (sdl_backend->initialize()) {
    return sdl_backend;
  }
#endif
  auto null_backend = std::make_unique<NullAudioBackend>();
  null_backend->initialize();
  return null_backend;
}

} // namespace comic2
