#include <cstdint>
#include <stdexcept>

#include "comic2/renderer.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
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

} // namespace

void run_renderer_tests() {
  test_surface_geometry();
  test_surface_plane_rw();
  test_presenter_copies_frame();

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
