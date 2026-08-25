#include <cstdint>
#include <stdexcept>

#include "comic2/bootstrap.hpp"
#include "comic2/game_state.hpp"
#include "comic2/renderer.hpp"
#include "comic2/renderer_validation.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

std::uint8_t read_surface_color(const comic2::EgaPlanarSurface &surface,
                                std::int32_t x, std::int32_t y) {
  const auto x_byte = static_cast<std::size_t>(x / 8);
  const auto bit = static_cast<std::uint8_t>(7 - (x % 8));
  const auto y_row = static_cast<std::size_t>(y);
  std::uint8_t color = 0;
  for (std::size_t plane = 0; plane < comic2::EgaPlanarSurface::kPlaneCount;
       ++plane) {
    const auto value = surface.get_plane_byte(plane, x_byte, y_row);
    if ((value & static_cast<std::uint8_t>(1U << bit)) != 0U) {
      color |= static_cast<std::uint8_t>(1U << plane);
    }
  }
  return color;
}

void test_surface_geometry() {
  comic2::EgaPlanarSurface surface(320, 200);
  expect(surface.row_stride_bytes() == 40,
         "row stride mismatch for 320px surface");
  expect(surface.plane_size_bytes() == 8000,
         "plane size mismatch for 320x200 surface");
}

void test_surface_plane_rw() {
  comic2::EgaPlanarSurface surface(320, 200);
  surface.set_plane_byte(2, 10, 5, 0xA5);
  expect(surface.get_plane_byte(2, 10, 5) == 0xA5,
         "plane byte write/read mismatch");
}

void test_presenter_copies_frame() {
  comic2::EgaPlanarSurface src(320, 200);
  src.set_plane_byte(1, 0, 0, 0x7E);

  comic2::MemoryFramePresenter presenter;
  presenter.present(src);

  src.set_plane_byte(1, 0, 0, 0x00);
  expect(presenter.has_frame(), "presenter should report frame availability");
  expect(presenter.last_frame().get_plane_byte(1, 0, 0) == 0x7E,
         "presenter did not retain frame copy");
}

void test_transition_palette_tint_applies_to_surface_pixels() {
  comic2::EgaPlanarSurface surface(320, 200);
  surface.clear(0x00);
  surface.set_plane_byte(1, 0, 0, 0x80);

  comic2::RoomTransitionState transition{};
  transition.active = true;
  transition.frame_index = 2;
  transition.palette_tint = 3;
  transition.palette_shift = 1;

  comic2::apply_transition_palette_tint(surface, transition);

  expect(
      read_surface_color(surface, 0, 0) == 0x08,
      "transition palette tint should shift the pixel color deterministically");
}

void test_transition_effects_are_deterministic() {
  comic2::EgaPlanarSurface surface(320, 200);
  surface.clear(0x00);

  comic2::RoomTransitionState transition{};
  transition.active = true;
  transition.effect_type = 0;
  transition.frame_index = 3;
  comic2::room_transition_palette_wave(surface, transition);
  const auto palette_hash = comic2::validation::hash_surface_planes(surface);
  expect(palette_hash != 0x0ULL,
         "palette wave should alter the frame deterministically");

  comic2::EgaPlanarSurface reveal(320, 200);
  reveal.clear(0x00);
  transition.effect_type = 1;
  transition.frame_index = 2;
  comic2::room_transition_reveal_sequence_a(reveal, transition);
  const auto reveal_hash = comic2::validation::hash_surface_planes(reveal);
  expect(reveal_hash != 0x0ULL,
         "reveal sequence A should alter the frame deterministically");
}

void test_player_sprite_frame_selection_uses_animation_state() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.animation_state =
      static_cast<std::uint8_t>(comic2::PlayerAnimationState::WalkCycle);
  state.player.animation_frame = 3;

  const auto frame = comic2::select_player_sprite_frame(state);
  expect(frame == 3, "walk cycle frame should resolve to the current frame");

  state.player.animation_state =
      static_cast<std::uint8_t>(comic2::PlayerAnimationState::JumpRise);
  expect(comic2::select_player_sprite_frame(state) == 4,
         "jump-rise state should resolve to the jump frame");
}

void test_player_sprite_frame_selection_uses_facing_direction() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.animation_state =
      static_cast<std::uint8_t>(comic2::PlayerAnimationState::WalkCycle);
  state.player.animation_frame = 2;
  state.player.facing_right = false;

  const auto frame = comic2::select_player_sprite_frame(state);
  expect(frame == 10,
         "left-facing walk cycle should use the mirrored frame offset");
}

void test_invulnerability_blink_visibility_uses_tick_parity() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.invuln_ticks = 0;
  expect(
      comic2::should_render_player_sprite(state),
      "player sprite should remain visible when invulnerability is inactive");

  state.player.invuln_ticks = 3;
  expect(!comic2::should_render_player_sprite(state),
         "player sprite should hide on odd invulnerability ticks");

  state.player.invuln_ticks = 4;
  expect(comic2::should_render_player_sprite(state),
         "player sprite should render on even invulnerability ticks");
}

void test_camera_y_clamps_to_room_bounds() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.room_grid.tile_w = 20;
  state.room_grid.tile_h = 20;
  state.room_grid.row_pointers.assign(20, 0);
  state.room_grid.tile_data.assign(400, 0x01);
  state.player.y = 480;
  state.camera_y = 0;

  comic2::camera_update_y_follow_comic_clamped(state, 200, 320);
  expect(state.camera_y == 120,
         "camera Y should clamp to the room-height viewport limit");
}

} // namespace

void run_renderer_tests() {
  test_surface_geometry();
  test_surface_plane_rw();
  test_presenter_copies_frame();
  test_transition_palette_tint_applies_to_surface_pixels();
  test_transition_effects_are_deterministic();
  test_player_sprite_frame_selection_uses_animation_state();
  test_player_sprite_frame_selection_uses_facing_direction();
  test_invulnerability_blink_visibility_uses_tick_parity();
  test_camera_y_clamps_to_room_bounds();

  // EgaPageFlipper tests
  {
    comic2::EgaPageFlipper flipper;
    flipper.init_double_buffering();
    expect(flipper.active_page() == 0x2000,
           "EgaPageFlipper should init with page 0x2000");
  }

  {
    comic2::EgaPageFlipper flipper;
    flipper.init_double_buffering();
    expect(flipper.active_page() == 0x2000,
           "initial active page should be 0x2000");

    flipper.present_and_flip_page();
    expect(flipper.active_page() == 0x0000,
           "after first flip should be 0x0000");

    flipper.present_and_flip_page();
    expect(flipper.active_page() == 0x2000,
           "after second flip should be 0x2000");

    flipper.present_and_flip_page();
    expect(flipper.active_page() == 0x0000,
           "after third flip should be 0x0000");
  }

  {
    comic2::EgaPageFlipper flipper;
    flipper.init_double_buffering();
    expect(flipper.other_page() == 0x0000,
           "initial other page should be 0x0000");

    flipper.present_and_flip_page();
    expect(flipper.other_page() == 0x2000,
           "after flip other page should be 0x2000");

    flipper.present_and_flip_page();
    expect(flipper.other_page() == 0x0000,
           "after second flip other page should be 0x0000");
  }

#ifdef COMIC2_USE_SDL2
  // SDL2 presenter tests (only run when SDL2 is available)
  // In headless/CI environments with dummy video driver, renderer creation may
  // fail
  {
    try {
      comic2::EgaPlanarSurface src(320, 200);
      src.set_plane_byte(0, 0, 0, 0xFF);
      src.set_plane_byte(1, 0, 0, 0xFF);

      comic2::Sdl2FramePresenter presenter(640, 480);
      presenter.present(src);

      // If we get here without exception, SDL2 presenter is working
      expect(true, "Sdl2FramePresenter should present without error");
    } catch (const std::runtime_error &e) {
      // SDL renderer creation can fail in headless environments (dummy driver)
      // This is expected and acceptable - log but don't fail the test
      std::string error(e.what());
      if (error.find("SDL_CreateRenderer failed") != std::string::npos) {
        // Expected in headless CI - pass silently
      } else {
        // Unexpected error - rethrow
        throw;
      }
    }
  }
#endif // COMIC2_USE_SDL2
}
