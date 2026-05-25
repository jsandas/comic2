# Implementation Plan

## Project Objective
Reverse-engineer **Captain Comic II: Fractured Reality** into a clean C++ reimplementation, using the annotated `comic2.asm` disassembly as the primary reference. `unpacked.exe.test.c` (Ghidra pseudo-C) is a secondary cross-reference only — neither source file is intended to compile.

---

## Phase 1–4: Game Loop & Player State Analysis (COMPLETED)

### Original Objective
Map the fragmented main game loop (`game_loop`, was `sub_35DE`) and identify the core player state variables (`comic_x`, `comic_y`, etc.).

## Status
- **Phase 1: Game Loop Defragmentation** - [COMPLETED]
- **Phase 2: Player State Identification** - [COMPLETED]
- **Phase 3: Function Mapping** - [COMPLETED]

## Background & Motivation
The main game loop (`game_loop`, was `sub_35DE`) was fragmented into 21 chunks in the disassembly. Understanding this loop is critical for identifying how input is processed, physics are applied, and entities are updated. Concurrently, locating the player state variables is necessary to trace how functions interact with the player.

## Scope
- Analyze and document the control flow of `game_loop` (`sub_35DE`) and its fragments. [DONE]
- Identify memory addresses corresponding to the player's X/Y coordinates, velocity, facing direction, state (standing, running, jumping), health, and inventory. [DONE]
- Identify the core movement and physics functions (e.g., `handle_fall_or_jump`, `move_left`, `move_right`) called within the loop. [DONE]
- Update project documentation (`FUNCTION_MAP.md`, `REVERSE_ENGINEERING_GUIDE.md`, `CORE_ENGINE.md`) with the findings. [DONE]

## Implementation Steps

### Phase 1: Game Loop Defragmentation [COMPLETED]
1.  **Trace `game_loop`**: Followed the control flow starting from `game_loop` (`sub_35DE`) in `comic2.asm`.
2.  **Identify Fragments**: Documented the primary fragments and their roles in the dispatcher.
3.  **Map Loop Sequence**: Created a high-level sequence diagram in `CORE_ENGINE.md`.

### Phase 2: Player State Identification [COMPLETED]
1.  **Trace Input Variables**: Followed references to input variables into the game loop.
2.  **Locate Coordinates**: Identified `comic_x`, `comic_y`, `comic_x_vel`, `comic_y_vel`.
3.  **Locate Attributes**: Found `comic_hp` (`byte_258CD`).
4.  **Correlate with C Decompilation**: Verified offsets using `unpacked.exe.test.c`.

### Phase 3: Function Mapping [COMPLETED]
1.  **Identify Physics Functions**: Located `update_player_physics` (`sub_2A10`, grounded) and `handle_airborne_movement` (`sub_2EDC`, airborne).
2.  **Identify Movement Functions**: Located `sub_2BDC` (move_left) and `sub_2C39` (move_right).
3.  **Annotate Assembly**: Add descriptive comments and rename labels for the identified functions and variables in `comic2.asm`.

### Phase 4: Documentation Update [COMPLETED]
1.  **Update `FUNCTION_MAP.md`**: Added identified addresses.
2.  **Update `CORE_ENGINE.md`**: Added dispatcher logic and state variables.

## Completed Findings
- [x] `comic_num_lives` = `word_2526F` (`ds:28Fh`)
- [x] `comic_firepower` = `byte_251FC` (`ds:1FCh`)
- [x] `comic_gems` = `word_25204` (`ds:204h`)

---

## Phase 5: Complete Function Annotation (COMPLETED)

### Goals
- Apply all confirmed `sub_XXXX` renames in `comic2.asm`
- Annotate the EGA blit pipeline (`gfx_startup_graphics_and_menu_probe`, `gfx_render_viewport_4plane`, `gfx_copy_viewport_plane`, `ent_draw_mapped_slot_or_placeholder`, `gfx_rle_blit_opaque_4plane`, `gfx_rle_blit_masked_or_4plane`, `gfx_set_crtc_start_on_retrace`)
- Locate and annotate entity management (`handle_enemies`, `handle_fireballs`, `handle_item`, `spawn_enemy`)
- Locate and annotate file I/O / resource loading (`load_level`, `load_fullscreen_graphic`, `load_shp_files`, `rle_decode`)
- Identify sound system: INT 3 handler, sound effect dispatch, effect addresses

### Steps
1. [x] Batch-rename all confirmed `sub_XXXX` labels with known identities
2. [x] Annotate EGA blit pipeline with full function headers (`gfx_startup_graphics_and_menu_probe`, `gfx_render_viewport_4plane`, `gfx_copy_viewport_plane`, `ent_draw_mapped_slot_or_placeholder`, `gfx_rle_blit_opaque_4plane`, `gfx_rle_blit_masked_or_4plane`, `gfx_set_crtc_start_on_retrace`)
3. [x] Trace `sub_437B` and `sub_5D5F` callers to locate entity management
	- Promoted provisional mappings:
	  - `sub_437B` -> `ent_update_entities_in_viewport`
	  - `sub_5D5F` -> `update_projectiles`
	  - `sub_1A63` -> `ent_build_room_entity_list`
	  - `sub_4579` -> `ent_deactivate_at_coords`
	  - `sub_46E6` -> `ent_activate_slot_into_runtime`
	  - `sub_4B25` -> `ent_update_object_behaviors`
	- Confirmed active runtime entity list at `unk_256E0` (8-byte records)
	- Promoted room-transition presentation helpers:
	  - `sub_3BC6` -> `room_transition_draw_header`
	  - `sub_3C13` -> `room_transition_reveal_sequence_a`
	  - `sub_3CB1` -> `room_transition_reveal_sequence_b`
	  - `sub_3F0A` -> `room_transition_palette_wave`
	  - `sub_3D3A` -> `room_transition_draw_reveal_quad`
	  - `sub_3D7B` -> `room_transition_blit_reveal_quad_4plane`
	  - `sub_3D90` -> `room_transition_blit_reveal_column_plane`
	  - `sub_3DA1` -> `room_transition_clear_reveal_box`
	  - `sub_3DB6` -> `room_transition_present_frame`
	  - `sub_3E1F` -> `room_transition_player_entry_sequence`
	  - `sub_3E98` -> `room_transition_player_exit_sequence`
	  - `sub_401B` -> `event_draw_stacked_sprite_rise`
	  - `sub_4073` -> `event_transition_camera_bob_step`
	  - `sub_409C` -> `event_finale_transition_sequence`
	  - `sub_4206` -> `ui_draw_framed_message_from_stream`
	  - `sub_42D4` -> `ui_draw_string_8x8`
	  - `sub_42E8` -> `ui_draw_glyph_8x8`
	  - `sub_4339` -> `ega_set_map_mask`
	  - `sub_39AD` -> `ui_show_modal_prompt_wait_key`
	  - `sub_3A17` -> `hud_update_mode_icons`
	  - `sub_660B` -> `io_open_file_or_prompt_retry`
	  - `sub_6011` -> `ui_select_next_valid_option`
	  - `sub_6021` -> `ui_select_prev_valid_option`
	  - `sub_6030` -> `ui_is_option_selectable`
	  - `sub_606B` -> `ui_render_option_list`
	  - `sub_615C` -> `ui_option_list_input_loop`
	  - `sub_61C8` -> `ui_edit_selected_option_text`
	  - `sub_62A4` -> `cfg_load_options_or_defaults`
	  - `sub_631A` -> `cfg_save_options`
	  - `sub_633C` -> `savegame_write_snapshot`
	  - `sub_636F` -> `savegame_read_snapshot`
	  - `sub_64F5` -> `ui_draw_completion_delta_hint`
	  - `sub_6545` -> `cfg_compute_hardware_seed`
	  - `sub_65EF` -> `io_verify_game000_or_prompt_retry`
	  - `sub_65FD` -> `io_verify_frcfg_or_prompt_retry`
	  - `sub_677A` -> `ui_render_game_selection_panel`
	  - `sub_6DF4` -> `cfg_run_integrity_challenge`
	  - `sub_6D99` -> `hud_add_bcd_counter_from_low_digit`
	  - `sub_6DE7` -> `hud_add_bcd_counter_from_mid_digit`
	  - `sub_7057` -> `cfg_input_challenge_code`
	  - `sub_6F47` -> `cfg_draw_integrity_challenge_screen`
	  - `sub_7164` -> `play_intro_cinematic`
	  - `sub_7255` -> `gfx_copy_backbuffer_to_active_page`
	  - `sub_7261` -> `intro_draw_sprite_and_present`
	  - `sub_7278` -> `wait_ticks_or_abort`
	  - `sub_74E1` -> `intro_wipe_split_buffers_step`
	  - `sub_752A` -> `gfx_copy_rect_all_planes`
	  - `sub_7553` -> `gfx_copy_rect_rows_stride40`
	  - `sub_756D` -> `intro_draw_overlay_with_comic`
	  - `sub_75AD` -> `intro_flash_comic_with_optional_overlay`
	  - `sub_7622` -> `intro_draw_comic_walk_frame`
	  - `sub_7741` -> `gfx_init_double_buffer_pages`
	  - `sub_774E` -> `gfx_present_and_flip_page`
	  - `sub_7765` -> `gfx_copy_page_4plane`
	  - `sub_790C` -> `gfx_blit_sprite_opaque_both_pages`
	  - `sub_7927` -> `gfx_blit_sprite_opaque_active_page`
	  - `sub_7962` -> `gfx_blit_sprite_plane_rows`
	  - `sub_7990` -> `gfx_load_and_blit_opaque_file`
	  - `sub_7999` -> `gfx_load_and_blit_masked_file`
	  - `sub_79A2` -> `io_read_file_to_seg001_0600`
	  - `sub_79ED` -> `gfx_rle_decode_copy_rowspan`
	  - `sub_7A39` -> `gfx_rle_decode_or_rowspan`
	  - `sub_7A68` -> `ega_select_plane_read_write`
	  - `sub_77A3` -> `gfx_blit_sprite_masked_active_page`
	  - `sub_77F2` -> `gfx_blit_masked_plane_aligned`
	  - `sub_7849` -> `gfx_blit_masked_plane_shift4`
	  - `sub_78C6` -> `gfx_blit_sprite_opaque_offset_both_pages`
	  - `sub_78E1` -> `gfx_blit_sprite_opaque_offset_active_page`
	  - `sub_7AAB` -> `gfx_clear_rect_both_pages`
	  - `sub_7AC8` -> `gfx_clear_rect_active_page`
	  - `sub_7B03` -> `gfx_clear_rect_plane_rows`
	  - `sub_7B31` -> `gfx_blit_sprite_masked_clipped_active_page`
	  - `sub_7B8B` -> `gfx_blit_masked_plane_aligned_clipped`
	  - `sub_7BEF` -> `gfx_blit_masked_plane_shift4_clipped`
	  - `sub_7C79` -> `gfx_blit_sprite_masked_bottom_clipped_active_page`
	  - `sub_7CCD` -> `gfx_blit_masked_plane_aligned_bottom_clipped`
	  - `sub_7D31` -> `gfx_blit_masked_plane_shift4_bottom_clipped`
	  - `sub_3A5F` -> `hud_draw_two_digit_counter`
	  - `sub_45A6` -> `hud_increment_lives_bcd_clamped`
	  - `sub_4870` -> `hud_set_data_disk_icon_collected`
	  - `sub_489E` -> `hud_bcd_decrement_cx`
	  - `sub_48AA` -> `hud_decrement_word_25202_bcd`
	  - `sub_48BF` -> `hud_decrement_lives_bcd`
	  - `sub_48D4` -> `ent_normalize_runtime_y_magnitudes`
	  - `sub_4A70` -> `ent_copy_descriptor_to_runtime_slot`
	  - `sub_4C06` -> `ent_deactivate_runtime_slot`
	  - `sub_49AD` -> `ent_build_runtime_slots_for_viewport`
	  - `sub_5A4E` -> `ent_move_x_and_bounce`
	  - `sub_5A99` -> `ent_apply_gravity_and_move_y`
	  - `sub_5B01` -> `ent_move_y_and_bounce`
	  - `sub_5B7B` -> `ent_move_y_with_probe_and_bounce`
	  - `sub_5BAC` -> `ent_move_x_with_range_probe_and_bounce`
	  - `sub_5BF7` -> `ent_probe_x_collision_strict`
	  - `sub_5C3E` -> `ent_probe_x_collision_range`
	  - `sub_5C91` -> `ent_probe_tile_above_threshold`
	  - `sub_5CA1` -> `ent_probe_tile_range_in_viewport`
	  - `sub_5CCF` -> `ent_set_horizontal_chase_direction_to_comic`
	  - `sub_5CEB` -> `spawn_player_projectile`
	  - `sub_5F86` -> `ui_draw_menu_frame`
	  - `sub_198E` -> `load_room_effect_script_and_trigger_ranges`
	  - `sub_1BFB` -> `check_word_is_ffff_or_0005`
	  - `sub_1C0B` -> `update_level5_memory_signature`
	  - `sub_1FA4` -> `io_load_rle_resource_to_e978`
	  - `sub_1FFE` -> `io_read_file_to_seg001_0600_resource`
	  - `sub_2022` -> `io_read_file_to_seg5e_base`
	  - `sub_2049` -> `io_read_xor25_block_9ec0`
	  - `sub_2072` -> `load_room_tilemap_from_resource_buffer`
	  - `sub_1E09` -> `gfx_draw_tile_id_at_pixel_xy`
	  - `sub_1E86` -> `gfx_render_room_tilemap_to_active_buffers`
	  - `sub_1F72` -> `gfx_copy_16x16_tile_plane_to_active_page`
	  - `sub_1F89` -> `gfx_copy_16x16_tile_plane_to_segment`
	  - `sub_22B` -> `gfx_disable_retrace_palette_script`
	  - `sub_238` -> `gfx_enable_retrace_palette_script`
	  - `sub_15D` -> `gfx_load_ega_palette_16_from_table`
	  - `sub_599` -> `input_wait_for_scancode_event`
	  - `sub_424` -> `input_wait_for_joystick_or_key_confirm`
	  - `sub_54B` -> `input_capture_unique_key_binding_and_echo`
	  - `sub_2B8E` -> `camera_update_y_follow_comic_clamped`
	  - `sub_327D` -> `player_cycle_mode_selection`
	  - `sub_30C6` -> `player_start_state4_action_animation`
	  - `sub_3468` -> `player_start_attack_animation`
	  - `sub_3610` -> `player_start_timed_overlay_sprite`
	  - `sub_362A` -> `check_comic_near_room_event_anchor`
	  - `sub_36C0` -> `update_room_event_anchor_motion`
	  - `sub_378E` -> `update_room_event_anchor_sprite`
	  - `sub_6806` -> `ent_process_interaction_script_and_queue_message`
	  - `sub_6D95` -> `hud_add_bcd_counter_from_tens_value`
	  - `sub_71FE` -> `intro_repeat_draw_with_offset_4_3`
	  - `sub_721B` -> `intro_repeat_draw_with_offset_8_6`
	  - `sub_7238` -> `intro_repeat_draw_with_offset_12_8`
	- Item-specific handlers isolated:
	  - `handle_projectile_impact`
	  - `tile_handle_collision_mode2_trigger`
	- Explicit spawn/respawn paths isolated:
	  - `ent_activate_slot_into_runtime`
	  - `ent_build_runtime_slots_for_viewport`
	  - `spawn_player_projectile`
4. [x] Trace DOS `int 21h` / `3D00h` open-file call sites from `start` to locate resource loaders
	- Startup/open-file chain confirmed:
	  - `io_verify_game000_or_prompt_retry`
	  - `io_verify_frcfg_or_prompt_retry`
	  - `io_open_file_or_prompt_retry`
	- Resource loader wrappers confirmed in active paths:
	  - `io_load_rle_resource_to_e978`
	  - `io_read_file_to_seg001_0600_resource`
	  - `io_read_file_to_seg5e_base`
	  - `io_read_xor25_block_9ec0`
	  - `io_read_file_to_seg001_0600`
5. [x] Trace INT 3 handler entry and initial sound effect table anchors
	- Handler entry confirmed: `loc_8C7` (installed by `sub_9C9`, restored by `sub_A5E`)
	- Timer playback loop confirmed: `loc_683` (PIT ch2 + speaker gate control)
	- Verified stream pointers in use: `0x00DB`, `0x965E`, `0x9676`, `0x96B6`
	- Canonical naming for every effect stream offset can be done as a follow-up refinement pass

---

## Phase 6: Data Structure & Resource Format Documentation (COMPLETED)

### Goals
- Confirm full enemy struct layout (12-byte format, all offsets)
- Document FR*.* level file format
- Document frpak.* archive format
- Document .SHP and .EGA sprite/graphics layouts
- Write C-equivalent `struct` definitions for all confirmed layouts

### Progress (Current)
- [x] Confirmed 12-byte mapped object record at `unk_25AD0` (`+0x00..+0x0A`)
- [x] Confirmed active runtime entity list format (8-byte records at `unk_256E0`)
- [x] Confirmed 0x20-byte runtime behavior slot layout (table at `0x48ED`)
- [x] Documented signed-RLE and 4-plane EGA packet decode contracts from code paths
- [x] Confirmed `unk_2E3CC` level loader indirection (per-level pointer -> 4 filename-pointer tuple)
- [x] FRDATA header/room-offset schema documented with field-level semantics used by loader
- [x] FRPAK `.001`..`.007` validated as direct full-file 4-plane streams (no directory walk in confirmed paths)
- [x] `.SHP` / `.EGA`-equivalent payload contracts finalized from sprite and RLE blit routines

---

## Phase 7: C++ Reimplementation (STARTED)

### Goals
- Platform abstraction layer (EGA rendering → SDL2 or similar)
- Port confirmed structs and globals to C++
- Reimplement game loop / dispatcher
- Reimplement physics, movement, entity system
- Behavioral verification against original via DOSBox oracle

### Progress (Current)
- [x] Created initial C++ scaffold under `reimpl/` with CMake build
- [x] Ported confirmed Phase 6 data layouts to `reimpl/include/comic2/types.hpp`
- [x] Implemented signed-RLE and 4-plane EGA packet decoders in `reimpl/src/resource_loader.cpp`
- [x] Added FRPAK validator executable (`reimpl/src/main.cpp`)
- [x] Verified build succeeds and FRPAK.001..FRPAK.007 decode with row span `0x1f40`
- [x] Added renderer abstraction baseline (`reimpl/include/comic2/renderer.hpp`, `reimpl/src/renderer.cpp`) with tests
- [x] Ported dispatcher skeleton and player/runtime state containers (`reimpl/include/comic2/dispatcher.hpp`, `reimpl/include/comic2/game_state.hpp`, `reimpl/src/dispatcher.cpp`) with tests
- [x] Added default input + physics stage handlers and deterministic tick replay tests (`reimpl/include/comic2/default_handlers.hpp`, `reimpl/src/default_handlers.cpp`, `reimpl/tests/dispatcher_tests.cpp`)
- [x] Started subsystem split with dedicated `player_controller` + `tile_collision` modules and tests (`reimpl/include/comic2/player_controller.hpp`, `reimpl/src/player_controller.cpp`, `reimpl/include/comic2/tile_collision.hpp`, `reimpl/src/tile_collision.cpp`, `reimpl/tests/player_controller_tests.cpp`)
- [x] Added scaffold baselines for `entity_runtime`, `projectiles`, and `room_loader` modules with focused tests (`reimpl/include/comic2/entity_runtime.hpp`, `reimpl/src/entity_runtime.cpp`, `reimpl/include/comic2/projectiles.hpp`, `reimpl/src/projectiles.cpp`, `reimpl/include/comic2/room_loader.hpp`, `reimpl/src/room_loader.cpp`, `reimpl/tests/subsystem_scaffold_tests.cpp`)

### Detailed Reimplementation Roadmap

#### 7.1 Architecture and Module Boundaries
- [x] `resource_loader` module: signed-RLE + 4-plane packet decode contracts
- [x] `renderer` module baseline: planar surface abstraction + presenter interface
- [x] `dispatcher` module baseline: stage priority selector + hookable tick execution
- [x] `game_state` baseline: player/input/flags/runtime containers
- [x] Split runtime into explicit subsystem modules:
	- [x] `player_controller` (movement/jump/fall/state transitions)
	- [x] `tile_collision` (tile lookup + threshold checks)
	- [x] `entity_runtime` scaffold (activation/deactivation/runtime slot updates)
	- [x] `projectiles` scaffold (spawn/update/collision/despawn)
	- [x] `room_loader` scaffold (load_resource equivalent, room decode and setup)

#### 7.2 Dispatcher Parity Plan (`loc_2341` Ordering)
- [x] Encoded current stage order and priority chain in `GameDispatcher`
- [x] Add dedicated stage handlers matching known flow order:
	1. `handle_level_transition`
	2. `handle_special_logic_1`
	3. `handle_special_logic_2`
	4. `handle_airborne_movement`
	5. `handle_timed_overlay`
	6. `update_player_physics` (grounded)
	7. `handle_player_animation`
	8. `handle_attack_animation`
	9. `handle_distance_interaction`
	10. `handle_tile_hazard`
	11. `handle_player_special_state`
	12. `handle_input_fallback`
- [x] Add a trace-mode dispatcher logger that records selected stage per tick for oracle comparison

#### 7.3 Player Movement and Physics Plan
- [ ] Implement horizontal movement parity for known left/right paths (`sub_2BDC`, `sub_2C39`)
- [ ] Implement grounded physics (`sub_2A10`) including jump counter consumption and fall start
- [ ] Implement airborne path (`sub_2EDC`) including lateral drift checks and vertical velocity updates
- [ ] Implement floor check trigger (`sub_2C9D`) to enter fall state when no support tile
- [ ] Add deterministic fixtures for edge cases:
	- jump while moving left/right
	- short-hop vs full jump counter usage
	- ledge walk-off transitioning into fall

#### 7.4 Tile Collision and Room Grid Plan
- [ ] Implement tile query service equivalent to `get_tile_at_pixels` (`sub_1CFE`)
- [ ] Port documented FRDATA room entry interpretation (`tile_w`, `tile_h`, `rle_data_off`)
- [ ] Build room row-pointer table equivalent (`0x2A0` pointer map behavior)
- [ ] Implement collision thresholds and hazard band checks used by dispatcher/hazard path
- [ ] Add tests for pixel->tile mapping and threshold comparisons at room bounds

#### 7.5 Entity and Projectile Plan
- [ ] Implement mapped-object activation pipeline:
	- room list build (`ent_build_room_entity_list`)
	- runtime slot build (`ent_build_runtime_slots_for_viewport`)
	- slot copy/deactivate behavior (`ent_copy_descriptor_to_runtime_slot`, `ent_deactivate_runtime_slot`)
- [ ] Implement projectile loop parity (`update_projectiles`) with viewport culling and tile collision
- [ ] Implement player projectile spawn (`spawn_player_projectile`) and impact dispatch
- [ ] Add table-driven tests for runtime slot state transitions and projectile despawn conditions

#### 7.6 Rendering Plan (EGA-Parity-Oriented)
- [ ] Implement masked and opaque sprite blit adapters over `EgaPlanarSurface`
- [ ] Implement page-flip abstraction compatible with current double-buffer assumptions
- [ ] Add validation helpers that compare plane bytes for known decode/blit fixtures
- [ ] Add optional SDL2 presenter behind `IFramePresenter` while keeping core logic headless-testable

#### 7.7 Integration Gates and Oracle Verification
- [ ] Gate A (Unit): all module tests pass in CI (`ctest`) with deterministic replay checks
- [ ] Gate B (Frame): scripted input sequences produce stable dispatcher-stage traces
- [ ] Gate C (State): key state vectors (`x/y/vel/state flags`) match expected snapshots per tick
- [ ] Gate D (Visual): selected rooms produce matching planar frame hashes against oracle captures
- [ ] Gate E (Behavior): death/hazard, room transition, and projectile interactions match expected outcomes

#### 7.8 Immediate Next Execution Order
- [ ] 1) Implement `tile_collision` service and replace physics stubs with tile-aware checks
- [ ] 2) Implement grounded + airborne handlers using documented priority and state flags
- [ ] 3) Implement room-loader integration (`load_resource` equivalent path) into runtime state
- [ ] 4) Implement projectile update path and add deterministic scripted tick tests
- [ ] 5) Add dispatcher trace capture and compare against oracle-driven expected stage sequences