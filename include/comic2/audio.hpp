#pragma once

#include <cstddef>
#include <memory>

namespace comic2 {

enum class AudioEvent {
  StartupChime,
  Jump,
  Hazard,
};

class IAudioBackend {
public:
  virtual ~IAudioBackend() = default;

  virtual bool initialize() = 0;
  virtual void enqueue_event(AudioEvent event) = 0;
  virtual void update() = 0;
  virtual void shutdown() = 0;

  virtual bool is_available() const = 0;
  virtual std::size_t events_enqueued() const = 0;
};

class NullAudioBackend final : public IAudioBackend {
public:
  bool initialize() override;
  void enqueue_event(AudioEvent event) override;
  void update() override;
  void shutdown() override;

  bool is_available() const override;
  std::size_t events_enqueued() const override;

private:
  std::size_t events_enqueued_ = 0;
};

std::unique_ptr<IAudioBackend> make_default_audio_backend();

} // namespace comic2
