#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>

#include "comic2/game_state.hpp"
#include "comic2/resource_formats.hpp"
#include "comic2/types.hpp"

namespace comic2 {

struct BootstrapLoadSummary;

std::optional<std::vector<std::uint8_t>>
load_file_bytes(const std::filesystem::path &path);
BootstrapLoadSummary
load_initial_bootstrap_resources(RuntimeState &state,
                                 const std::filesystem::path &root);

} // namespace comic2
