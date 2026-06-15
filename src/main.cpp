#include <filesystem>
#include <iostream>

#include "comic2/default_handlers.hpp"
#include "comic2/dispatcher.hpp"
#include "comic2/game_state.hpp"

int main(int argc, char **argv) {
  try {
    const std::filesystem::path root =
        (argc > 1) ? std::filesystem::path(argv[1])
                   : std::filesystem::current_path();

    std::cout << "Starting comic2 bootstrap from: " << root.string() << "\n";

    comic2::RuntimeState state{};
    state.current_level = 0;
    state.current_room = 0;
    state.player.x = 64;
    state.player.y = 96;
    state.player.is_airborne = false;
    state.player.is_physics_active = true;

    comic2::GameDispatcher dispatcher;
    comic2::install_default_stage_hooks(dispatcher);
    dispatcher.set_trace_enabled(true);

    const auto result = dispatcher.run_tick(state);
    std::cout << "Bootstrap tick stage=" << comic2::to_string(result.stage)
              << " hook_executed=" << std::boolalpha << result.hook_executed
              << std::noboolalpha << "\n";
    std::cout << "Bootstrap ready: level=" << state.current_level
              << " room=" << state.current_room << "\n";

    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 1;
  }
}
