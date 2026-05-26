#include <stdexcept>
#include <limits>
#include <vector>

#include "comic2/entity_runtime.hpp"
#include "comic2/projectiles.hpp"
#include "comic2/room_loader.hpp"

namespace {

void expect(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_entity_runtime_prunes_inactive_slots() {
    std::vector<comic2::RuntimeEntitySlot32> slots(2);
    slots[0].mapped_object_ptr = 0x1234;
    slots[1].mapped_object_ptr = 0;

    comic2::prune_inactive_runtime_slots(slots);

    expect(slots.size() == 1, "prune should keep only active runtime slots");
    expect(slots[0].mapped_object_ptr == 0x1234, "active slot should be preserved");
}

void test_projectile_updates_and_despawns_out_of_bounds() {
    std::vector<comic2::ProjectileState> projectiles;
    comic2::spawn_projectile(projectiles, 10, 10, 3, 0);

    comic2::update_projectiles(projectiles, comic2::ProjectileBounds{.min_x = 0, .max_x = 11, .min_y = 0, .max_y = 50});

    expect(projectiles.size() == 1, "spawn should append one projectile");
    expect(!projectiles[0].active, "projectile should deactivate when out of bounds");
}

void test_room_loader_decodes_frdata_entry() {
    const std::vector<std::uint8_t> bytes = {
        0x28, 0x00,  // tile_w = 40
        0x18, 0x00,  // tile_h = 24
        0x34, 0x12,  // rle_data_off = 0x1234
    };

    const std::optional<comic2::FrdataRoomEntry> entry = comic2::decode_frdata_room_entry(bytes, 0);
    expect(entry.has_value(), "decode should succeed on full room entry payload");
    expect(entry->tile_w == 40, "tile_w decode mismatch");
    expect(entry->tile_h == 24, "tile_h decode mismatch");
    expect(entry->rle_data_off == 0x1234, "rle_data_off decode mismatch");
}

void test_room_loader_rejects_huge_offset() {
    const std::vector<std::uint8_t> bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    const std::optional<comic2::FrdataRoomEntry> entry =
        comic2::decode_frdata_room_entry(bytes, std::numeric_limits<std::size_t>::max());
    expect(!entry.has_value(), "decode should reject oversized offsets safely");
}

}  // namespace

void run_subsystem_scaffold_tests() {
    test_entity_runtime_prunes_inactive_slots();
    test_projectile_updates_and_despawns_out_of_bounds();
    test_room_loader_decodes_frdata_entry();
    test_room_loader_rejects_huge_offset();
}
