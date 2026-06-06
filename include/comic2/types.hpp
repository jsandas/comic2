#pragma once

#include <cstdint>
#include <vector>

namespace comic2 {

#pragma pack(push, 1)

struct MappedObject12 {
  std::uint16_t room_x;
  std::uint16_t room_y;
  std::uint16_t descriptor_ptr;
  std::uint16_t state_flags;
  std::uint16_t world_x;
  std::uint16_t world_y;

  bool operator==(const MappedObject12 &) const = default;
};

struct ActiveEntity8 {
  std::int16_t x;
  std::int16_t y;
  std::uint16_t flags_or_slot;
  std::uint16_t sprite_or_obj;

  bool operator==(const ActiveEntity8 &) const = default;
};

struct RuntimeEntitySlot32 {
  std::uint16_t hitbox_w;
  std::uint16_t hitbox_h;
  std::uint16_t type_flags;
  std::uint16_t behavior_state;
  std::uint16_t anim_span;
  std::uint16_t anim_period;
  std::uint16_t unknown_0c;
  std::uint16_t unknown_0e;
  std::int16_t x;
  std::int16_t y;
  std::int16_t param_a;
  std::int16_t param_b;
  std::uint16_t dir_toggle;
  std::uint16_t anim_tick;
  std::uint16_t unknown_1c;
  std::uint16_t mapped_object_ptr;

  bool operator==(const RuntimeEntitySlot32 &) const = default;
};

struct FrdataRoomEntry {
  std::uint16_t tile_w;
  std::uint16_t tile_h;
  std::uint16_t rle_data_off;

  bool operator==(const FrdataRoomEntry &) const = default;
};

#pragma pack(pop)

struct RoomTileGrid {
  std::uint16_t tile_w = 0;
  std::uint16_t tile_h = 0;
  std::vector<std::uint16_t> row_pointers;
  std::vector<std::uint8_t> tile_data;

  bool operator==(const RoomTileGrid &) const = default;
};

} // namespace comic2
