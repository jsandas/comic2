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

## Phase 5: Complete Function Annotation (current)

### Goals
- Apply all confirmed `sub_XXXX` renames in `comic2.asm`
- Annotate the EGA blit pipeline (`gfx_startup_graphics_and_menu_probe`, `gfx_render_viewport_4plane`, `gfx_copy_viewport_plane`, `ent_draw_mapped_slot_or_placeholder`, `gfx_rle_blit_opaque_4plane`, `gfx_rle_blit_masked_or_4plane`, `gfx_set_crtc_start_on_retrace`)
- Locate and annotate entity management (`handle_enemies`, `handle_fireballs`, `handle_item`, `spawn_enemy`)
- Locate and annotate file I/O / resource loading (`load_level`, `load_fullscreen_graphic`, `load_shp_files`, `rle_decode`)
- Identify sound system: INT 3 handler, sound effect dispatch, effect addresses

### Steps
1. [x] Batch-rename all confirmed `sub_XXXX` labels with known identities
2. [x] Annotate EGA blit pipeline with full function headers (`gfx_startup_graphics_and_menu_probe`, `gfx_render_viewport_4plane`, `gfx_copy_viewport_plane`, `ent_draw_mapped_slot_or_placeholder`, `gfx_rle_blit_opaque_4plane`, `gfx_rle_blit_masked_or_4plane`, `gfx_set_crtc_start_on_retrace`)
3. [ ] Trace `sub_437B` and `sub_5D5F` callers to locate entity management (in progress)
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
	- Remaining: isolate item-specific handler(s) and explicit spawn/respawn routine(s)
4. [ ] Trace DOS `int 21h` / `3D00h` open-file call sites from `start` to locate resource loaders
5. [x] Trace INT 3 handler entry and initial sound effect table anchors
	- Handler entry confirmed: `loc_8C7` (installed by `sub_9C9`, restored by `sub_A5E`)
	- Timer playback loop confirmed: `loc_683` (PIT ch2 + speaker gate control)
	- Verified stream pointers in use: `0x00DB`, `0x965E`, `0x9676`, `0x96B6`
	- Remaining: assign canonical effect names to all stream offsets

---

## Phase 6: Data Structure & Resource Format Documentation

### Goals
- Confirm full enemy struct layout (12-byte format, all offsets)
- Document FR*.* level file format
- Document frpak.* archive format
- Document .SHP and .EGA sprite/graphics layouts
- Write C-equivalent `struct` definitions for all confirmed layouts

---

## Phase 7: C++ Reimplementation

### Goals
- Platform abstraction layer (EGA rendering → SDL2 or similar)
- Port confirmed structs and globals to C++
- Reimplement game loop / dispatcher
- Reimplement physics, movement, entity system
- Behavioral verification against original via DOSBox oracle