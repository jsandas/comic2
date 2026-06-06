#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace comic2 {

#pragma pack(push, 1)

struct EgaRle4PlaneHeader {
    std::uint16_t row_span_bytes;
};

struct OpaqueSpriteRecordHeader {
    std::uint16_t width_pixels;
    std::uint16_t height_rows;
};

struct MaskedSpriteRecordHeader {
    std::uint16_t width_pixels;
    std::uint16_t height_rows;
    std::uint16_t image_data_off;
};

#pragma pack(pop)

struct Ega4PlaneImage {
    std::uint16_t row_span_bytes = 0;  // total decoded bytes per plane (from file header)
    std::uint16_t width_bytes = 0;     // row stride in bytes (must be set by caller for blitting)
    std::uint16_t height_rows = 0;     // image height in rows (must be set by caller for blitting)
    std::array<std::vector<std::uint8_t>, 4> planes;
};

struct SignedRleResult {
    std::vector<std::uint8_t> bytes;
    std::size_t consumed = 0;
};

SignedRleResult decode_signed_rle(std::span<const std::uint8_t> encoded);
Ega4PlaneImage decode_ega_4plane_rle(std::span<const std::uint8_t> encoded, bool require_full_consumption = true);

}  // namespace comic2
