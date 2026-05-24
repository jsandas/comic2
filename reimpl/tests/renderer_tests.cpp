#include <cstdint>
#include <stdexcept>

#include "comic2/renderer.hpp"

namespace {

void expect(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_surface_geometry() {
    comic2::EgaPlanarSurface surface(320, 200);
    expect(surface.row_stride_bytes() == 40, "row stride mismatch for 320px surface");
    expect(surface.plane_size_bytes() == 8000, "plane size mismatch for 320x200 surface");
}

void test_surface_plane_rw() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.set_plane_byte(2, 10, 5, 0xA5);
    expect(surface.get_plane_byte(2, 10, 5) == 0xA5, "plane byte write/read mismatch");
}

void test_presenter_copies_frame() {
    comic2::EgaPlanarSurface src(320, 200);
    src.set_plane_byte(1, 0, 0, 0x7E);

    comic2::MemoryFramePresenter presenter;
    presenter.present(src);

    src.set_plane_byte(1, 0, 0, 0x00);
    expect(presenter.has_frame(), "presenter should report frame availability");
    expect(presenter.last_frame().get_plane_byte(1, 0, 0) == 0x7E, "presenter did not retain frame copy");
}

}  // namespace

void run_renderer_tests() {
    test_surface_geometry();
    test_surface_plane_rw();
    test_presenter_copies_frame();
}
