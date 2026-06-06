#include <cstdint>
#include <stdexcept>
#include <vector>

#include "comic2/renderer.hpp"
#include "comic2/renderer_validation.hpp"
#include "comic2/resource_formats.hpp"

namespace {

void expect(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void expect_empty(const std::string& result, const char* context) {
    if (!result.empty()) {
        std::runtime_error err(std::string(context) + ": " + result);
        throw err;
    }
}

// ============================================================================
// Decode fixture tests
// ============================================================================

void test_decode_known_rle_4plane_stream() {
    auto stream = comic2::validation::make_known_rle_4plane_stream();
    auto expected = comic2::validation::make_expected_planes_for_known_rle();

    auto image = comic2::decode_ega_4plane_rle(stream, true);
    expect(image.row_span_bytes == 2, "row_span should be 2");

    for (std::size_t p = 0; p < 4; ++p) {
        auto result = comic2::validation::compare_plane_bytes(image.planes[p], expected[p], p);
        expect_empty(result, "decode_known_rle plane mismatch");
    }
}

void test_decode_run_only_rle_4plane_stream() {
    auto stream = comic2::validation::make_run_only_rle_4plane_stream();
    auto expected = comic2::validation::make_expected_planes_for_run_only_rle();

    auto image = comic2::decode_ega_4plane_rle(stream, true);
    expect(image.row_span_bytes == 4, "row_span should be 4");

    for (std::size_t p = 0; p < 4; ++p) {
        auto result = comic2::validation::compare_plane_bytes(image.planes[p], expected[p], p);
        expect_empty(result, "decode_run_only_rle plane mismatch");
    }
}

void test_decode_mixed_rle_4plane_stream() {
    auto stream = comic2::validation::make_mixed_rle_4plane_stream();
    auto expected = comic2::validation::make_expected_planes_for_mixed_rle();

    auto image = comic2::decode_ega_4plane_rle(stream, true);
    expect(image.row_span_bytes == 4, "row_span should be 4");

    for (std::size_t p = 0; p < 4; ++p) {
        auto result = comic2::validation::compare_plane_bytes(image.planes[p], expected[p], p);
        expect_empty(result, "decode_mixed_rle plane mismatch");
    }
}

// ============================================================================
// Opaque blit fixture tests
// ============================================================================

void test_opaque_blit_solid_image_to_empty_surface() {
    // Blit an 8x8 solid image to an empty surface at (0, 0)
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);

    auto image = comic2::validation::make_8x8_solid_image(0x01, 0x02, 0x04, 0x08);
    comic2::gfx_rle_blit_opaque_4plane(surface, 0, 0, image);

    // Expected: first byte of each plane, rows 0-7 should have the solid values
    std::vector<std::vector<std::uint8_t>> expected_planes(4);
    for (std::size_t p = 0; p < 4; ++p) {
        std::uint8_t val = static_cast<std::uint8_t>(1 << p);
        expected_planes[p].assign(8, val);
    }

    auto result = comic2::validation::validate_opaque_blit(surface, image, 0, 0, expected_planes);
    expect_empty(result, "opaque_blit_solid_to_empty");
}

void test_opaque_blit_at_offset() {
    // Blit an 8x8 solid image at pixel (16, 10) -> byte offset (2, 10)
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);

    auto image = comic2::validation::make_8x8_solid_image(0x01, 0x02, 0x04, 0x08);
    comic2::gfx_rle_blit_opaque_4plane(surface, 16, 10, image);

    // Expected: byte offset 2, rows 10-17 should have the solid values
    std::vector<std::vector<std::uint8_t>> expected_planes(4);
    for (std::size_t p = 0; p < 4; ++p) {
        std::uint8_t val = static_cast<std::uint8_t>(1 << p);
        expected_planes[p].assign(8, val);
    }

    auto result = comic2::validation::validate_opaque_blit(surface, image, 16, 10, expected_planes);
    expect_empty(result, "opaque_blit_at_offset");

    // Verify surrounding bytes are still zero
    expect(surface.get_plane_byte(0, 1, 10) == 0, "byte before blit region should be 0");
    expect(surface.get_plane_byte(0, 1, 17) == 0, "byte after blit region should be 0");
}

void test_opaque_blit_overwrites_existing_data() {
    // Pre-fill surface with 0xFF, then blit 0x00 image
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0xFF);

    auto image = comic2::validation::make_8x8_solid_image(0x00, 0x00, 0x00, 0x00);
    comic2::gfx_rle_blit_opaque_4plane(surface, 0, 0, image);

    // The blit region should be 0x00 (overwritten)
    std::vector<std::vector<std::uint8_t>> expected_planes(4);
    for (std::size_t p = 0; p < 4; ++p) {
        expected_planes[p].assign(8, 0x00);
    }

    auto result = comic2::validation::validate_opaque_blit(surface, image, 0, 0, expected_planes);
    expect_empty(result, "opaque_blit_overwrites");

    // Outside the blit region should still be 0xFF
    expect(surface.get_plane_byte(0, 0, 8) == 0xFF, "outside blit region should be unchanged");
}

// ============================================================================
// Masked (OR) blit fixture tests
// ============================================================================

void test_masked_blit_or_with_empty_surface() {
    // OR blit to empty surface should behave like opaque blit
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);

    auto image = comic2::validation::make_8x8_solid_image(0x01, 0x02, 0x04, 0x08);
    comic2::gfx_rle_blit_masked_or_4plane(surface, 0, 0, image);

    std::vector<std::vector<std::uint8_t>> expected_planes(4);
    for (std::size_t p = 0; p < 4; ++p) {
        std::uint8_t val = static_cast<std::uint8_t>(1 << p);
        expected_planes[p].assign(8, val);
    }

    auto result = comic2::validation::validate_masked_blit(surface, image, 0, 0, expected_planes);
    expect_empty(result, "masked_blit_or_empty");
}

void test_masked_blit_or_with_existing_data() {
    // Pre-fill with 0xF0, OR with 0x0F -> should get 0xFF
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0xF0);

    auto image = comic2::validation::make_8x8_solid_image(0x0F, 0x0F, 0x0F, 0x0F);
    comic2::gfx_rle_blit_masked_or_4plane(surface, 0, 0, image);

    std::vector<std::vector<std::uint8_t>> expected_planes(4);
    for (std::size_t p = 0; p < 4; ++p) {
        expected_planes[p].assign(8, 0xFF);  // 0xF0 | 0x0F = 0xFF
    }

    auto result = comic2::validation::validate_masked_blit(surface, image, 0, 0, expected_planes);
    expect_empty(result, "masked_blit_or_existing");

    // Outside blit region should be unchanged
    expect(surface.get_plane_byte(0, 0, 8) == 0xF0, "outside blit region should be unchanged");
}

void test_masked_blit_preserves_unset_bits() {
    // Pre-fill with 0xAA (10101010), OR with 0x55 (01010101) -> 0xFF
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0xAA);

    auto image = comic2::validation::make_8x8_solid_image(0x55, 0x55, 0x55, 0x55);
    comic2::gfx_rle_blit_masked_or_4plane(surface, 0, 0, image);

    std::vector<std::vector<std::uint8_t>> expected_planes(4);
    for (std::size_t p = 0; p < 4; ++p) {
        expected_planes[p].assign(8, 0xFF);  // 0xAA | 0x55 = 0xFF
    }

    auto result = comic2::validation::validate_masked_blit(surface, image, 0, 0, expected_planes);
    expect_empty(result, "masked_blit_preserves_bits");
}

void test_masked_blit_double_or_idempotent() {
    // OR'ing the same data twice should not change the result after first pass
    comic2::EgaPlanarSurface surface1(320, 200);
    surface1.clear(0);

    comic2::EgaPlanarSurface surface2(320, 200);
    surface2.clear(0);

    auto image = comic2::validation::make_8x8_solid_image(0x01, 0x02, 0x04, 0x08);

    comic2::gfx_rle_blit_masked_or_4plane(surface1, 0, 0, image);
    comic2::gfx_rle_blit_masked_or_4plane(surface2, 0, 0, image);
    comic2::gfx_rle_blit_masked_or_4plane(surface2, 0, 0, image);  // second OR

    // Both surfaces should be identical
    for (std::size_t p = 0; p < 4; ++p) {
        for (std::size_t row = 0; row < 8; ++row) {
            expect(surface1.get_plane_byte(p, 0, row) == surface2.get_plane_byte(p, 0, row),
                   "double OR should be idempotent");
        }
    }
}

// ============================================================================
// Surface comparison and hash tests
// ============================================================================

void test_compare_surface_planes_identical() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);

    std::vector<std::vector<std::uint8_t>> expected(4);
    for (auto& p : expected) {
        p.assign(8000, 0);  // 40 bytes * 200 rows
    }

    auto result = comic2::validation::compare_surface_planes(surface, expected);
    expect_empty(result, "compare_identical_surfaces");
}

void test_compare_surface_planes_mismatch() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);
    surface.set_plane_byte(0, 0, 0, 0x42);

    std::vector<std::vector<std::uint8_t>> expected(4);
    for (auto& p : expected) {
        p.assign(8000, 0);
    }

    auto result = comic2::validation::compare_surface_planes(surface, expected);
    expect(!result.empty(), "should detect mismatch");
}

void test_hash_surface_deterministic() {
    comic2::EgaPlanarSurface surface1(320, 200);
    surface1.clear(0);
    surface1.set_plane_byte(0, 10, 5, 0xAB);

    comic2::EgaPlanarSurface surface2(320, 200);
    surface2.clear(0);
    surface2.set_plane_byte(0, 10, 5, 0xAB);

    auto hash1 = comic2::validation::hash_surface_planes(surface1);
    auto hash2 = comic2::validation::hash_surface_planes(surface2);

    expect(hash1 == hash2, "identical surfaces should have identical hashes");
}

void test_hash_surface_differs_on_byte_change() {
    comic2::EgaPlanarSurface surface1(320, 200);
    surface1.clear(0);

    comic2::EgaPlanarSurface surface2(320, 200);
    surface2.clear(0);
    surface2.set_plane_byte(0, 0, 0, 0x01);

    auto hash1 = comic2::validation::hash_surface_planes(surface1);
    auto hash2 = comic2::validation::hash_surface_planes(surface2);

    expect(hash1 != hash2, "different surfaces should have different hashes");
}

void test_hash_plane_region_deterministic() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);
    surface.set_plane_byte(0, 10, 5, 0xAB);
    surface.set_plane_byte(0, 11, 5, 0xCD);

    auto hash1 = comic2::validation::hash_plane_region(surface, 0, 10, 5, 2, 2);
    auto hash2 = comic2::validation::hash_plane_region(surface, 0, 10, 5, 2, 2);

    expect(hash1 == hash2, "same region should produce same hash");
}

void test_hash_plane_region_differs_on_offset() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);
    surface.set_plane_byte(0, 10, 5, 0xAB);

    // Region that includes the set byte
    auto hash1 = comic2::validation::hash_plane_region(surface, 0, 10, 5, 1, 1);

    // Region that does not include the set byte
    auto hash2 = comic2::validation::hash_plane_region(surface, 0, 11, 5, 1, 1);

    expect(hash1 != hash2, "different regions should produce different hashes");
}

// ============================================================================
// Region comparison tests
// ============================================================================

void test_compare_plane_region_exact_match() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);
    surface.set_plane_byte(0, 5, 10, 0x12);
    surface.set_plane_byte(0, 6, 10, 0x34);
    surface.set_plane_byte(0, 5, 11, 0x56);
    surface.set_plane_byte(0, 6, 11, 0x78);

    std::vector<std::uint8_t> expected = {0x12, 0x34, 0x56, 0x78};
    auto result = comic2::validation::compare_plane_region(surface, 0, 5, 10, 2, 2, expected);
    expect_empty(result, "compare_plane_region_exact");
}

void test_compare_plane_region_mismatch() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);
    surface.set_plane_byte(0, 5, 10, 0xFF);

    std::vector<std::uint8_t> expected = {0x00};
    auto result = comic2::validation::compare_plane_region(surface, 0, 5, 10, 1, 1, expected);
    expect(!result.empty(), "should detect region mismatch");
}

// ============================================================================
// Multi-blit composition test
// ============================================================================

void test_multi_blit_composition() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);

    // Blit two images at different positions
    auto image1 = comic2::validation::make_8x8_solid_image(0x01, 0x00, 0x00, 0x00);
    auto image2 = comic2::validation::make_8x8_solid_image(0x02, 0x00, 0x00, 0x00);

    comic2::gfx_rle_blit_opaque_4plane(surface, 0, 0, image1);
    comic2::gfx_rle_blit_opaque_4plane(surface, 16, 0, image2);

    // Region 1 should have 0x01
    std::vector<std::uint8_t> exp1(8, 0x01);
    auto result1 = comic2::validation::compare_plane_region(surface, 0, 0, 0, 1, 8, exp1);
    expect_empty(result1, "first blit region");

    // Region 2 should have 0x02
    std::vector<std::uint8_t> exp2(8, 0x02);
    auto result2 = comic2::validation::compare_plane_region(surface, 0, 2, 0, 1, 8, exp2);
    expect_empty(result2, "second blit region");

    // Between the two blits should be 0
    expect(surface.get_plane_byte(0, 1, 0) == 0, "between blits should be 0");
}

// ============================================================================
// Patterned image blit test
// ============================================================================

void test_patterned_image_blit() {
    comic2::EgaPlanarSurface surface(320, 200);
    surface.clear(0);

    auto image = comic2::validation::make_16x16_patterned_image();
    comic2::gfx_rle_blit_opaque_4plane(surface, 0, 0, image);

    // Verify the pattern at a known position
    // Plane 0: byte[i] = (0+1) * (i+1) = i+1
    expect(surface.get_plane_byte(0, 0, 0) == 1, "patterned image plane 0 byte 0");
    expect(surface.get_plane_byte(0, 1, 0) == 2, "patterned image plane 0 byte 1");

    // Plane 3: byte[i] = (3+1) * (i+1) = 4*(i+1)
    expect(surface.get_plane_byte(3, 0, 0) == 4, "patterned image plane 3 byte 0");
    expect(surface.get_plane_byte(3, 1, 0) == 8, "patterned image plane 3 byte 1");
}

}  // namespace

void run_renderer_validation_tests() {
    test_decode_known_rle_4plane_stream();
    test_decode_run_only_rle_4plane_stream();
    test_decode_mixed_rle_4plane_stream();

    test_opaque_blit_solid_image_to_empty_surface();
    test_opaque_blit_at_offset();
    test_opaque_blit_overwrites_existing_data();

    test_masked_blit_or_with_empty_surface();
    test_masked_blit_or_with_existing_data();
    test_masked_blit_preserves_unset_bits();
    test_masked_blit_double_or_idempotent();

    test_compare_surface_planes_identical();
    test_compare_surface_planes_mismatch();
    test_hash_surface_deterministic();
    test_hash_surface_differs_on_byte_change();
    test_hash_plane_region_deterministic();
    test_hash_plane_region_differs_on_offset();

    test_compare_plane_region_exact_match();
    test_compare_plane_region_mismatch();

    test_multi_blit_composition();
    test_patterned_image_blit();
}
