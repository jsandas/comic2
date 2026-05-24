#include "comic2/renderer.hpp"

#include <algorithm>
#include <stdexcept>

namespace comic2 {
namespace {

std::size_t checked_offset(const EgaPlanarSurface& surface, std::size_t x_byte, std::size_t y_row) {
    if (x_byte >= surface.row_stride_bytes()) {
        throw std::out_of_range("x_byte out of range");
    }
    if (y_row >= surface.height_rows()) {
        throw std::out_of_range("y_row out of range");
    }
    return y_row * surface.row_stride_bytes() + x_byte;
}

}  // namespace

EgaPlanarSurface::EgaPlanarSurface(std::uint16_t width_pixels, std::uint16_t height_rows)
    : width_pixels_(width_pixels),
      height_rows_(height_rows),
      row_stride_bytes_(static_cast<std::size_t>(width_pixels + 7U) / 8U) {
    if (width_pixels == 0 || height_rows == 0) {
        throw std::invalid_argument("surface dimensions must be non-zero");
    }

    const auto plane_bytes = plane_size_bytes();
    for (auto& p : planes_) {
        p.assign(plane_bytes, 0);
    }
}

void EgaPlanarSurface::clear(std::uint8_t value) {
    for (auto& p : planes_) {
        std::fill(p.begin(), p.end(), value);
    }
}

std::span<std::uint8_t> EgaPlanarSurface::plane(std::size_t index) {
    if (index >= kPlaneCount) {
        throw std::out_of_range("plane index out of range");
    }
    return planes_[index];
}

std::span<const std::uint8_t> EgaPlanarSurface::plane(std::size_t index) const {
    if (index >= kPlaneCount) {
        throw std::out_of_range("plane index out of range");
    }
    return planes_[index];
}

std::uint8_t EgaPlanarSurface::get_plane_byte(std::size_t plane_index, std::size_t x_byte, std::size_t y_row) const {
    if (plane_index >= kPlaneCount) {
        throw std::out_of_range("plane index out of range");
    }
    const auto off = checked_offset(*this, x_byte, y_row);
    return planes_[plane_index][off];
}

void EgaPlanarSurface::set_plane_byte(std::size_t plane_index, std::size_t x_byte, std::size_t y_row, std::uint8_t value) {
    if (plane_index >= kPlaneCount) {
        throw std::out_of_range("plane index out of range");
    }
    const auto off = checked_offset(*this, x_byte, y_row);
    planes_[plane_index][off] = value;
}

void MemoryFramePresenter::present(const EgaPlanarSurface& frame) {
    last_frame_ = frame;
    has_frame_ = true;
}

const EgaPlanarSurface& MemoryFramePresenter::last_frame() const {
    if (!has_frame_) {
        throw std::logic_error("no frame has been presented");
    }
    return last_frame_;
}

}  // namespace comic2
