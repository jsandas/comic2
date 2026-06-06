#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "comic2/renderer.hpp"

namespace comic2 {
namespace validation {

// Compare a single plane's bytes against an expected vector.
// Returns an empty string on success, or a descriptive mismatch message.
std::string compare_plane_bytes(std::span<const std::uint8_t> actual,
                                const std::vector<std::uint8_t> &expected,
                                std::size_t plane_index);

// Compare all 4 planes of a surface against expected plane vectors.
// Returns an empty string on success, or the first mismatch message.
std::string compare_surface_planes(
    const EgaPlanarSurface &surface,
    const std::vector<std::vector<std::uint8_t>> &expected_planes);

// Compare a rectangular region of a single plane against expected bytes.
// region_width is in bytes (not pixels).
std::string compare_plane_region(const EgaPlanarSurface &surface,
                                 std::size_t plane_index, std::size_t x_byte,
                                 std::size_t y_row,
                                 std::size_t region_width_bytes,
                                 std::size_t region_height_rows,
                                 const std::vector<std::uint8_t> &expected);

// Compute a simple hash over all 4 planes for frame-level comparison.
// Useful for oracle-style visual verification (Gate D in PLAN.md).
std::uint64_t hash_surface_planes(const EgaPlanarSurface &surface);

// Compute a hash over a rectangular region of a single plane.
std::uint64_t hash_plane_region(const EgaPlanarSurface &surface,
                                std::size_t plane_index, std::size_t x_byte,
                                std::size_t y_row,
                                std::size_t region_width_bytes,
                                std::size_t region_height_rows);

// --- Fixture factories ---

// Create a small 8x8 Ega4PlaneImage with known byte patterns for testing.
// Each plane gets a distinct fill byte pattern.
Ega4PlaneImage make_8x8_solid_image(std::uint8_t plane0_byte,
                                    std::uint8_t plane1_byte,
                                    std::uint8_t plane2_byte,
                                    std::uint8_t plane3_byte);

// Create a 16x16 image where each plane has a known checkerboard-like pattern.
Ega4PlaneImage make_16x16_patterned_image();

// Create a known RLE-encoded 4-plane stream for decode validation.
// Row span = 2 bytes, each plane has a different encode pattern.
std::vector<std::uint8_t> make_known_rle_4plane_stream();

// Expected decoded planes for make_known_rle_4plane_stream().
std::vector<std::vector<std::uint8_t>> make_expected_planes_for_known_rle();

// Create a known RLE-encoded 4-plane stream with run packets only.
std::vector<std::uint8_t> make_run_only_rle_4plane_stream();

// Expected decoded planes for make_run_only_rle_4plane_stream().
std::vector<std::vector<std::uint8_t>> make_expected_planes_for_run_only_rle();

// Create a known RLE-encoded 4-plane stream with mixed literal and run packets.
std::vector<std::uint8_t> make_mixed_rle_4plane_stream();

// Expected decoded planes for make_mixed_rle_4plane_stream().
std::vector<std::vector<std::uint8_t>> make_expected_planes_for_mixed_rle();

// --- Blit fixture validators ---

// Validate that an opaque blit of the given image at (x, y) produces
// the expected bytes in the destination surface.
std::string validate_opaque_blit(
    const EgaPlanarSurface &dest, const Ega4PlaneImage &image,
    std::size_t x_pixels, std::size_t y_rows,
    const std::vector<std::vector<std::uint8_t>> &expected_planes);

// Validate that a masked (OR) blit of the given image at (x, y) produces
// the expected bytes in the destination surface.
std::string validate_masked_blit(
    const EgaPlanarSurface &dest, const Ega4PlaneImage &image,
    std::size_t x_pixels, std::size_t y_rows,
    const std::vector<std::vector<std::uint8_t>> &expected_planes);

} // namespace validation
} // namespace comic2
