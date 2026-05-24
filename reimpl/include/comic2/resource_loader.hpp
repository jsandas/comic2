#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

#include "comic2/resource_formats.hpp"
#include "comic2/types.hpp"

namespace comic2 {

std::vector<std::uint8_t> load_file_bytes(const std::filesystem::path& path);

Ega4PlaneImage load_frpak_fullscreen_image(const std::filesystem::path& path);
SignedRleResult load_signed_rle_blob(const std::filesystem::path& path);

FrdataRoomEntry read_frdata_room_entry(std::span<const std::uint8_t> frdata_bytes, std::uint16_t room_index);

}  // namespace comic2
