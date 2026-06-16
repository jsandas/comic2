#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

#include "comic2/game_state.hpp"
#include "comic2/resource_formats.hpp"
#include "comic2/types.hpp"

namespace comic2 {

struct BootstrapLoadSummary {
  std::size_t metadata_files_tried = 0;
  std::size_t sprite_files_tried = 0;
  bool room_grid_loaded = false;
};

std::vector<std::uint8_t> load_file_bytes(const std::filesystem::path &path);
BootstrapLoadSummary load_initial_bootstrap_resources(RuntimeState &state,
                                                      const std::filesystem::path &root);

Ega4PlaneImage load_frpak_fullscreen_image(const std::filesystem::path &path);

} // namespace comic2
