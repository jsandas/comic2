#include <stdexcept>

#include "comic2/game_state.hpp"
#include "comic2/renderer.hpp"
#include "comic2/ui.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void test_bcd_counter_helpers() {
  using comic2::bcd_decrement;
  using comic2::bcd_increment;

  expect(bcd_increment(0x00) == 0x01, "BCD increment should advance 00->01");
  expect(bcd_increment(0x09) == 0x10, "BCD increment should carry 09->10");
  expect(bcd_increment(0x99) == 0x99, "BCD increment should clamp at 99");
  expect(bcd_decrement(0x10) == 0x09, "BCD decrement should carry 10->09");
  expect(bcd_decrement(0x00) == 0x00, "BCD decrement should clamp at 00");
}

void test_hud_renders_counters_and_icons() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.score = 0x1234;
  state.player.gems = 0x07;
  state.player.lives = 0x03;
  state.ui.active_mode_mask = 0x01;
  state.ui.inventory_mask = 0x03;

  comic2::EgaPlanarSurface frame(320, 200);
  frame.clear(0x00);
  comic2::hud_render_overlay(frame, state);

  bool saw_pixels = false;
  for (std::size_t y = 0; y < frame.height_rows(); ++y) {
    for (std::size_t x = 0; x < frame.width_pixels(); ++x) {
      if (frame.get_plane_byte(0, x / 8, y) != 0x00) {
        saw_pixels = true;
        break;
      }
    }
    if (saw_pixels) {
      break;
    }
  }

  expect(saw_pixels, "HUD overlay should draw visible pixels");
}

void test_menu_state_machine_and_option_navigation() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.ui.menu_state = comic2::MenuState::Pause;
  state.ui.option_labels = {"Resume", "Save", "Quit"};
  state.ui.selected_option_index = 0;

  comic2::ui_select_next_valid_option(state);
  expect(state.ui.selected_option_index == 1,
         "next valid option should advance the selection");

  comic2::ui_option_list_input_loop(state,
                                    comic2::InputState{.pause_pressed = true});
  expect(state.ui.menu_state == comic2::MenuState::Pause,
         "pause input should keep the menu active");
}

} // namespace

void run_ui_tests() {
  test_bcd_counter_helpers();
  test_hud_renders_counters_and_icons();
  test_menu_state_machine_and_option_navigation();
}
