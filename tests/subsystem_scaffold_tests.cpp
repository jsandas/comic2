#include <algorithm>
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
    encoded.insert(encoded.end(), bytes.begin() + static_cast<std::ptrdiff_t>(offset),
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
      comic2::RoomTileGrid{}, 0, 0);

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
  expect(projectiles[0].x_vel == 0, "projectile x_vel should be 0 when facing "
                                    "left and airborne (adjustment cancels)");
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
      empty_grid, 100, 0);

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
        empty_grid, 0, 0);
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

void test_room_loader_rejects_huge_offset() {
  const std::vector<std::uint8_t> bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const std::optional<comic2::FrdataRoomEntry> entry =
      comic2::decode_frdata_room_entry(bytes,
                                       std::numeric_limits<std::size_t>::max());
  expect(!entry.has_value(), "decode should reject oversized offsets safely");
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
  test_room_loader_rejects_huge_offset();
  test_room_loader_populates_runtime_state_from_resource_buffer();
}
