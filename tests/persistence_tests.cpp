#include <stdexcept>
#include <vector>

#include "comic2/game_state.hpp"
#include "comic2/persistence.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void test_config_roundtrip_preserves_settings() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.ui.music_volume = 0x05;
  state.ui.sfx_volume = 0x07;
  state.ui.display_scale = 0x02;

  std::vector<std::uint8_t> bytes;
  comic2::cfg_save_options(state, bytes);
  expect(bytes.size() >= 3, "config save should write at least three bytes");

  comic2::RuntimeState loaded = comic2::make_default_runtime_state();
  comic2::cfg_load_options_or_defaults(loaded, bytes);
  expect(loaded.ui.music_volume == 0x05, "music volume should roundtrip");
  expect(loaded.ui.sfx_volume == 0x07, "sfx volume should roundtrip");
  expect(loaded.ui.display_scale == 0x02, "display scale should roundtrip");
}

void test_snapshot_roundtrip_preserves_state() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.score = 0x1234;
  state.player.gems = 0x07;
  state.player.lives = 0x03;
  state.player.hp = 0x09;
  state.current_level = 2;
  state.current_room = 4;
  state.ui.menu_state = comic2::MenuState::Options;

  std::vector<std::uint8_t> bytes;
  comic2::savegame_write_snapshot(state, bytes);
  expect(!bytes.empty(), "snapshot writer should emit data");

  comic2::RuntimeState loaded = comic2::make_default_runtime_state();
  comic2::savegame_read_snapshot(loaded, bytes);
  expect(loaded.current_level == 2, "level should roundtrip");
  expect(loaded.current_room == 4, "room should roundtrip");
  expect(loaded.player.score == 0x1234, "score should roundtrip");
  expect(loaded.player.gems == 0x07, "gems should roundtrip");
  expect(loaded.player.lives == 0x03, "lives should roundtrip");
  expect(loaded.ui.menu_state == comic2::MenuState::Options,
         "menu state should roundtrip");
}

} // namespace

void run_persistence_tests() {
  test_config_roundtrip_preserves_settings();
  test_snapshot_roundtrip_preserves_state();
}
