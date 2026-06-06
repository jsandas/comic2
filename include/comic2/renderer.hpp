#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "comic2/resource_formats.hpp"

namespace comic2 {

class EgaPlanarSurface {
public:
    static constexpr std::size_t kPlaneCount = 4;
    static constexpr std::uint16_t kPage0 = 0x0000;
    static constexpr std::uint16_t kPage1 = 0x2000;

    explicit EgaPlanarSurface(std::uint16_t width_pixels = 320, std::uint16_t height_rows = 200);

    std::uint16_t width_pixels() const noexcept { return width_pixels_; }
    std::uint16_t height_rows() const noexcept { return height_rows_; }
    std::size_t row_stride_bytes() const noexcept { return row_stride_bytes_; }
    std::size_t plane_size_bytes() const noexcept { return row_stride_bytes_ * static_cast<std::size_t>(height_rows_); }

    void clear(std::uint8_t value = 0);

    std::span<std::uint8_t> plane(std::size_t index);
    std::span<const std::uint8_t> plane(std::size_t index) const;

    std::uint8_t get_plane_byte(std::size_t plane_index, std::size_t x_byte, std::size_t y_row) const;
    void set_plane_byte(std::size_t plane_index, std::size_t x_byte, std::size_t y_row, std::uint8_t value);

private:
    std::uint16_t width_pixels_;
    std::uint16_t height_rows_;
    std::size_t row_stride_bytes_;
    std::array<std::vector<std::uint8_t>, kPlaneCount> planes_;
};

class IFramePresenter {
public:
    virtual ~IFramePresenter() = default;
    virtual void present(const EgaPlanarSurface& frame) = 0;
};

class MemoryFramePresenter final : public IFramePresenter {
public:
    void present(const EgaPlanarSurface& frame) override;

    bool has_frame() const noexcept { return has_frame_; }
    const EgaPlanarSurface& last_frame() const;

private:
    bool has_frame_ = false;
    EgaPlanarSurface last_frame_{};
};

#ifdef COMIC2_USE_SDL2
class Sdl2FramePresenter final : public IFramePresenter {
public:
    explicit Sdl2FramePresenter(std::uint16_t window_width = 640, std::uint16_t window_height = 480);
    ~Sdl2FramePresenter() override;

    void present(const EgaPlanarSurface& frame) override;

    // Disable copying
    Sdl2FramePresenter(const Sdl2FramePresenter&) = delete;
    Sdl2FramePresenter& operator=(const Sdl2FramePresenter&) = delete;

private:
    struct Impl;
    Impl* impl_{};
};
#endif  // COMIC2_USE_SDL2

// Page flipping abstraction for EGA double buffering
class EgaPageFlipper {
public:
    // Initialize double buffering with page 0x2000 as the active page
    void init_double_buffering();
    
    // Present current draw page and flip to the other page
    void present_and_flip_page();
    
    // Get the current active page address
    std::uint16_t active_page() const noexcept { return active_page_; }
    
    // Get the other page address
    std::uint16_t other_page() const noexcept { return active_page_ ^ 0x2000; }

private:
    std::uint16_t active_page_ = 0;
};

// Blit functions for EGA 4-plane sprites
//
// These functions implement the core blitting operations that mirror the
// original Captain Comic II assembly implementations:
// - gfx_rle_blit_opaque_4plane: Copies RLE-encoded 4-plane sprite data
// - gfx_rle_blit_masked_or_4plane: ORs RLE-encoded 4-plane sprite data (masked blitting)
//
// Parameters:
//   dest: The destination surface to blit to
//   x_pixels: X coordinate in pixels where to start blitting
//   y_rows: Y coordinate in rows where to start blitting
//   image_data: The RLE-encoded 4-plane sprite data to blit
void gfx_rle_blit_opaque_4plane(EgaPlanarSurface& dest, std::size_t x_pixels, std::size_t y_rows, 
                               const Ega4PlaneImage& image_data);
void gfx_rle_blit_masked_or_4plane(EgaPlanarSurface& dest, std::size_t x_pixels, std::size_t y_rows, 
                                      const Ega4PlaneImage& image_data);

}  // namespace comic2
