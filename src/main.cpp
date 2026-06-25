#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>

#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/renderer.hpp"
#include "comic2/resource_loader.hpp"

int main(int argc, char **argv) {
  try {
    const std::filesystem::path root = (argc > 1)
                                           ? std::filesystem::path(argv[1])
                                           : std::filesystem::current_path();

    auto state = comic2::make_default_runtime_state();
    const auto bootstrap = comic2::load_initial_bootstrap_resources(state, root);
    if (!bootstrap.room_grid_loaded) {
      std::cerr << "WARNING: no bootstrap room grid loaded from " << root.string()
                << " (using fallback frame path)\n";
    }

    auto dispatcher = comic2::make_default_game_dispatcher();
    dispatcher.set_trace_enabled(true);

#ifdef COMIC2_USE_SDL2
    comic2::Sdl2FramePresenter presenter(640, 480);
#else
    comic2::MemoryFramePresenter presenter;
#endif

    const auto loop_summary = comic2::run_render_loop(
        state, dispatcher, presenter, 120, std::chrono::milliseconds(16));

    std::cout << "render loop complete: frames_rendered="
              << loop_summary.frames_rendered
              << " ticks_processed=" << loop_summary.ticks_processed
              << " last_stage=" << comic2::to_string(loop_summary.last_stage)
              << "\n";
    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 1;
  }
}
