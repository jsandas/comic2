#include "comic2/renderer_validation.hpp"

#include <numeric>
#include <sstream>

namespace comic2 {
namespace validation {
namespace {

std::string format_mismatch(std::size_t plane_index, std::size_t byte_offset,
                            std::uint8_t actual, std::uint8_t expected) {
    std::ostringstream oss;
    oss << "Plane " << plane_index << " byte[" << byte_offset
        << "]: expected 0x" << std::hex << static_cast<int>(expected)
        << ", got 0x" << static_cast<int>(actual);
    return oss.str();
}

}  // namespace

std::string compare_plane_bytes(std::span<const std::uint8_t> actual,
                                const std::vector<std::uint8_t>& expected,
                                std::size_t plane_index) {
    if (actual.size() != expected.size()) {
        std::ostringstream oss;
        oss << "Plane " << plane_index << " size mismatch: expected "
            << expected.size() << ", got " << actual.size();
        return oss.str();
    }
    for (std::size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != expected[i]) {
            return format_mismatch(plane_index, i, actual[i], expected[i]);
        }
    }
    return "";
}

std::string compare_surface_planes(const EgaPlanarSurface& surface,
                                   const std::vector<std::vector<std::uint8_t>>& expected_planes) {
    if (expected_planes.size() != EgaPlanarSurface::kPlaneCount) {
        return "expected_planes size mismatch: expected 4 planes";
    }
    for (std::size_t p = 0; p < EgaPlanarSurface::kPlaneCount; ++p) {
        auto result = compare_plane_bytes(surface.plane(p), expected_planes[p], p);
        if (!result.empty()) {
            return result;
        }
    }
    return "";
}

std::string compare_plane_region(const EgaPlanarSurface& surface,
                                 std::size_t plane_index,
                                 std::size_t x_byte, std::size_t y_row,
                                 std::size_t region_width_bytes, std::size_t region_height_rows,
                                 const std::vector<std::uint8_t>& expected) {
    const auto row_stride = surface.row_stride_bytes();
    const auto expected_size = region_width_bytes * region_height_rows;
    if (expected.size() != expected_size) {
        std::ostringstream oss;
        oss << "Region expected size mismatch: expected " << expected_size
            << ", got " << expected.size();
        return oss.str();
    }

    auto plane = surface.plane(plane_index);
    std::size_t expected_idx = 0;
    for (std::size_t row = 0; row < region_height_rows; ++row) {
        const auto offset = (y_row + row) * row_stride + x_byte;
        for (std::size_t col = 0; col < region_width_bytes; ++col) {
            const auto actual = plane[offset + col];
            const auto exp = expected[expected_idx++];
            if (actual != exp) {
                const auto global_offset = offset + col;
                return format_mismatch(plane_index, global_offset, actual, exp);
            }
        }
    }
    return "";
}

std::uint64_t hash_surface_planes(const EgaPlanarSurface& surface) {
    std::uint64_t hash = 0xcbf29ce484222325;  // FNV-1a offset basis
    for (std::size_t p = 0; p < EgaPlanarSurface::kPlaneCount; ++p) {
        auto plane = surface.plane(p);
        for (const auto byte : plane) {
            hash ^= static_cast<std::uint64_t>(byte);
            hash *= 0x100000001b3;  // FNV-1a prime
        }
    }
    return hash;
}

std::uint64_t hash_plane_region(const EgaPlanarSurface& surface,
                                std::size_t plane_index,
                                std::size_t x_byte, std::size_t y_row,
                                std::size_t region_width_bytes, std::size_t region_height_rows) {
    std::uint64_t hash = 0xcbf29ce484222325;  // FNV-1a offset basis
    const auto row_stride = surface.row_stride_bytes();
    auto plane = surface.plane(plane_index);
    for (std::size_t row = 0; row < region_height_rows; ++row) {
        const auto offset = (y_row + row) * row_stride + x_byte;
        for (std::size_t col = 0; col < region_width_bytes; ++col) {
            hash ^= static_cast<std::uint64_t>(plane[offset + col]);
            hash *= 0x100000001b3;
        }
    }
    return hash;
}

// --- Fixture factories ---

Ega4PlaneImage make_8x8_solid_image(std::uint8_t plane0_byte, std::uint8_t plane1_byte,
                                    std::uint8_t plane2_byte, std::uint8_t plane3_byte) {
    Ega4PlaneImage img;
    img.row_span_bytes = 1;  // 8 pixels = 1 byte per row
    // 8 rows, 1 byte per row = 1 byte per plane
    img.planes[0].assign(8, plane0_byte);
    img.planes[1].assign(8, plane1_byte);
    img.planes[2].assign(8, plane2_byte);
    img.planes[3].assign(8, plane3_byte);
    return img;
}

Ega4PlaneImage make_16x16_patterned_image() {
    Ega4PlaneImage img;
    img.row_span_bytes = 2;  // 16 pixels = 2 bytes per row
    // 16 rows, 2 bytes per row = 32 bytes per plane
    for (std::size_t p = 0; p < 4; ++p) {
        img.planes[p].resize(32);
        for (std::size_t i = 0; i < 32; ++i) {
            img.planes[p][i] = static_cast<std::uint8_t>((p + 1) * (i + 1) & 0xFF);
        }
    }
    return img;
}

// Create a known RLE-encoded 4-plane stream for decode validation.
// Row span = 2 bytes.
// Plane 0: literal packet: 0x02 0x10 0x20 (copy 2 bytes: 0x10, 0x20)
// Plane 1: run packet: 0x82 0x30 (run 2 of 0x30)
// Plane 2: literal packet: 0x02 0x40 0x50
// Plane 3: run packet: 0x82 0x60
std::vector<std::uint8_t> make_known_rle_4plane_stream() {
    // Header: row_span = 2 (little-endian)
    // Then 4 planes of RLE data
    return {
        0x02, 0x00,           // row_span = 2
        0x02, 0x10, 0x20,     // plane 0: literal 2 bytes
        0x82, 0x30,           // plane 1: run 2 of 0x30
        0x02, 0x40, 0x50,     // plane 2: literal 2 bytes
        0x82, 0x60,           // plane 3: run 2 of 0x60
    };
}

std::vector<std::vector<std::uint8_t>> make_expected_planes_for_known_rle() {
    return {
        {0x10, 0x20},  // plane 0
        {0x30, 0x30},  // plane 1
        {0x40, 0x50},  // plane 2
        {0x60, 0x60},  // plane 3
    };
}

// Run-only stream: row_span = 4 bytes
// Each plane is a single run packet
std::vector<std::uint8_t> make_run_only_rle_4plane_stream() {
    return {
        0x04, 0x00,           // row_span = 4
        0x84, 0x11,           // plane 0: run 4 of 0x11
        0x84, 0x22,           // plane 1: run 4 of 0x22
        0x84, 0x33,           // plane 2: run 4 of 0x33
        0x84, 0x44,           // plane 3: run 4 of 0x44
    };
}

std::vector<std::vector<std::uint8_t>> make_expected_planes_for_run_only_rle() {
    return {
        {0x11, 0x11, 0x11, 0x11},
        {0x22, 0x22, 0x22, 0x22},
        {0x33, 0x33, 0x33, 0x33},
        {0x44, 0x44, 0x44, 0x44},
    };
}

// Mixed stream: row_span = 4 bytes
// Plane 0: literal 2 bytes + run 2
// Plane 1: run 2 + literal 2 bytes
// Plane 2: literal 4 bytes
// Plane 3: run 4
std::vector<std::uint8_t> make_mixed_rle_4plane_stream() {
    return {
        0x04, 0x00,           // row_span = 4
        0x02, 0xAA, 0xBB,     // plane 0: literal 2 bytes
        0x82, 0xCC,           // plane 0: run 2 of 0xCC
        0x82, 0xDD,           // plane 1: run 2 of 0xDD
        0x02, 0xEE, 0xFF,     // plane 1: literal 2 bytes
        0x04, 0x01, 0x02, 0x03, 0x04,  // plane 2: literal 4 bytes
        0x84, 0x55,           // plane 3: run 4 of 0x55
    };
}

std::vector<std::vector<std::uint8_t>> make_expected_planes_for_mixed_rle() {
    return {
        {0xAA, 0xBB, 0xCC, 0xCC},  // plane 0
        {0xDD, 0xDD, 0xEE, 0xFF},  // plane 1
        {0x01, 0x02, 0x03, 0x04},  // plane 2
        {0x55, 0x55, 0x55, 0x55},  // plane 3
    };
}

// --- Blit fixture validators ---

std::string validate_opaque_blit(const EgaPlanarSurface& dest,
                                 const Ega4PlaneImage& image,
                                 std::size_t x_pixels, std::size_t y_rows,
                                 const std::vector<std::vector<std::uint8_t>>& expected_planes) {
    if (expected_planes.size() != EgaPlanarSurface::kPlaneCount) {
        return "expected_planes size mismatch: expected 4 planes";
    }

    const auto x_byte = x_pixels / 8;
    const auto row_count = image.planes[0].size() / image.row_span_bytes;

    for (std::size_t p = 0; p < EgaPlanarSurface::kPlaneCount; ++p) {
        auto result = compare_plane_region(dest, p, x_byte, y_rows,
                                           image.row_span_bytes, row_count,
                                           expected_planes[p]);
        if (!result.empty()) {
            return "Plane " + std::to_string(p) + " blit mismatch: " + result;
        }
    }
    return "";
}

std::string validate_masked_blit(const EgaPlanarSurface& dest,
                                 const Ega4PlaneImage& image,
                                 std::size_t x_pixels, std::size_t y_rows,
                                 const std::vector<std::vector<std::uint8_t>>& expected_planes) {
    if (expected_planes.size() != EgaPlanarSurface::kPlaneCount) {
        return "expected_planes size mismatch: expected 4 planes";
    }

    const auto x_byte = x_pixels / 8;
    const auto row_count = image.planes[0].size() / image.row_span_bytes;

    for (std::size_t p = 0; p < EgaPlanarSurface::kPlaneCount; ++p) {
        auto result = compare_plane_region(dest, p, x_byte, y_rows,
                                           image.row_span_bytes, row_count,
                                           expected_planes[p]);
        if (!result.empty()) {
            return "Plane " + std::to_string(p) + " masked blit mismatch: " + result;
        }
    }
    return "";
}

}  // namespace validation
}  // namespace comic2
