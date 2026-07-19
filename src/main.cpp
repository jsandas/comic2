#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>
#include <limits>

#include "comic2/audio.hpp"
#include "comic2/bootstrap.hpp"
#include "comic2/default_handlers.hpp"
#include "comic2/renderer.hpp"
#include "comic2/resource_loader.hpp"

int main(int argc, char **argv) {
  try {
    const std::filesystem::path root = (argc > 1)
                                           ? std::filesystem::path(argv[1])
                                           : std::filesystem::current_path();

#ifdef COMIC2_USE_SDL2
    comic2::Sdl2FramePresenter presenter(640, 480);
#else
    comic2::MemoryFramePresenter presenter;
#endif
    auto audio_backend = comic2::make_default_audio_backend();

    // Run until user quits (frame budget is effectively unbounded)
    const auto loop_summary = comic2::run_integrated_bootstrap_loop(
        root, presenter, std::numeric_limits<int>::max(),
        std::chrono::milliseconds(16), audio_backend.get());

    std::cout << "render loop complete: frames_rendered="
              << loop_summary.loop.frames_rendered
              << " ticks_processed=" << loop_summary.loop.ticks_processed
              << " last_stage=" << comic2::to_string(loop_summary.loop.last_stage)
              << " quit_requested=" << std::boolalpha
              << loop_summary.loop.quit_requested << std::noboolalpha << "\n";
    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 1;
  }
}
