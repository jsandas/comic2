#include <algorithm>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "comic2/entity_runtime.hpp"
#include "comic2/projectiles.hpp"
#include "comic2/room_loader.hpp"
#include "comic2/tile_collision.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void expect_eq(std::size_t actual, std::size_t expected, const char *message) {
  if (actual != expected) {
    throw std::runtime_error(message);
  }
}

std::vector<std::uint8_t>
encode_literal_signed_rle(const std::vector<std::uint8_t> &bytes) {
  std::vector<std::uint8_t> encoded;
  std::size_t offset = 0;
  while (offset < bytes.size()) {
    const std::size_t chunk = std::min<std::size_t>(127, bytes.size() - offset);
    encoded.push_back(static_cast<std::uint8_t>(chunk));
    encoded.insert(encoded.end(),
                   bytes.begin() + static_cast<std::ptrdiff_t>(offset),
                   bytes.begin() + static_cast<std::ptrdiff_t>(offset + chunk));
    offset += chunk;
  }
  encoded.push_back(0x00);
  return encoded;
}

void test_entity_runtime_prunes_inactive_slots() {
  std::vector<comic2::RuntimeEntitySlot32> slots(2);
  slots[0].mapped_object_ptr = 0x1234;
  slots[1].mapped_object_ptr = 0;

  comic2::prune_inactive_runtime_slots(slots);

  expect(slots.size() == 1, "prune should keep only active runtime slots");
  expect(slots[0].mapped_object_ptr == 0x1234,
         "active slot should be preserved");
}

void test_ent_build_room_entity_list_filters_by_room() {
  std::vector<comic2::MappedObject12> mapped_objects = {
      {.room_x = 0,
       .room_y = 0,
       .descriptor_ptr = 0x100,
       .state_flags = 0,
       .world_x = 50,
       .world_y = 100},
      {.room_x = 1,
       .room_y = 0,
       .descriptor_ptr = 0x200,
       .state_flags = 0,
       .world_x = 150,
       .world_y = 100},
      {.room_x = 0,
       .room_y = 0,
       .descriptor_ptr = 0x300,
       .state_flags = 0,
       .world_x = 250,
       .world_y = 100},
  };

  std::vector<comic2::ActiveEntity8> active_entities;
  comic2::EntityActivationState activation_state;
  comic2::ent_build_room_entity_list(mapped_objects, 0, 0, active_entities,
                                     activation_state);

  expect_eq(active_entities.size(), 2, "should find 2 entities in room (0,0)");
  expect(active_entities[0].x == 50 && active_entities[0].y == 100,
         "first entity position mismatch");
  expect(active_entities[1].x == 250 && active_entities[1].y == 100,
         "second entity position mismatch");
  expect(activation_state.active_count == 2, "activation state count mismatch");
}

void test_ent_build_runtime_slots_for_viewport_culls() {
  std::vector<comic2::ActiveEntity8> active_entities = {
      {.x = 10, .y = 10, .flags_or_slot = 0, .sprite_or_obj = 0},
      {.x = 500, .y = 10, .flags_or_slot = 0, .sprite_or_obj = 1},
      {.x = 100, .y = 10, .flags_or_slot = 0, .sprite_or_obj = 2},
  };

  std::vector<comic2::MappedObject12> mapped_objects = {
      {.room_x = 16,
       .room_y = 16,
       .descriptor_ptr = 0x100,
       .state_flags = 0,
       .world_x = 10,
       .world_y = 10},
      {.room_x = 16,
       .room_y = 16,
       .descriptor_ptr = 0x200,
       .state_flags = 0,
       .world_x = 500,
       .world_y = 10},
      {.room_x = 16,
       .room_y = 16,
       .descriptor_ptr = 0x300,
       .state_flags = 0,
       .world_x = 100,
       .world_y = 10},
  };

  std::vector<comic2::RuntimeEntitySlot32> runtime_slots;
  comic2::EntityActivationState activation_state;
  comic2::EntityViewportBounds viewport{
      .min_x = 0, .max_x = 319, .min_y = 0, .max_y = 199};
  std::uint16_t activation_toggle = 1;

  comic2::ent_build_runtime_slots_for_viewport(
      active_entities, mapped_objects, runtime_slots, viewport,
      activation_state, activation_toggle);

  expect_eq(runtime_slots.size(), 6, "should have 6 runtime slots");
  expect(activation_state.active_count <= 2,
         "should activate at most 2 slots (entities in viewport)");

  const auto active_count = std::count_if(
      runtime_slots.begin(), runtime_slots.end(),
      [](const auto &slot) { return slot.mapped_object_ptr != 0; });
  expect(active_count <= 2, "should have at most 2 active runtime slots");
}

void test_ent_copy_descriptor_to_runtime_slot_initializes() {
  comic2::MappedObject12 descriptor = {
      .room_x = 16,
      .room_y = 16,
      .descriptor_ptr = 0x100,
      .state_flags = 1,
      .world_x = 100,
      .world_y = 50,
  };

  comic2::RuntimeEntitySlot32 slot = {};
  std::uint16_t activation_toggle = 1;

  comic2::ent_copy_descriptor_to_runtime_slot(descriptor, slot, 0x42,
                                              activation_toggle);

  expect(slot.mapped_object_ptr == 0x42,
         "mapped_object_ptr should be set to behavior_ptr");
  expect(slot.x == 100 && slot.y == 50,
         "position should be copied from descriptor");
  expect(slot.hitbox_w == 16 && slot.hitbox_h == 16,
         "hitbox should be copied from descriptor");
  expect(slot.behavior_state == 1,
         "behavior_state should be copied from state_flags");
  expect(slot.dir_toggle == 1,
         "dir_toggle should be initialized to 1 (activation_toggle & 3)");
  expect(activation_toggle == 2,
         "activation_toggle should be XORed with 3 (1 ^ 3 = 2)");
}

void test_deactivate_runtime_slot_clears() {
  comic2::RuntimeEntitySlot32 slot = {
      .hitbox_w = 0,
      .hitbox_h = 0,
      .type_flags = 0x10,
      .behavior_state = 5,
      .anim_span = 0,
      .anim_period = 0,
      .unknown_0c = 0,
      .unknown_0e = 0,
      .x = 100,
      .y = 50,
      .param_a = 0,
      .param_b = 0,
      .dir_toggle = 0,
      .anim_tick = 0,
      .unknown_1c = 0,
      .mapped_object_ptr = 0x1234,
  };

  comic2::deactivate_runtime_slot(slot);

  expect(slot.mapped_object_ptr == 0, "mapped_object_ptr should be cleared");
  expect(slot.behavior_state == 0, "behavior_state should be cleared");
  expect(slot.type_flags == 0, "type_flags should be cleared");
  expect(slot.x == 100 && slot.y == 50, "position should be preserved");
}

void test_projectile_updates_and_despawns_out_of_bounds() {
  std::vector<comic2::ProjectileState> projectiles;
  comic2::spawn_projectile(projectiles, 10, 10, 3, 0);

  comic2::update_projectiles(
      projectiles,
      comic2::ProjectileBounds{
          .min_x = 0, .max_x = 11, .min_y = 0, .max_y = 50},
      comic2::RoomTileGrid{}, 0, 0, 200, 152);

  expect(projectiles.size() == 1, "spawn should append one projectile");
  expect(!projectiles[0].active,
         "projectile should deactivate when out of bounds");
}

void test_spawn_player_projectile_with_facing() {
  std::vector<comic2::ProjectileState> projectiles;

  comic2::ProjectileSpawnParams params;
  params.x = 100;
  params.y = 50;
  params.facing_right = true;
  params.is_airborne = false;
  params.firepower = 1;

  comic2::spawn_player_projectile(projectiles, params);

  expect_eq(projectiles.size(), 1, "should spawn one projectile");
  expect(projectiles[0].x == 104, "projectile x should be offset by 4");
  expect(projectiles[0].y == 58,
         "projectile y should be player y + 8 when grounded");
  expect(projectiles[0].x_vel == 0x0010,
         "projectile x_vel should be positive when facing right");
  expect(projectiles[0].y_vel == -0x000C,
         "projectile y_vel should be negative (upward arc)");
  expect(projectiles[0].active, "projectile should be active");
}

void test_spawn_player_projectile_airborne_adjustment() {
  std::vector<comic2::ProjectileState> projectiles;

  comic2::ProjectileSpawnParams params;
  params.x = 100;
  params.y = 50;
  params.facing_right = false;
  params.is_airborne = true;
  params.firepower = 1;

  comic2::spawn_player_projectile(projectiles, params);

  expect_eq(projectiles.size(), 1, "should spawn one projectile");
  expect(
      projectiles[0].x_vel == -0x0010,
      "projectile x_vel should remain -0x0010 when facing left and airborne");
  expect(projectiles[0].y == 54,
         "projectile y should be player y + 4 when airborne");
}

void test_projectile_tile_collision_detection() {
  comic2::RoomTileGrid room_grid;
  room_grid.tile_w = 40;
  room_grid.tile_h = 24;
  room_grid.row_pointers.resize(24, 0);
  room_grid.tile_data.resize(40 * 24 * 2, 0x01);

  comic2::ProjectileState projectile;
  projectile.x = 50;
  projectile.y = 50;
  projectile.active = true;

  const bool collision =
      comic2::check_projectile_tile_collision(projectile, room_grid);
  expect(collision, "projectile should collide with non-zero tile");
}

void test_projectile_viewport_culling() {
  std::vector<comic2::ProjectileState> projectiles;
  comic2::spawn_projectile(projectiles, 100, 100, -20, 0);

  comic2::RoomTileGrid empty_grid;
  empty_grid.tile_w = 40;
  empty_grid.tile_h = 24;

  comic2::update_projectiles(
      projectiles,
      comic2::ProjectileBounds{
          .min_x = 0, .max_x = 319, .min_y = 0, .max_y = 199},
      empty_grid, 100, 0, 200, 152);

  expect(!projectiles[0].active, "projectile should deactivate when outside "
                                 "viewport bounds (x < viewport_min_x - 8)");
}

void test_projectile_anim_frame_cycles() {
  std::vector<comic2::ProjectileState> projectiles;
  comic2::spawn_projectile(projectiles, 100, 100, 1, 0);

  comic2::RoomTileGrid empty_grid;
  empty_grid.tile_w = 40;
  empty_grid.tile_h = 24;

  for (int i = 0; i < 10; ++i) {
    comic2::update_projectiles(
        projectiles,
        comic2::ProjectileBounds{
            .min_x = 0, .max_x = 319, .min_y = 0, .max_y = 199},
        empty_grid, 0, 0, 200, 152);
  }

  expect(projectiles[0].anim_frame == 2, "anim_frame should cycle modulo 8");
}

void test_ent_activation_pipeline_integration() {
  std::vector<comic2::MappedObject12> mapped_objects = {
      {.room_x = 10,
       .room_y = 20,
       .descriptor_ptr = 0x100,
       .state_flags = 0,
       .world_x = 50,
       .world_y = 50},
      {.room_x = 10,
       .room_y = 20,
       .descriptor_ptr = 0x200,
       .state_flags = 0,
       .world_x = 150,
       .world_y = 50},
      {.room_x = 0,
       .room_y = 0,
       .descriptor_ptr = 0x300,
       .state_flags = 0,
       .world_x = 10,
       .world_y = 10},
  };

  std::vector<comic2::ActiveEntity8> active_entities;
  comic2::EntityActivationState activation_state;
  comic2::ent_build_room_entity_list(mapped_objects, 10, 20, active_entities,
                                     activation_state);

  expect_eq(active_entities.size(), 2,
            "should find 2 entities in room (10,20)");
  expect(active_entities[0].sprite_or_obj == 0,
         "first active entity should point to mapped_objects[0]");
  expect(active_entities[1].sprite_or_obj == 1,
         "second active entity should point to mapped_objects[1]");

  std::vector<comic2::RuntimeEntitySlot32> runtime_slots;
  comic2::EntityViewportBounds viewport{
      .min_x = 0, .max_x = 319, .min_y = 0, .max_y = 199};
  std::uint16_t activation_toggle = 1;

  comic2::ent_build_runtime_slots_for_viewport(
      active_entities, mapped_objects, runtime_slots, viewport,
      activation_state, activation_toggle);

  expect(runtime_slots[0].mapped_object_ptr == 0,
         "first runtime slot should point to mapped_objects[0]");
  expect(runtime_slots[1].mapped_object_ptr == 1,
         "second runtime slot should point to mapped_objects[1]");
  expect(runtime_slots[0].type_flags == 0x100,
         "first runtime slot type_flags mismatch");
  expect(runtime_slots[1].type_flags == 0x200,
         "second runtime slot type_flags mismatch");
}

void test_room_loader_decodes_frdata_entry() {
  const std::vector<std::uint8_t> bytes = {
      0x28, 0x00, // tile_w = 40
      0x18, 0x00, // tile_h = 24
      0x34, 0x12, // rle_data_off = 0x1234
  };

  const std::optional<comic2::FrdataRoomEntry> entry =
      comic2::decode_frdata_room_entry(bytes, 0);
  expect(entry.has_value(), "decode should succeed on full room entry payload");
  expect(entry->tile_w == 40, "tile_w decode mismatch");
  expect(entry->tile_h == 24, "tile_h decode mismatch");
  expect(entry->rle_data_off == 0x1234, "rle_data_off decode mismatch");
}

void test_room_loader_resolves_room_payload_location() {
  std::vector<std::uint8_t> bytes(0x40, 0x00);
  bytes[2] = 0x03;
  bytes[3] = 0x00;
  bytes[0x04] = 0x04;
  bytes[0x05] = 0x00;
  bytes[0x06] = 0x03;
  bytes[0x07] = 0x00;
  bytes[0x08] = 0x20;
  bytes[0x09] = 0x00;

  const std::optional<comic2::RoomLoadSpec> spec =
      comic2::resolve_room_load_spec("/tmp/FRDATA.0", bytes, 3, 0,
                                     comic2::ResourceAssetKind::RoomPayload);
  expect(spec.has_value(), "resolver should map a valid room payload");
  expect(spec->source_path == std::filesystem::path("/tmp/FRDATA.0"),
         "resolver should preserve the source path");
  expect(spec->table_offset == 0x04, "resolver should report table offset");
  expect(spec->room_entry_offset == 0x04,
         "resolver should report room entry offset");
  expect(spec->resource_offset == 0x20,
         "resolver should report room payload offset");
  expect(spec->room_entry.tile_w == 4, "resolver should decode tile_w");
  expect(spec->room_entry.tile_h == 3, "resolver should decode tile_h");
  expect(spec->room_entry.rle_data_off == 0x20,
         "resolver should decode payload offset");

  const std::optional<comic2::RoomLoadSpec> table_spec =
      comic2::resolve_room_load_spec("/tmp/FRDATA.0", bytes, 3, 0,
                                     comic2::ResourceAssetKind::RoomTable);
  expect(table_spec.has_value(), "resolver should map room table access");
  expect(table_spec->resource_offset == 0x04,
         "room table access should point at the room entry");
}

void test_room_loader_rejects_huge_offset() {
  const std::vector<std::uint8_t> bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const std::optional<comic2::FrdataRoomEntry> entry =
      comic2::decode_frdata_room_entry(bytes,
                                       std::numeric_limits<std::size_t>::max());
  expect(!entry.has_value(), "decode should reject oversized offsets safely");
}

void test_room_loader_rejects_out_of_bounds_room_index() {
  std::vector<std::uint8_t> bytes(0x0A, 0x00);
  bytes[2] = 0x03;
  bytes[3] = 0x00;
  bytes[0x04] = 0x04;
  bytes[0x05] = 0x00;
  bytes[0x06] = 0x03;
  bytes[0x07] = 0x00;
  bytes[0x08] = 0x20;
  bytes[0x09] = 0x00;

  comic2::RuntimeState state;
  const bool loaded =
      comic2::load_room_tilemap_from_resource_buffer(state, bytes, 3, 1);
  expect(!loaded,
         "room loader should reject a room index that is outside the table");
}

void test_room_loader_rejects_sentinel_entries() {
  std::vector<std::uint8_t> bytes(0x10, 0x00);
  bytes[2] = 0x03;
  bytes[3] = 0x00;
  bytes[0x04] = 0xFF;
  bytes[0x05] = 0xFF;
  bytes[0x06] = 0xFF;
  bytes[0x07] = 0xFF;
  bytes[0x08] = 0xFF;
  bytes[0x09] = 0xFF;

  const std::optional<comic2::RoomLoadSpec> spec =
      comic2::resolve_room_load_spec("/tmp/FRDATA.0", bytes, 3, 0,
                                     comic2::ResourceAssetKind::RoomPayload);
  expect(!spec.has_value(),
         "resolver should reject sentinel room entries as invalid");
}

void test_room_loader_populates_runtime_state_from_resource_buffer() {
  std::vector<std::uint8_t> decoded_room_bytes(0x2C4, 0x00);
  decoded_room_bytes[0] = 0x11;
  decoded_room_bytes[1] = 0x22;
  decoded_room_bytes[2] = 0x33;
  decoded_room_bytes[3] = 0x44;
  decoded_room_bytes[4] = 0x55;
  decoded_room_bytes[5] = 0x66;

  decoded_room_bytes[0x2A0] = 0x00;
  decoded_room_bytes[0x2A1] = 0x00;
  decoded_room_bytes[0x2A2] = 0x04;
  decoded_room_bytes[0x2A3] = 0x00;
  decoded_room_bytes[0x2A4] = 0x08;
  decoded_room_bytes[0x2A5] = 0x00;

  const std::vector<std::uint8_t> encoded_room_bytes =
      encode_literal_signed_rle(decoded_room_bytes);

  std::vector<std::uint8_t> resource_bytes(0x20, 0x00);
  resource_bytes[2] = 0x03;
  resource_bytes[3] = 0x00;

  resource_bytes.resize(0x20, 0x00);
  resource_bytes.resize(0x20 + encoded_room_bytes.size(), 0x00);
  std::copy(encoded_room_bytes.begin(), encoded_room_bytes.end(),
            resource_bytes.begin() + 0x20);

  resource_bytes[0x04] = 0x04;
  resource_bytes[0x05] = 0x00;
  resource_bytes[0x06] = 0x03;
  resource_bytes[0x07] = 0x00;
  resource_bytes[0x08] = 0x20;
  resource_bytes[0x09] = 0x00;

  comic2::RuntimeState state;
  const bool loaded = comic2::load_room_tilemap_from_resource_buffer(
      state, resource_bytes, 3, 0);

  expect(loaded, "room loader should populate runtime state from resource");
  expect(state.current_level == 3, "room loader should set current level");
  expect(state.current_room == 0, "room loader should set current room");
  expect(state.room_entry.tile_w == 4, "room entry tile_w should be stored");
  expect(state.room_entry.tile_h == 3, "room entry tile_h should be stored");
  expect(state.room_grid.tile_w == 4, "room grid tile_w should match entry");
  expect(state.room_grid.tile_h == 3, "room grid tile_h should match entry");
  expect(state.room_grid.row_pointers == std::vector<std::uint16_t>{0, 4, 8},
         "room loader should build row pointer table");
  expect(state.room_grid.tile_data == decoded_room_bytes,
         "room loader should store decoded room bytes");
}

void test_room_loader_handles_self_referential_resource_bytes_span() {
  std::vector<std::uint8_t> decoded_room_bytes(0x2C4, 0x00);
  decoded_room_bytes[0x2A0] = 0x00;
  decoded_room_bytes[0x2A1] = 0x00;
  decoded_room_bytes[0x2A2] = 0x04;
  decoded_room_bytes[0x2A3] = 0x00;
  decoded_room_bytes[0x2A4] = 0x08;
  decoded_room_bytes[0x2A5] = 0x00;

  const std::vector<std::uint8_t> encoded_room_bytes =
      encode_literal_signed_rle(decoded_room_bytes);

  std::vector<std::uint8_t> resource_bytes(0x20 + encoded_room_bytes.size(),
                                           0x00);
  resource_bytes[2] = 0x03;
  resource_bytes[3] = 0x00;
  resource_bytes[0x04] = 0x04;
  resource_bytes[0x05] = 0x00;
  resource_bytes[0x06] = 0x03;
  resource_bytes[0x07] = 0x00;
  resource_bytes[0x08] = 0x20;
  resource_bytes[0x09] = 0x00;
  std::copy(encoded_room_bytes.begin(), encoded_room_bytes.end(),
            resource_bytes.begin() + 0x20);

  comic2::RuntimeState state;
  state.room_resource_bytes = resource_bytes;

  const bool loaded = comic2::load_room_tilemap_from_resource_buffer(
      state, state.room_resource_bytes, 3, 0);

  expect(loaded, "room loader should tolerate a self-referential span");
  expect(state.room_resource_bytes == resource_bytes,
         "room loader should preserve the copied resource bytes");
}

void test_room_loader_decodes_mapped_objects_from_payload() {
  std::vector<std::uint8_t> decoded_room_bytes(0x2D2, 0x00);
  decoded_room_bytes[0x2B0] = 0x02;
  decoded_room_bytes[0x2B1] = 0x00;

  // Record 0
  decoded_room_bytes[0x2B2] = 0x01;
  decoded_room_bytes[0x2B3] = 0x00;
  decoded_room_bytes[0x2B4] = 0x03;
  decoded_room_bytes[0x2B5] = 0x00;
  decoded_room_bytes[0x2B6] = 0x34;
  decoded_room_bytes[0x2B7] = 0x12;
  decoded_room_bytes[0x2B8] = 0x02;
  decoded_room_bytes[0x2B9] = 0x00;
  decoded_room_bytes[0x2BA] = 0x64;
  decoded_room_bytes[0x2BB] = 0x00;
  decoded_room_bytes[0x2BC] = 0x32;
  decoded_room_bytes[0x2BD] = 0x00;

  // Record 1
  decoded_room_bytes[0x2BE] = 0x01;
  decoded_room_bytes[0x2BF] = 0x00;
  decoded_room_bytes[0x2C0] = 0x03;
  decoded_room_bytes[0x2C1] = 0x00;
  decoded_room_bytes[0x2C2] = 0x78;
  decoded_room_bytes[0x2C3] = 0x56;
  decoded_room_bytes[0x2C4] = 0x01;
  decoded_room_bytes[0x2C5] = 0x00;
  decoded_room_bytes[0x2C6] = 0x2C;
  decoded_room_bytes[0x2C7] = 0x01;
  decoded_room_bytes[0x2C8] = 0x5A;
  decoded_room_bytes[0x2C9] = 0x00;

  const auto mapped = comic2::decode_room_mapped_objects(decoded_room_bytes);
  expect(mapped.has_value(), "mapped object decode should succeed");
  expect_eq(mapped->size(), 2, "mapped object decode should return 2 records");
  expect((*mapped)[0].descriptor_ptr == 0x1234,
         "first descriptor pointer mismatch");
  expect((*mapped)[1].descriptor_ptr == 0x5678,
         "second descriptor pointer mismatch");
  expect((*mapped)[1].world_x == 300 && (*mapped)[1].world_y == 90,
         "second mapped object world coords mismatch");
}

void test_room_loader_wires_runtime_tables_from_loaded_mapped_objects() {
  std::vector<std::uint8_t> decoded_room_bytes(0x2D2, 0x00);
  decoded_room_bytes[0x2A0] = 0x00;
  decoded_room_bytes[0x2A1] = 0x00;
  decoded_room_bytes[0x2A2] = 0x04;
  decoded_room_bytes[0x2A3] = 0x00;
  decoded_room_bytes[0x2A4] = 0x08;
  decoded_room_bytes[0x2A5] = 0x00;

  decoded_room_bytes[0x2B0] = 0x02;
  decoded_room_bytes[0x2B1] = 0x00;

  // Record 0 in room (0,3), inside viewport
  decoded_room_bytes[0x2B2] = 0x00;
  decoded_room_bytes[0x2B3] = 0x00;
  decoded_room_bytes[0x2B4] = 0x03;
  decoded_room_bytes[0x2B5] = 0x00;
  decoded_room_bytes[0x2B6] = 0x10;
  decoded_room_bytes[0x2B7] = 0x00;
  decoded_room_bytes[0x2B8] = 0x07;
  decoded_room_bytes[0x2B9] = 0x00;
  decoded_room_bytes[0x2BA] = 0x14;
  decoded_room_bytes[0x2BB] = 0x00;
  decoded_room_bytes[0x2BC] = 0x1E;
  decoded_room_bytes[0x2BD] = 0x00;

  // Record 1 in room (0,3), outside viewport
  decoded_room_bytes[0x2BE] = 0x00;
  decoded_room_bytes[0x2BF] = 0x00;
  decoded_room_bytes[0x2C0] = 0x03;
  decoded_room_bytes[0x2C1] = 0x00;
  decoded_room_bytes[0x2C2] = 0x20;
  decoded_room_bytes[0x2C3] = 0x00;
  decoded_room_bytes[0x2C4] = 0x08;
  decoded_room_bytes[0x2C5] = 0x00;
  decoded_room_bytes[0x2C6] = 0xF4;
  decoded_room_bytes[0x2C7] = 0x01;
  decoded_room_bytes[0x2C8] = 0x14;
  decoded_room_bytes[0x2C9] = 0x00;

  const std::vector<std::uint8_t> encoded_room_bytes =
      encode_literal_signed_rle(decoded_room_bytes);

  std::vector<std::uint8_t> resource_bytes(0x30 + encoded_room_bytes.size(),
                                           0x00);
  resource_bytes[2] = 0x03;
  resource_bytes[3] = 0x00;
  resource_bytes[0x04] = 0x04;
  resource_bytes[0x05] = 0x00;
  resource_bytes[0x06] = 0x03;
  resource_bytes[0x07] = 0x00;
  resource_bytes[0x08] = 0x30;
  resource_bytes[0x09] = 0x00;
  std::copy(encoded_room_bytes.begin(), encoded_room_bytes.end(),
            resource_bytes.begin() + 0x30);

  comic2::RuntimeState state;
  const bool loaded = comic2::load_room_tilemap_from_resource_buffer(
      state, resource_bytes, 3, 0);

  expect(loaded, "room loader should succeed with mapped-object payload");
  expect_eq(state.mapped_objects.size(), 2,
            "room loader should populate mapped object table from payload");
  expect_eq(state.active_entities.size(), 2,
            "room loader should build active entity list from mapped objects");
  expect_eq(state.runtime_slots.size(), 6,
            "room loader should provision runtime slot table");
  expect(state.activation_state.active_count == 1,
         "runtime slot wiring should cull out-of-viewport descriptor");
  expect(state.runtime_slots[0].mapped_object_ptr == 0,
         "first runtime slot should point at first mapped object");
}

void test_room_loader_handles_corrupt_mapped_object_payload_stably() {
  std::vector<std::uint8_t> decoded_room_bytes(0x2C4, 0x00);
  decoded_room_bytes[0x2A0] = 0x00;
  decoded_room_bytes[0x2A1] = 0x00;
  decoded_room_bytes[0x2A2] = 0x04;
  decoded_room_bytes[0x2A3] = 0x00;
  decoded_room_bytes[0x2A4] = 0x08;
  decoded_room_bytes[0x2A5] = 0x00;
  decoded_room_bytes[0x2B0] = 0x08;
  decoded_room_bytes[0x2B1] = 0x00;

  const std::vector<std::uint8_t> encoded_room_bytes =
      encode_literal_signed_rle(decoded_room_bytes);

  std::vector<std::uint8_t> resource_bytes(0x30 + encoded_room_bytes.size(),
                                           0x00);
  resource_bytes[2] = 0x03;
  resource_bytes[3] = 0x00;
  resource_bytes[0x04] = 0x04;
  resource_bytes[0x05] = 0x00;
  resource_bytes[0x06] = 0x03;
  resource_bytes[0x07] = 0x00;
  resource_bytes[0x08] = 0x30;
  resource_bytes[0x09] = 0x00;
  std::copy(encoded_room_bytes.begin(), encoded_room_bytes.end(),
            resource_bytes.begin() + 0x30);

  comic2::RuntimeState state;
  state.mapped_objects.push_back(
      comic2::MappedObject12{.room_x = 1,
                             .room_y = 1,
                             .descriptor_ptr = 0x1234,
                             .state_flags = 0,
                             .world_x = 10,
                             .world_y = 10});

  const bool loaded = comic2::load_room_tilemap_from_resource_buffer(
      state, resource_bytes, 3, 0);

  expect(loaded,
         "corrupt mapped-object payload should not fail room tilemap load");
  expect(state.mapped_objects.empty(),
         "corrupt mapped-object payload should clear mapped object table");
  expect(state.active_entities.empty(),
         "corrupt mapped-object payload should clear active entities");
  expect(state.runtime_slots.empty(),
         "corrupt mapped-object payload should keep runtime slots controlled");
}

void test_frpak_catalog_builds_file_index() {
  const std::vector<std::uint8_t> bytes = {
      0x40, 0x00, 0x01, 0x02, 0x03,
  };

  const auto file =
      comic2::build_frpak_catalog_file("/tmp/FRPAK.001", bytes, 1);
  expect(file.has_value(), "frpak catalog should build for valid payload");
  expect(file->pak_id == 1, "frpak catalog should preserve pak id");
  expect(file->file_size == bytes.size(),
         "frpak catalog should store file size");
  expect(file->records.size() == 1,
         "frpak catalog should expose a single direct-stream record");
  expect(file->records[0].record_id == 0,
         "frpak direct-stream record id should start at zero");
  expect(file->records[0].data_offset == 0,
         "frpak direct-stream record offset should be zero");
  expect(file->records[0].data_size == bytes.size(),
         "frpak direct-stream record size should match file size");
  expect(file->records[0].row_span_bytes == 0x40,
         "frpak direct-stream row span should decode from file header");
}

void test_frpak_catalog_rejects_truncated_header() {
  const std::vector<std::uint8_t> bytes = {0x40};
  const auto file =
      comic2::build_frpak_catalog_file("/tmp/FRPAK.001", bytes, 1);
  expect(!file.has_value(),
         "frpak catalog should reject files smaller than header size");
}

void test_frpak_catalog_rejects_zero_row_span_header() {
  const std::vector<std::uint8_t> bytes = {0x00, 0x00, 0x01};
  const auto file =
      comic2::build_frpak_catalog_file("/tmp/FRPAK.001", bytes, 1);
  expect(!file.has_value(),
         "frpak catalog should reject zero row-span headers");
}

void test_frpak_catalog_record_bounds_validation() {
  const comic2::FrpakCatalogRecord valid{
      .pak_id = 1,
      .record_id = 0,
      .data_offset = 2,
      .data_size = 4,
      .row_span_bytes = 0x20,
  };
  expect(comic2::validate_frpak_catalog_record_bounds(valid, 6),
         "frpak bounds validator should accept in-range records");

  const comic2::FrpakCatalogRecord offset_past_end{
      .pak_id = 1,
      .record_id = 1,
      .data_offset = 7,
      .data_size = 0,
      .row_span_bytes = 0x20,
  };
  expect(!comic2::validate_frpak_catalog_record_bounds(offset_past_end, 6),
         "frpak bounds validator should reject offsets beyond file size");

  const comic2::FrpakCatalogRecord size_overflow{
      .pak_id = 1,
      .record_id = 2,
      .data_offset = 4,
      .data_size = 3,
      .row_span_bytes = 0x20,
  };
  expect(!comic2::validate_frpak_catalog_record_bounds(size_overflow, 6),
         "frpak bounds validator should reject records exceeding file size");
}

void test_bootstrap_populates_frpak_catalog_for_known_files() {
  const auto root =
      std::filesystem::temp_directory_path() / "comic2_frpak_catalog_fixture";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  const std::vector<std::uint8_t> frpak_bytes = {
      0x40, 0x00, 0xAA, 0xBB, 0xCC,
  };
  {
    std::ofstream output(root / "FRPAK.001", std::ios::binary);
    output.write(reinterpret_cast<const char *>(frpak_bytes.data()),
                 static_cast<std::streamsize>(frpak_bytes.size()));
  }

  comic2::RuntimeState state;
  const auto summary = comic2::load_initial_bootstrap_resources(state, root);

  expect(summary.sprite_files_tried > 0,
         "bootstrap should attempt sprite candidate discovery");
  expect(state.frpak_catalog.files.size() == 1,
         "bootstrap should index valid FRPAK files in catalog");
  expect(state.frpak_catalog.files[0].pak_id == 1,
         "bootstrap catalog should preserve FRPAK numeric id");
  expect(state.frpak_catalog.files[0].records.size() == 1,
         "bootstrap catalog should include one record for direct payload file");

  const auto record =
      comic2::find_frpak_catalog_record(state.frpak_catalog, 1, 0);
  expect(record.has_value(),
         "catalog lookup should resolve known pak/record pair");
  expect(record->data_size == frpak_bytes.size(),
         "catalog record size should match source file size");

  std::filesystem::remove_all(root);
}

} // namespace

void run_subsystem_scaffold_tests() {
  test_entity_runtime_prunes_inactive_slots();
  test_ent_build_room_entity_list_filters_by_room();
  test_ent_build_runtime_slots_for_viewport_culls();
  test_ent_copy_descriptor_to_runtime_slot_initializes();
  test_deactivate_runtime_slot_clears();
  test_projectile_updates_and_despawns_out_of_bounds();
  test_spawn_player_projectile_with_facing();
  test_spawn_player_projectile_airborne_adjustment();
  test_projectile_tile_collision_detection();
  test_projectile_viewport_culling();
  test_projectile_anim_frame_cycles();
  test_ent_activation_pipeline_integration();
  test_room_loader_decodes_frdata_entry();
  test_room_loader_resolves_room_payload_location();
  test_room_loader_rejects_huge_offset();
  test_room_loader_rejects_out_of_bounds_room_index();
  test_room_loader_rejects_sentinel_entries();
  test_room_loader_populates_runtime_state_from_resource_buffer();
  test_room_loader_handles_self_referential_resource_bytes_span();
  test_room_loader_decodes_mapped_objects_from_payload();
  test_room_loader_wires_runtime_tables_from_loaded_mapped_objects();
  test_room_loader_handles_corrupt_mapped_object_payload_stably();
  test_frpak_catalog_builds_file_index();
  test_frpak_catalog_rejects_truncated_header();
  test_frpak_catalog_rejects_zero_row_span_header();
  test_frpak_catalog_record_bounds_validation();
  test_bootstrap_populates_frpak_catalog_for_known_files();
}
