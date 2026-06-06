#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

#include "comic2/resource_formats.hpp"
#include "comic2/types.hpp"

namespace comic2 {

std::vector<std::uint8_t> load_file_bytes(const std::filesystem::path &path);

Ega4PlaneImage load_frpak_fullscreen_image(const std::filesystem::path &path);

} // namespace comic2
