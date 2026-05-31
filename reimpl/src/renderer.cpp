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

// Helper function to decode RLE data and copy to a single plane
void decode_rle_copy_rowspan(std::span<std::uint8_t> dest_plane, std::size_t dest_offset, 
                           std::span<const std::uint8_t> source_data, std::size_t& source_offset) {
    while (source_offset < source_data.size()) {
        const std::uint8_t packet = source_data[source_offset++];
        if ((packet & 0x80U) == 0) {
            // Literal packet
            const auto count = static_cast<std::size_t>(packet);
            if (source_offset + count > source_data.size()) {
                throw std::runtime_error("RLE literal packet truncated");
            }
            std::copy(source_data.begin() + static_cast<std::ptrdiff_t>(source_offset),
                       source_data.begin() + static_cast<std::ptrdiff_t>(source_offset + count),
                       dest_plane.begin() + static_cast<std::ptrdiff_t>(dest_offset));
            source_offset += count;
            dest_offset += count;
        } else {
            // Run packet
            const auto count = static_cast<std::size_t>(packet & 0x7FU);
            if (source_offset >= source_data.size()) {
                throw std::runtime_error("RLE run packet truncated");
            }
            const auto value = source_data[source_offset++];
            std::fill(dest_plane.begin() + static_cast<std::ptrdiff_t>(dest_offset),
                       dest_plane.begin() + static_cast<std::ptrdiff_t>(dest_offset + count),
                       value);
            dest_offset += count;
        }
    }
}

// Helper function to decode RLE data and OR with a single plane
void decode_rle_or_rowspan(std::span<std::uint8_t> dest_plane, std::size_t dest_offset, 
                           std::span<const std::uint8_t> source_data, std::size_t& source_offset) {
    while (source_offset < source_data.size()) {
        const std::uint8_t packet = source_data[source_offset++];
        if ((packet & 0x80U) == 0) {
            // Literal packet - OR with destination
            const auto count = static_cast<std::size_t>(packet);
            if (source_offset + count > source_data.size()) {
                throw std::runtime_error("RLE literal packet truncated");
            }
            for (std::size_t i = 0; i < count; ++i) {
                const auto value = source_data[source_offset++];
                dest_plane[dest_offset + i] |= value;
            }
            dest_offset += count;
        } else {
            // Run packet - OR with destination
            const auto count = static_cast<std::size_t>(packet & 0x7FU);
            if (source_offset >= source_data.size()) {
                throw std::runtime_error("RLE run packet truncated");
            }
            const auto value = source_data[source_offset++];
            for (std::size_t i = 0; i < count; ++i) {
                dest_plane[dest_offset + i] |= value;
            }
            dest_offset += count;
        }
    }
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

void EgaPageFlipper::init_double_buffering() {
    // Initialize double buffering with page 0x2000 as the active page
    // This matches the original assembly code behavior
    active_page_ = 0x2000;
}

void EgaPageFlipper::present_and_flip_page() {
    // Flip between pages by XORing with 0x2000
    // This matches the original assembly code behavior:
    // xor cs:word_773F, 2000h
    active_page_ ^= 0x2000;
}

void gfx_rle_blit_opaque_4plane(EgaPlanarSurface& dest, std::size_t x_pixels, std::size_t y_rows, 
                               const Ega4PlaneImage& image_data) {
    // Validate destination coordinates
    if (x_pixels >= dest.width_pixels() || y_rows >= dest.height_rows()) {
        throw std::out_of_range("blit coordinates out of range");
    }
    
    // Validate image data
    if (image_data.row_span_bytes == 0) {
        throw std::invalid_argument("image row span cannot be zero");
    }
    
    // Calculate destination row stride in bytes
    const auto dest_row_stride = dest.row_stride_bytes();
    
    // For each plane, decode and copy the RLE data
    for (std::size_t plane_index = 0; plane_index < 4; ++plane_index) {
        const auto& source_plane = image_data.planes[plane_index];
        auto dest_plane_span = dest.plane(plane_index);
        auto dest_plane = dest_plane_span.data();
        
        // For each row in the image
        for (std::size_t row = 0; row < image_data.planes[0].size() / image_data.row_span_bytes; ++row) {
            // Calculate destination offset for this row
            const auto dest_row_offset = (y_rows + row) * dest_row_stride;
            const auto dest_byte_offset = x_pixels / 8;
            
            // Copy the row data to the destination plane
            std::size_t source_offset = row * image_data.row_span_bytes;
            std::size_t dest_offset = dest_row_offset + dest_byte_offset;
            
            // Copy the data
            std::copy(source_plane.begin() + static_cast<std::ptrdiff_t>(source_offset),
                      source_plane.begin() + static_cast<std::ptrdiff_t>(source_offset + image_data.row_span_bytes),
                      dest_plane + static_cast<std::ptrdiff_t>(dest_offset));
        }
    }
}

void gfx_rle_blit_masked_or_4plane(EgaPlanarSurface& dest, std::size_t x_pixels, std::size_t y_rows, 
                                   const Ega4PlaneImage& image_data) {
    // Validate destination coordinates
    if (x_pixels >= dest.width_pixels() || y_rows >= dest.height_rows()) {
        throw std::out_of_range("blit coordinates out of range");
    }
    
    // Validate image data
    if (image_data.row_span_bytes == 0) {
        throw std::invalid_argument("image row span cannot be zero");
    }
    
    // Calculate destination row stride in bytes
    const auto dest_row_stride = dest.row_stride_bytes();
    
    // For each plane, decode and OR the RLE data
    for (std::size_t plane_index = 0; plane_index < 4; ++plane_index) {
        const auto& source_plane = image_data.planes[plane_index];
        auto dest_plane_span = dest.plane(plane_index);
        auto dest_plane = dest_plane_span.data();
        
        // For each row in the image
        for (std::size_t row = 0; row < image_data.planes[0].size() / image_data.row_span_bytes; ++row) {
            // Calculate destination offset for this row
            const auto dest_row_offset = (y_rows + row) * dest_row_stride;
            const auto dest_byte_offset = x_pixels / 8;
            
            // OR the row data with the destination plane
            std::size_t source_offset = row * image_data.row_span_bytes;
            std::size_t dest_offset = dest_row_offset + dest_byte_offset;
            
            // OR the data
            for (std::size_t i = 0; i < image_data.row_span_bytes; ++i) {
                dest_plane[dest_offset + i] |= source_plane[source_offset + i];
            }
        }
    }
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
