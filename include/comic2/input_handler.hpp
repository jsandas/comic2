#pragma once

#include "comic2/game_state.hpp"

namespace comic2 {

// Keyboard input handler using SDL2
// Provides real keyboard event processing instead of environment variables
class KeyboardInputHandler {
public:
  KeyboardInputHandler();
  ~KeyboardInputHandler();

  // Poll current keyboard state and update InputState
  // Returns true if the window should continue running (false on quit request)
  bool poll_events(InputState &input);

  // Disable copying
  KeyboardInputHandler(const KeyboardInputHandler &) = delete;
  KeyboardInputHandler &operator=(const KeyboardInputHandler &) = delete;

private:
  struct Impl;
  Impl *impl_{};
};

} // namespace comic2
