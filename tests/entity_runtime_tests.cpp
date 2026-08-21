#include <stdexcept>
#include <vector>

#include "comic2/entity_runtime.hpp"
#include "comic2/game_state.hpp"

namespace {

void expect(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void test_entity_behavior_dispatch_handles_chase_bounce_and_jump() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.x = 80;
  state.player.y = 64;
  state.player.facing_right = true;

  state.runtime_slots.resize(3);
  state.runtime_slots[0].mapped_object_ptr = 1;
  state.runtime_slots[0].behavior_state = 0x0001;
  state.runtime_slots[0].x = 40;
  state.runtime_slots[0].y = 64;
  state.runtime_slots[0].param_a = 0;
  state.runtime_slots[0].param_b = 0;

  state.runtime_slots[1].mapped_object_ptr = 2;
  state.runtime_slots[1].behavior_state = 0x0002;
  state.runtime_slots[1].x = 60;
  state.runtime_slots[1].y = 70;
  state.runtime_slots[1].param_a = 0;
  state.runtime_slots[1].param_b = 0;

  state.runtime_slots[2].mapped_object_ptr = 3;
  state.runtime_slots[2].behavior_state = 0x0003;
  state.runtime_slots[2].x = 90;
  state.runtime_slots[2].y = 60;
  state.runtime_slots[2].param_a = 0;
  state.runtime_slots[2].param_b = 0;

  comic2::update_entity_behaviors(state);

  expect(state.runtime_slots[0].x > 40,
         "chase behavior should move toward player");
  expect(state.runtime_slots[1].x == 60,
         "bounce behavior should keep horizontal position stable");
  expect(state.runtime_slots[1].y != 70,
         "bounce behavior should change vertical position");
  expect(state.runtime_slots[2].y < 60, "jump behavior should raise entity");
}

void test_entity_damage_and_invulnerability_flow() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.x = 50;
  state.player.y = 50;
  state.player.hp = 8;
  state.player.invuln_ticks = 0;

  state.runtime_slots.resize(1);
  state.runtime_slots[0].mapped_object_ptr = 1;
  state.runtime_slots[0].behavior_state = 0x0000;
  state.runtime_slots[0].x = 48;
  state.runtime_slots[0].y = 48;
  state.runtime_slots[0].hitbox_w = 12;
  state.runtime_slots[0].hitbox_h = 12;

  comic2::apply_entity_combat(state);

  expect(state.player.hp == 7, "damage should reduce player hp");
  expect(state.player.invuln_ticks == 12,
         "damage should start invulnerability timer");
  expect(state.player.x < 50, "damage should apply knockback recoil");
}

void test_pickups_update_state_and_remove_collectibles() {
  comic2::RuntimeState state = comic2::make_default_runtime_state();
  state.player.x = 50;
  state.player.y = 50;
  state.player.gems = 2;
  state.player.firepower = 1;

  state.runtime_slots.resize(2);
  state.runtime_slots[0].mapped_object_ptr = 1;
  state.runtime_slots[0].behavior_state = 0x0004;
  state.runtime_slots[0].x = 48;
  state.runtime_slots[0].y = 48;
  state.runtime_slots[0].hitbox_w = 8;
  state.runtime_slots[0].hitbox_h = 8;

  state.runtime_slots[1].mapped_object_ptr = 2;
  state.runtime_slots[1].behavior_state = 0x0005;
  state.runtime_slots[1].x = 60;
  state.runtime_slots[1].y = 50;
  state.runtime_slots[1].hitbox_w = 8;
  state.runtime_slots[1].hitbox_h = 8;

  comic2::apply_entity_combat(state);

  expect(state.player.gems == 3, "gem pickup should increase gem count");
  expect(state.player.firepower == 2,
         "powerup pickup should increase firepower");
  expect(state.runtime_slots[0].mapped_object_ptr == 0,
         "collected gem should deactivate runtime slot");
  expect(state.runtime_slots[1].mapped_object_ptr == 0,
         "collected powerup should deactivate runtime slot");
}

} // namespace

void run_entity_runtime_tests() {
  test_entity_behavior_dispatch_handles_chase_bounce_and_jump();
  test_entity_damage_and_invulnerability_flow();
  test_pickups_update_state_and_remove_collectibles();
}
