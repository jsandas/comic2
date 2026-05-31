#include "comic2/renderer.hpp"
#include "comic2/resource_loader.hpp"

#include <gtest/gtest.h>

namespace comic2 {
namespace {

TEST(RendererTest, OpaqueBlitTest) {
    // Create a test surface
    EgaPlanarSurface surface(320, 200);
    
    // Create test image data with simple pattern
    Ega4PlaneImage image;
    image.row_span_bytes = 40; // 320 pixels / 8 = 40 bytes per row
    
    // Initialize 4 planes with simple test data
    for (int plane = 0; plane < 4; ++plane) {
        image.planes[plane].resize(200 * 40); // 200 rows
        for (std::size_t i = 0; i < image.planes[plane].size(); ++i) {
            image.planes[plane][i] = static_cast<std::uint8_t>(plane + i % 256);
        }
    }
    
    // Test blitting to a valid position
    gfx_rle_blit_opaque_4plane(surface, 0, 0, image);
    
    // Verify that data was copied correctly
    for (int plane = 0; plane < 4; ++plane) {
        for (std::size_t i = 0; i < image.planes[plane].size(); ++i) {
            EXPECT_EQ(surface.plane(plane)[i], image.planes[plane][i]);
        }
    }
}

TEST(RendererTest, MaskedBlitTest) {
    // Create a test surface
    EgaPlanarSurface surface(320, 200);
    
    // Initialize with some data
    surface.clear(0x55);
    
    // Create test image data with simple pattern
    Ega4PlaneImage image;
    image.row_span_bytes = 40; // 320 pixels / 8 = 40 bytes per row
    
    // Initialize 4 planes with simple test data
    for (int plane = 0; plane < 4; ++plane) {
        image.planes[plane].resize(200 * 40); // 200 rows
        for (std::size_t i = 0; i < image.planes[plane].size(); ++i) {
            image.planes[plane][i] = static_cast<std::uint8_t>(plane + i % 256);
        }
    }
    
    // Test blitting to a valid position
    gfx_rle_blit_masked_or_4plane(surface, 0, 0, image);
    
    // Verify that data was OR'd correctly
    for (int plane = 0; plane < 4; ++plane) {
        for (std::size_t i = 0; i < image.planes[plane].size(); ++i) {
            // The result should be OR'd with the original data
            EXPECT_EQ(surface.plane(plane)[i], 0x55 | image.planes[plane][i]);
        }
    }
}

} // namespace
} // namespace comic2