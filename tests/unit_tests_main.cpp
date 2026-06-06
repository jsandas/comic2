#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "comic2/resource_formats.hpp"

void run_dispatcher_tests();
void run_player_controller_tests();
void run_renderer_tests();
void run_renderer_validation_tests();
void run_subsystem_scaffold_tests();
void run_tile_collision_tests();

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void test_signed_rle_decode() {
  // Encoded stream:
  // 0x03 -> copy 3 literal bytes [0x10,0x11,0x12]
  // 0xFD -> run length 3 of 0xAA
  // 0x00 -> terminator
  const std::vector<std::uint8_t> encoded = {0x03, 0x10, 0x11, 0x12,
                                             0xFD, 0xAA, 0x00};
  const auto out = comic2::decode_signed_rle(encoded);

  const std::vector<std::uint8_t> expected = {0x10, 0x11, 0x12,
                                              0xAA, 0xAA, 0xAA};
  expect(out.bytes == expected, "decode_signed_rle produced unexpected bytes");
  expect(out.consumed == encoded.size(),
         "decode_signed_rle consumed byte count mismatch");
}

void test_ega_4plane_decode() {
  // row_span = 2 bytes/plane
  // Plane 0: literal(2) 01 02
  // Plane 1: run(2) value 03
  // Plane 2: literal(2) 04 05
  // Plane 3: run(2) value 06
  const std::vector<std::uint8_t> encoded = {
      0x02, 0x00, 0x02, 0x01, 0x02, 0x82, 0x03, 0x02, 0x04, 0x05, 0x82, 0x06,
  };

  const auto image = comic2::decode_ega_4plane_rle(encoded, true);
  expect(image.row_span_bytes == 2, "decode_ega_4plane_rle row_span mismatch");

  expect((image.planes[0] == std::vector<std::uint8_t>{0x01, 0x02}),
         "plane 0 mismatch");
  expect((image.planes[1] == std::vector<std::uint8_t>{0x03, 0x03}),
         "plane 1 mismatch");
  expect((image.planes[2] == std::vector<std::uint8_t>{0x04, 0x05}),
         "plane 2 mismatch");
  expect((image.planes[3] == std::vector<std::uint8_t>{0x06, 0x06}),
         "plane 3 mismatch");
}

} // namespace

int main() {
  try {
    test_signed_rle_decode();
    test_ega_4plane_decode();
    run_dispatcher_tests();
    run_player_controller_tests();
    run_renderer_tests();
    run_renderer_validation_tests();
    run_subsystem_scaffold_tests();
    run_tile_collision_tests();
    std::cout << "comic2_unit_tests: OK\n";
    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "comic2_unit_tests: FAIL: " << ex.what() << "\n";
    return 1;
  }
}
