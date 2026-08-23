#include "comic2/persistence.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace comic2 {
namespace {

constexpr std::uint8_t kConfigVersion = 1;
constexpr std::uint8_t kSnapshotVersion = 1;

void write_u16(std::vector<std::uint8_t> &bytes, std::uint16_t value) {
  bytes.push_back(static_cast<std::uint8_t>(value & 0xFF));
  bytes.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
}

std::uint16_t read_u16(const std::vector<std::uint8_t> &bytes,
                       std::size_t &offset) {
  if (offset + 2 > bytes.size()) {
    return 0;
  }
  const std::uint16_t value =
      static_cast<std::uint16_t>(bytes[offset]) |
      (static_cast<std::uint16_t>(bytes[offset + 1]) << 8);
  offset += 2;
  return value;
}

} // namespace

void cfg_load_options_or_defaults(RuntimeState &state,
                                  const std::vector<std::uint8_t> &bytes) {
  if (bytes.empty()) {
    state.ui.music_volume = 0x04;
    state.ui.sfx_volume = 0x04;
    state.ui.display_scale = 0x01;
    return;
  }

  std::size_t offset = 0;
  const std::uint8_t version = bytes[offset++];
  (void)version;

  if (offset < bytes.size()) {
    state.ui.music_volume = bytes[offset++];
  }
  if (offset < bytes.size()) {
    state.ui.sfx_volume = bytes[offset++];
  }
  if (offset < bytes.size()) {
    state.ui.display_scale = bytes[offset++];
  }
}

void cfg_save_options(const RuntimeState &state,
                      std::vector<std::uint8_t> &bytes) {
  bytes.clear();
  bytes.push_back(kConfigVersion);
  bytes.push_back(state.ui.music_volume);
  bytes.push_back(state.ui.sfx_volume);
  bytes.push_back(state.ui.display_scale);
}

void savegame_write_snapshot(const RuntimeState &state,
                             std::vector<std::uint8_t> &bytes) {
  bytes.clear();
  bytes.push_back(kSnapshotVersion);
  write_u16(bytes, state.current_level);
  write_u16(bytes, state.current_room);
  bytes.push_back(state.player.score & 0xFF);
  bytes.push_back((state.player.score >> 8) & 0xFF);
  bytes.push_back(state.player.gems);
  bytes.push_back(state.player.lives);
  bytes.push_back(state.player.hp);
  bytes.push_back(static_cast<std::uint8_t>(state.ui.menu_state));
}

void savegame_read_snapshot(RuntimeState &state,
                            const std::vector<std::uint8_t> &bytes) {
  if (bytes.empty()) {
    return;
  }

  std::size_t offset = 0;
  const auto version = bytes[offset++];
  (void)version;

  state.current_level = read_u16(bytes, offset);
  state.current_room = read_u16(bytes, offset);

  if (offset + 2 > bytes.size()) {
    return;
  }
  const auto score_lo = bytes[offset++];
  const auto score_hi = bytes[offset++];
  state.player.score = static_cast<std::uint16_t>(
      score_lo | (static_cast<std::uint16_t>(score_hi) << 8));

  if (offset < bytes.size()) {
    state.player.gems = bytes[offset++];
  } else {
    return;
  }
  if (offset < bytes.size()) {
    state.player.lives = bytes[offset++];
  } else {
    return;
  }
  if (offset < bytes.size()) {
    state.player.hp = bytes[offset++];
  } else {
    return;
  }
  if (offset < bytes.size()) {
    state.ui.menu_state = static_cast<MenuState>(bytes[offset++]);
  }
}

} // namespace comic2
