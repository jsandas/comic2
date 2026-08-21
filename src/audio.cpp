#include "comic2/audio.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "comic2/game_state.hpp"

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

SoundStream make_stream_from_offset(std::uint16_t stream_offset) {
  SoundStream stream;
  stream.priority = 1;
  switch (stream_offset) {
  case 0x00DB:
    stream.words = {0x0300u, 0x0004u, 0x0280u, 0x0003u, 0x0000u, 0x0000u};
    break;
  case 0x965E:
    stream.words = {0x0200u, 0x0006u, 0x0100u, 0x0004u, 0x0000u, 0x0000u};
    break;
  case 0x9676:
    stream.words = {0x0180u, 0x0002u, 0x0200u, 0x0002u, 0x0000u, 0x0000u};
    break;
  case 0x96B6:
    stream.words = {0x0250u, 0x0004u, 0x01F0u, 0x0003u, 0x0000u, 0x0000u};
    break;
  default:
    stream.words = {0x01C0u, 0x0003u, 0x0000u, 0x0000u};
    break;
  }
  return stream;
}

bool read_env_bool(const char *name) {
  const char *value = std::getenv(name);
  return value != nullptr && (*value == '1' || *value == 't' || *value == 'T' ||
                              *value == 'y' || *value == 'Y');
}

[[maybe_unused]] SoundStream make_event_stream_impl(AudioEvent event) {
  switch (event) {
  case AudioEvent::StartupChime:
    return SoundStream{{0x0300u, 0x0004u, 0x0280u, 0x0003u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::Jump:
    return SoundStream{{0x0180u, 0x0002u, 0x0200u, 0x0002u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::Hazard:
    return SoundStream{{0x0200u, 0x0006u, 0x0100u, 0x0004u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::Shoot:
    return SoundStream{{0x01E0u, 0x0002u, 0x00C0u, 0x0001u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::Hit:
    return SoundStream{{0x0200u, 0x0003u, 0x0100u, 0x0002u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::Pickup:
    return SoundStream{{0x0400u, 0x0002u, 0x0500u, 0x0002u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::Death:
    return SoundStream{{0x00C0u, 0x0008u, 0x0080u, 0x0004u, 0x0000u, 0x0000u},
                       1};
  case AudioEvent::LevelStart:
    return SoundStream{{0x0250u, 0x0004u, 0x01F0u, 0x0003u, 0x0000u, 0x0000u},
                       1};
  }
  return SoundStream{{0x01C0u, 0x0003u, 0x0000u, 0x0000u}, 1};
}

#ifdef COMIC2_USE_SDL2
class Sdl2ToneAudioBackend final : public IAudioBackend {
public:
  bool initialize() override {
    if (available_) {
      return true;
    }
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

    const SoundStream stream = make_event_stream_impl(event);
    const int sample_rate =
        obtained_spec_.freq > 0 ? obtained_spec_.freq : 22050;
    const int sample_count = std::max(1, sample_rate / 40);
    auto pcm = synthesize_sound_samples(stream, sample_rate, sample_count);
    if (pcm.empty()) {
      return;
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
  bool available_ = false;
  bool owns_sdl_audio_subsystem_ = false;
  std::size_t events_enqueued_ = 0;
  SDL_AudioDeviceID device_id_ = 0;
  SDL_AudioSpec obtained_spec_{};
};
#endif

} // namespace

SoundPlaybackResult handle_sound_command(SoundPlaybackState &state,
                                         SoundCommand command,
                                         std::uint16_t stream_offset,
                                         std::uint16_t stream_segment,
                                         std::uint8_t priority) {
  SoundPlaybackResult result{.playing = state.is_playing,
                             .priority = state.priority};
  switch (command) {
  case SoundCommand::SetGateOn:
    state.gate_enabled = true;
    state.is_enabled = true;
    break;
  case SoundCommand::Play: {
    const bool replace = !state.is_playing || priority >= state.priority;
    if (replace) {
      state.is_playing = true;
      state.is_enabled = true;
      state.priority = priority;
      state.stream_offset = stream_offset;
      state.stream_segment = stream_segment;
      state.tick_remaining = 0;
      state.stream_index = 0;
      state.stream_words = make_stream_from_offset(stream_offset).words;
    }
    break;
  }
  case SoundCommand::SetGateOff:
    state.gate_enabled = false;
    state.is_enabled = false;
    break;
  case SoundCommand::Stop:
    state.is_playing = false;
    state.tick_remaining = 0;
    state.stream_index = 0;
    break;
  case SoundCommand::Query:
    break;
  }

  result.playing = state.is_playing && state.is_enabled;
  result.priority = state.priority;
  return result;
}

SoundStream make_stream_for_event(AudioEvent event) {
  return make_event_stream_impl(event);
}

std::vector<std::int16_t> synthesize_sound_samples(const SoundStream &stream,
                                                   int sample_rate,
                                                   int sample_count) {
  std::vector<std::int16_t> samples(static_cast<std::size_t>(sample_count), 0);
  if (sample_count <= 0 || stream.words.empty()) {
    return samples;
  }

  const std::uint16_t divisor = stream.words.front();
  const std::uint16_t duration = stream.words.size() > 1 ? stream.words[1] : 1;
  const int frequency =
      divisor != 0 ? 1193182 / std::max<int>(1, divisor) : 440;
  const int half_period = std::max(1, sample_rate / std::max(1, frequency * 2));
  constexpr int kAmplitude = 10000;
  const int note_length = std::max(1, duration * 16);

  for (int i = 0; i < sample_count; ++i) {
    const int phase = (i * 2) % (half_period * 2);
    const bool high = (phase < half_period);
    const int envelope = std::min(note_length, std::max(0, sample_count - i));
    samples[static_cast<std::size_t>(i)] =
        high ? static_cast<std::int16_t>(kAmplitude * envelope / note_length)
             : static_cast<std::int16_t>(-kAmplitude * envelope / note_length);
  }

  return samples;
}

void advance_sound_stream(SoundPlaybackState &state, int sample_rate) {
  (void)sample_rate;
  if (!state.is_playing || !state.is_enabled) {
    return;
  }

  if (state.tick_remaining == 0) {
    if (state.stream_index + 1 >= state.stream_words.size()) {
      state.is_playing = false;
      state.tick_remaining = 0;
      return;
    }

    const std::uint16_t divisor = state.stream_words[state.stream_index];
    const std::uint16_t duration = state.stream_words[state.stream_index + 1];
    state.stream_index += 2;
    if (divisor == 0) {
      state.is_playing = false;
      state.tick_remaining = 0;
      return;
    }
    state.tick_remaining = duration == 0 ? 1 : duration;
    return;
  }

  if (state.tick_remaining > 0) {
    --state.tick_remaining;
  }
}

void queue_audio_event(RuntimeState &state, AudioEvent event) {
  if (state.audio.mute || !state.audio.audio_enabled) {
    return;
  }
  state.audio.pending_events.push_back(event);
}

void flush_audio_events(RuntimeState &state, IAudioBackend *backend) {
  if (backend == nullptr) {
    state.audio.pending_events.clear();
    return;
  }

  for (const auto event : state.audio.pending_events) {
    backend->enqueue_event(event);
  }
  state.audio.pending_events.clear();
}

bool NullAudioBackend::initialize() { return true; }

void NullAudioBackend::enqueue_event(AudioEvent event) {
  (void)event;
  ++events_enqueued_;
}

void NullAudioBackend::update() {}

void NullAudioBackend::shutdown() {}

bool NullAudioBackend::is_available() const { return false; }

std::size_t NullAudioBackend::events_enqueued() const {
  return events_enqueued_;
}

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
