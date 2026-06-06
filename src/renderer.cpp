#include "comic2/renderer.hpp"

#include <algorithm>
#include <stdexcept>

#ifdef COMIC2_USE_SDL2
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL.h>
#else
#error "SDL2 headers not found"
#endif
#endif

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

// cppcheck-suppress functionConst
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
    
    // Validate image dimensions
    if (image_data.width_bytes == 0 || image_data.height_rows == 0) {
        throw std::invalid_argument("image width_bytes and height_rows must be non-zero for blitting");
    }
    
    // Calculate destination row stride in bytes
    const auto dest_row_stride = dest.row_stride_bytes();
    const auto dest_byte_offset = x_pixels / 8;
    
    // For each plane, copy row-by-row using explicit image dimensions
    for (std::size_t plane_index = 0; plane_index < 4; ++plane_index) {
        const auto& source_plane = image_data.planes[plane_index];
        auto dest_plane_span = dest.plane(plane_index);
        auto dest_plane = dest_plane_span.data();
        
        for (std::size_t row = 0; row < image_data.height_rows; ++row) {
            const auto source_offset = row * image_data.width_bytes;
            const auto dest_offset = (y_rows + row) * dest_row_stride + dest_byte_offset;
            
            std::copy(source_plane.begin() + static_cast<std::ptrdiff_t>(source_offset),
                      source_plane.begin() + static_cast<std::ptrdiff_t>(source_offset + image_data.width_bytes),
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
    
    // Validate image dimensions
    if (image_data.width_bytes == 0 || image_data.height_rows == 0) {
        throw std::invalid_argument("image width_bytes and height_rows must be non-zero for blitting");
    }
    
    // Calculate destination row stride in bytes
    const auto dest_row_stride = dest.row_stride_bytes();
    const auto dest_byte_offset = x_pixels / 8;
    
    // For each plane, OR row-by-row using explicit image dimensions
    for (std::size_t plane_index = 0; plane_index < 4; ++plane_index) {
        const auto& source_plane = image_data.planes[plane_index];
        auto dest_plane_span = dest.plane(plane_index);
        auto dest_plane = dest_plane_span.data();
        
        for (std::size_t row = 0; row < image_data.height_rows; ++row) {
            const auto source_offset = row * image_data.width_bytes;
            const auto dest_offset = (y_rows + row) * dest_row_stride + dest_byte_offset;
            
            for (std::size_t i = 0; i < image_data.width_bytes; ++i) {
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

#ifdef COMIC2_USE_SDL2
// ============================================================================
// SDL2 Frame Presenter Implementation
// ============================================================================

struct Sdl2FramePresenter::Impl {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
    SDL_PixelFormat* pixel_format{};
    std::array<Uint32, 16> palette{};
    std::uint16_t window_width{};
    std::uint16_t window_height{};

    Impl(std::uint16_t ww, std::uint16_t hh)
        : window_width(ww), window_height(hh) {}

    ~Impl() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        if (pixel_format) {
            SDL_FreeFormat(pixel_format);
        }
        SDL_Quit();
    }
};

Sdl2FramePresenter::Sdl2FramePresenter(std::uint16_t window_width, std::uint16_t window_height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
    }

    impl_ = new Impl(window_width, window_height);

    // Create window
    impl_->window = SDL_CreateWindow(
        "Captain Comic II: Fractured Reality (Reimplementation)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    if (!impl_->window) {
        delete impl_;
        impl_ = nullptr;
        throw std::runtime_error("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
    }

    // Create renderer - try accelerated first, fall back to software for headless/CI
    impl_->renderer = SDL_CreateRenderer(
        impl_->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!impl_->renderer) {
        // Acceleration not available (headless/dummy driver) - try software renderer
        impl_->renderer = SDL_CreateRenderer(
            impl_->window,
            -1,
            SDL_RENDERER_SOFTWARE
        );
    }

    if (!impl_->renderer) {
        delete impl_;
        impl_ = nullptr;
        throw std::runtime_error("SDL_CreateRenderer failed: " + std::string(SDL_GetError()));
    }

    // Create an explicit 32-bit RGBA format for display
    impl_->pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    if (!impl_->pixel_format) {
        delete impl_;
        impl_ = nullptr;
        throw std::runtime_error("SDL_AllocFormat failed: " + std::string(SDL_GetError()));
    }

    constexpr std::uint8_t kEgaPalette[16][4] = {
        {  0,   0,   0, 255 },  // Black
        {  0,   0, 200, 255 },  // Blue
        {  0, 200,   0, 255 },  // Green
        {  0, 200, 200, 255 },  // Cyan
        { 200,   0,   0, 255 },  // Red
        { 200,   0, 200, 255 },  // Magenta
        { 200, 100,   0, 255 },  // Brown
        { 200, 200, 200, 255 },  // Light gray
        { 100, 100, 100, 255 },  // Dark gray
        {  50,  50, 255, 255 },  // Light blue
        {  50, 255,  50, 255 },  // Light green
        {  50, 255, 255, 255 },  // Light cyan
        { 255,  50,  50, 255 },  // Light red
        { 255,  50, 255, 255 },  // Light magenta
        { 255, 255,  50, 255 },  // Yellow
        { 255, 255, 255, 255 },  // White
    };
    for (std::size_t i = 0; i < impl_->palette.size(); ++i) {
        impl_->palette[i] = SDL_MapRGBA(impl_->pixel_format,
                                       kEgaPalette[i][0],
                                       kEgaPalette[i][1],
                                       kEgaPalette[i][2],
                                       kEgaPalette[i][3]);
    }

    impl_->texture = SDL_CreateTexture(
        impl_->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        320,  // EGA resolution width
        200   // EGA resolution height
    );

    if (!impl_->texture) {
        delete impl_;
        impl_ = nullptr;
        throw std::runtime_error("SDL_CreateTexture failed: " + std::string(SDL_GetError()));
    }
}

Sdl2FramePresenter::~Sdl2FramePresenter() {
    delete impl_;
}

void Sdl2FramePresenter::present(const EgaPlanarSurface& frame) {
    if (!impl_ || !impl_->renderer || !impl_->texture) {
        return;
    }

    // Convert 4-plane EGA data to packed 32-bit pixels
    // EGA uses 4 planes, each pixel is represented by 4 bits (one bit per plane)
    if (frame.width_pixels() != 320 || frame.height_rows() != 200) {
        throw std::invalid_argument("Sdl2FramePresenter expects a 320x200 EGA frame");
    }

    void* pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(impl_->texture, nullptr, &pixels, &pitch) != 0) {
        return;
    }

    if (pitch % sizeof(Uint32) != 0) {
        SDL_UnlockTexture(impl_->texture);
        throw std::runtime_error("SDL texture pitch is not a multiple of Uint32");
    }

    auto* dst = static_cast<Uint32*>(pixels);
    const auto row_pixels = static_cast<std::size_t>(pitch / sizeof(Uint32));
    const auto row_stride = frame.row_stride_bytes();
    const auto* plane0 = frame.plane(0).data();
    const auto* plane1 = frame.plane(1).data();
    const auto* plane2 = frame.plane(2).data();
    const auto* plane3 = frame.plane(3).data();

    for (std::size_t y = 0; y < 200; ++y) {
        for (std::size_t x = 0; x < 320; ++x) {
            const auto byte_x = x / 8;
            const auto bit_x = 7 - (x % 8);
            const auto off = y * row_stride + byte_x;

            std::uint8_t color_index = 0;
            if (plane0[off] & (1u << bit_x)) {
                color_index |= 0x1;
            }
            if (plane1[off] & (1u << bit_x)) {
                color_index |= 0x2;
            }
            if (plane2[off] & (1u << bit_x)) {
                color_index |= 0x4;
            }
            if (plane3[off] & (1u << bit_x)) {
                color_index |= 0x8;
            }

            dst[y * row_pixels + x] = impl_->palette[color_index];
        }
    }

    // Unlock texture and update
    SDL_UnlockTexture(impl_->texture);

    // Copy texture to renderer
    SDL_RenderClear(impl_->renderer);
    SDL_RenderCopy(impl_->renderer, impl_->texture, nullptr, nullptr);
    SDL_RenderPresent(impl_->renderer);
}
#endif  // COMIC2_USE_SDL2

}  // namespace comic2
