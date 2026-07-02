#include "comic2/input_handler.hpp"

#include <cstdlib>
#include <stdexcept>

#ifdef COMIC2_USE_SDL2
#include <SDL.h>
#endif

namespace comic2 {

// Helper function to read boolean environment variables
// This mirrors read_bootstrap_bool_env behavior
static bool read_env_bool(const char *name) {
  const char *value = std::getenv(name);
  return value != nullptr && (*value == '1' || *value == 't' || *value == 'T' ||
                              *value == 'y' || *value == 'Y');
}

struct KeyboardInputHandler::Impl {
  bool sdl_available{false};
  bool sdl_initialized{false};
  bool quit_requested{false};

#ifdef COMIC2_USE_SDL2
  Impl() {
    // Try to initialize SDL for event handling, but don't require it
    // If SDL isn't available (e.g., headless environment), fall back to env vars
    int init_flags = SDL_WasInit(SDL_INIT_EVERYTHING);
    if (init_flags == 0) {
      // SDL not yet initialized - try to initialize it
      if (SDL_Init(SDL_INIT_EVENTS) == 0) {
        sdl_available = true;
        sdl_initialized = true;
      }
      // If SDL_Init fails, just leave sdl_available=false for fallback
    } else {
      // SDL is already initialized (e.g., by the presenter)
      sdl_available = true;
      // Don't set sdl_initialized since we didn't initialize it
    }
  }

  ~Impl() {
    if (sdl_initialized) {
      SDL_Quit();
    }
  }
#else
  Impl() {}
  ~Impl() {}
#endif
};

KeyboardInputHandler::KeyboardInputHandler() : impl_(new Impl()) {}

KeyboardInputHandler::~KeyboardInputHandler() { delete impl_; }

bool KeyboardInputHandler::poll_events(InputState &input) {
  // Check if we should force environment variable mode (for testing)
  bool force_env_input = read_env_bool("COMIC2_FORCE_ENV_INPUT");

#ifdef COMIC2_USE_SDL2
  if (!force_env_input && impl_->sdl_available) {
    // Use SDL2 keyboard input
    SDL_Event event;

    // Reset input state
    input.jump_pressed = false;
    input.left_pressed = false;
    input.right_pressed = false;
    input.down_pressed = false;

    // Process all pending events
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          impl_->quit_requested = true;
          break;

        case SDL_KEYDOWN:
          if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            impl_->quit_requested = true;
          } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT ||
                     event.key.keysym.sym == SDLK_a) {
            input.left_pressed = true;
          } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT ||
                     event.key.keysym.sym == SDLK_d) {
            input.right_pressed = true;
          } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE ||
                     event.key.keysym.sym == SDLK_UP ||
                     event.key.keysym.scancode == SDL_SCANCODE_W) {
            input.jump_pressed = true;
          } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN ||
                     event.key.keysym.sym == SDLK_s) {
            input.down_pressed = true;
          }
          break;

        case SDL_KEYUP:
          // Note: We could track key states separately for continuous input,
          // but for now we process only KEYDOWN events. If we need continuous
          // holding, we'd need to maintain state and query it here.
          break;

        default:
          break;
      }
    }

    return !impl_->quit_requested;
  }
#endif

  // Fallback: use environment variables when SDL2 is not available
  input.jump_pressed = read_env_bool("COMIC2_INPUT_JUMP");
  input.left_pressed = read_env_bool("COMIC2_INPUT_LEFT");
  input.right_pressed = read_env_bool("COMIC2_INPUT_RIGHT");
  input.down_pressed = read_env_bool("COMIC2_INPUT_DOWN");

  if (read_env_bool("COMIC2_INPUT_QUIT")) {
    impl_->quit_requested = true;
  }

  return !impl_->quit_requested;
}

bool KeyboardInputHandler::is_quit_requested() const {
  return impl_->quit_requested;
}

} // namespace comic2
