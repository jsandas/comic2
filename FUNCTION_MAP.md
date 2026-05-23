# Captain Comic 2 Function Map
# Cross-reference between disassembly addresses and identified functions

## Core System Functions

### Initialization and Setup
```
start (Line 68)             → main entry point, game initialization
sub_9C9 (Line 1464)         → install_interrupt_handlers
sub_A5E (Line 1519)         → restore_interrupt_handlers  
sub_27A (Line 435)          → wait_n_ticks(ax) - waits for game ticks
```

### Interrupt Handlers  
```
INT 08h Handler             → Timer interrupt (18.2 Hz game tick)
INT 09h Handler             → Keyboard scancode handler
INT 1Ch Handler             → User timer callback
INT 3 Handler               → Sound system control (priority queue)
```

### Input System
```
sub_96F (Line 1393)         → read_joystick_axis - analog stick reading
sub_9AF (Line 1444)         → pit_count - get PIT counter for timing

Key State Variables:
byte_245 / game_tick_flag   → Set to 1 by INT 08h each tick
byte_251-254                → Function key scancodes (F1-F4: 3Bh,3Ch,3Dh,3Eh)
byte_258 key_state_left     → 1 if Left arrow pressed
byte_259 key_state_right    → 1 if Right arrow pressed
byte_25A key_state_up       → 1 if Up arrow pressed
byte_25B key_state_down     → 1 if Down arrow pressed
byte_25C key_state_jump     → 1 if Space/Jump pressed
byte_25D key_state_fire     → 1 if R/Fire pressed
byte_25E scancode_left      → Left arrow (4Bh)
byte_25F scancode_right     → Right arrow (4Dh)
byte_260 scancode_up        → Up arrow (48h)
byte_261 scancode_down      → Down arrow (50h)
byte_262 scancode_jump      → Space bar (39h)
byte_263 scancode_fire      → 'R' key (52h)
```

## Graphics Functions

### EGA Video Operations
```
gfx_startup_graphics_and_menu_probe (was sub_2B4)   → Startup/title-screen graphics prep + resource probe
gfx_render_viewport_4plane (was sub_1D2C)           → Viewport row setup + 4-plane dispatch
gfx_copy_viewport_plane (was sub_1DC0)              → Per-plane viewport copy to A000h
ent_draw_mapped_slot_or_placeholder (was sub_451C)  → Object slot resolve + mapped sprite/placeholder draw
gfx_rle_blit_opaque_4plane (was sub_79C7)           → 4-plane opaque RLE blit helper
gfx_rle_blit_masked_or_4plane (was sub_7A13)        → 4-plane masked RLE OR-blit helper
gfx_set_crtc_start_on_retrace (was sub_7A89)        → CRTC start update synchronized to retrace
sprite_frame_table_offset (was sub_3DC3)            → Frame index to 128-byte sprite table offset
viewport_xy_to_di (was sub_3DD7)                    → Viewport-relative X/Y to screen DI offset
draw_player_sprite (was sub_3DFB)                   → Draw facing-specific player sprite at comic_x/comic_y
room_transition_draw_header (was sub_3BC6)          → Render transition header overlay and settle one frame
room_transition_reveal_sequence_a (was sub_3C13)    → Multi-step room reveal sequence (variant A)
room_transition_reveal_sequence_b (was sub_3CB1)    → Multi-step room reveal sequence (variant B)
room_transition_palette_wave (was sub_3F0A)         → Palette/sprite wave sweep during room transition intro
room_transition_draw_reveal_quad (was sub_3D3A)     → Draw a 2x2 reveal quad from transition frame indices
room_transition_blit_reveal_quad_4plane (was sub_3D7B) → Blit one reveal quad tile across all four EGA planes
room_transition_blit_reveal_column_plane (was sub_3D90) → Per-plane reveal column blit (16 rows)
room_transition_clear_reveal_box (was sub_3DA1)     → Clear 0x20x0x20 reveal box at transition-relative coordinates
room_transition_present_frame (was sub_3DB6)        → Update visible entities, flush transition frame, and tick-wait
room_transition_player_entry_sequence (was sub_3E1F) → Transition intro with staged player-entry sprite sweep
room_transition_player_exit_sequence (was sub_3E98)  → Transition outro with staged player-exit sprite sweep
event_draw_stacked_sprite_rise (was sub_401B)        → Draw rising stacked sprites during event/object resolution
event_transition_camera_bob_step (was sub_4073)      → Single camera bob step with viewport redraw/update during transition
event_finale_transition_sequence (was sub_409C)      → Finale transition sequence with repeated sprite-rise passes and room load handoff
ui_draw_framed_message_from_stream (was sub_4206)    → Draw framed message/panel from encoded width/height + content stream
ui_draw_string_8x8 (was sub_42D4)                    → Draw NUL-terminated 8x8 UI string using current cell cursor
ui_draw_glyph_8x8 (was sub_42E8)                     → Draw one 8x8 UI glyph and advance cursor by one cell
ega_set_map_mask (was sub_4339)                      → Set EGA Sequencer Map Mask (port 3C4h index 2)
ui_show_modal_prompt_wait_key (was sub_39AD)         → Show framed modal prompt and wait for acknowledgement/confirm key
hud_update_mode_icons (was sub_3A17)                 → Update paired HUD mode/status icons from current mode index
io_open_file_or_prompt_retry (was sub_660B)          → Attempt file open; on failure show prompt and retry loop
ui_select_next_valid_option (was sub_6011)           → Advance to next selectable option index
ui_select_prev_valid_option (was sub_6021)           → Move to previous selectable option index
ui_is_option_selectable (was sub_6030)               → Test whether option index is currently selectable
ui_render_option_list (was sub_606B)                 → Draw option list panel with current selection highlight
ui_option_list_input_loop (was sub_615C)             → Handle option-list navigation/confirm/cancel input loop
ui_edit_selected_option_text (was sub_61C8)          → Edit selected option text field (inline keyboard entry)
cfg_load_options_or_defaults (was sub_62A4)          → Load options/config table from disk or initialize defaults
cfg_save_options (was sub_631A)                      → Save options/config table to disk
savegame_write_snapshot (was sub_633C)               → Write runtime snapshot to selected savegame file
savegame_read_snapshot (was sub_636F)                → Read runtime snapshot from selected savegame file (with header check)
ui_draw_completion_delta_hint (was sub_64F5)         → Draw completion/collection delta hint for current selection
cfg_compute_hardware_seed (was sub_6545)             → Compute hardware/time-derived seed used by settings/save flow
io_verify_game000_or_prompt_retry (was sub_65EF)     → Verify GAME.000 availability; on failure show prompt/retry loop
io_verify_frcfg_or_prompt_retry (was sub_65FD)       → Verify FR.CFG availability; on failure show prompt/retry loop
ui_render_game_selection_panel (was sub_677A)        → Render game-selection panel entries and current highlight
cfg_run_integrity_challenge (was sub_6DF4)           → Run startup config integrity challenge/update flow
hud_add_bcd_counter_from_low_digit (was sub_6D99)    → Add value into HUD BCD counter chain starting at lowest digit pair
hud_add_bcd_counter_from_mid_digit (was sub_6DE7)    → Add value into HUD BCD counter chain starting at middle digit pair
cfg_input_challenge_code (was sub_7057)              → Input challenge code digits during config integrity flow
cfg_draw_integrity_challenge_screen (was sub_6F47)   → Draw integrity challenge screen frame, labels, and seed/code preview
play_intro_cinematic (was sub_7164)                  → Play scripted intro cinematic sequence and hand off to menu flow
gfx_copy_backbuffer_to_active_page (was sub_7255)    → Copy 6000h backbuffer into current active display page
intro_draw_sprite_and_present (was sub_7261)         → Draw intro sprite frame, present, and advance one tick
wait_ticks_or_abort (was sub_7278)                   → Wait N ticks unless abort key/event requests DOS exit

Key Patterns:
- Port 0x3C4/0x3C5 → Sequencer (Map Mask for write plane selection)
- Port 0x3CE/0x3CF → Graphics Controller (Read Map Select)
- Port 0x3DA → Input Status Register (vertical retrace check)
- Video Buffer 0x0000 → Onscreen buffer
- Video Buffer 0x2000 → Offscreen rendering buffer
```

### Palette and Mode
```
INT 10h, AX=000Dh           → Set EGA 320×200 16-color mode
INT 10h, AX=0F00h           → Get current video mode
INT 10h, AX=1000h           → Set palette register
```

## Game Logic

### Main Game Loop
```
game_loop (was sub_35DE)    → Main game loop (FRAGMENTED function)
                              - Fragment 1: 3167 - 4276 (Dispatcher, Physics, Trigger logic)
                              - Fragment 2: 4598 - 4823 (Player Death, Airborne movement)
                              - Fragment 3: 4989 - 5043 (Player Animation)
                              - Fragment 4: 5240 - 5380 (Player State Check)
                              - Fragment 5: 5410 - 5555 (Attack Animation)
                              - Fragment 6: 5653 - 5694 (Special Logic 1)
                              - Fragment 7: 5872 - 6004 (Special Logic 2, Distance-based logic)

Loop Structure:
1. Wait for game tick: call sub_27A (wait_n_ticks)
2. Clear BIOS keyboard buffer: loc_2341 (clears 0040:001A)
3. Dispatch state handlers: loc_275C, handle_airborne_movement, update_player_physics, handle_player_animation, handle_attack_animation, etc.
4. Input handling: loc_2341 block (if no state active)
5. Update projectiles: update_projectiles (Projectile Update Loop - movement, collision, and interaction)
6. Scan viewport entities/interactions: ent_update_entities_in_viewport
7. Draw sprites: draw_sprite
8. Repeat
```

### Player Character
```
comic_x                     → ds:6C6h / word_256A6 (Line 111536) - X position
comic_y                     → ds:6C8h / word_256A8 (Line 111537) - Y position
comic_x_vel                 → ds:892h / word_25872 (Line 111984) - Horizontal velocity
comic_y_vel                 → ds:894h / word_25874 (Line 111986) - Vertical velocity
comic_state                 → ds:8AEh / byte_2588E (Line 112006) - 0=idle, 1=move, 2=jump/fall, 3=die, 6=attack
comic_is_physics_active     → ds:897h / byte_25877 (Line 111989)
comic_jump_counter          → ds:898h / byte_25878 (Line 111990) - Ticks remaining in jump
comic_is_animation_active   → ds:8A3h / byte_25883 (Line 111999)
comic_is_attack_active      → ds:8A7h / byte_25887 (Line 112003)
comic_facing                → ds:8ADh / byte_2588D (Line 112005) - 1=right, 2=left
comic_hp                    → ds:8EDh / byte_258CD (Line 112068) - Health points (max 12)
comic_num_lives             → ds:28Fh / word_2526F (Line 110468) - Remaining lives
comic_firepower             → ds:1FCh / byte_251FC (Line 110355) - Firepower level (max 6, active shots)
comic_gems                  → ds:204h / word_25204 (Line 110362) - Gem count (Extra life at 100)
comic_score_low             → ds:202h / word_25202 (Line 110361) - Partial score/treasure
```

### HUD Meter Helpers
```
byte_258CA / byte_258CB     → Paired HUD animation counters for glyph-based meter effects
byte_258CC                  → HUD animation lockout / flash timer gate

hud_c_meter_reset (was sub_3A92)         → Reset C-glyph meter state and redraw baseline
hud_star_meter_reset (was sub_3AA0)      → Reset *-glyph meter state and redraw baseline
hud_star_meter_step_up (was sub_3ACE)    → Increment *-glyph meter animation one step
hud_star_meter_step_down (was sub_3B12)  → Decrement *-glyph meter animation one step
hud_c_meter_step_up (was sub_3B51)       → Increment C-glyph meter animation one step
hud_c_meter_step_down (was sub_3B8C)     → Decrement C-glyph meter animation one step
```

Movement and Collision Functions:
- sub_2BDC                 → move_player_left (Line 4335)
- sub_2C39                 → move_player_right (Line 4363)
- sub_2C9D                 → check_floor_collision (Line 4421)
- move_left_step_grounded (was sub_2BF7)      → Apply grounded left movement with collision and viewport scroll
- move_right_step_grounded (was sub_2C54)     → Apply grounded right movement with collision and viewport scroll
- move_left_step_airborne (was sub_3026)      → Apply airborne left drift with collision and viewport scroll
- move_right_step_airborne (was sub_305E)     → Apply airborne right drift with collision and viewport scroll
- check_airborne_side_clear (was sub_309D)    → Test lateral tile clearance for airborne horizontal movement
- tile_check_horizontal_step_clear (was sub_2D61)         → Check horizontal step clearance against tile/blocking thresholds
- tile_handle_collision_mode2_trigger (was sub_2D31)      → Handle collision-trigger table for mode-2 floor-check context
- check_airborne_path_clear (was sub_2FEE)     → Check airborne path against tile/ceiling boundaries
- check_airborne_tile_threshold (was sub_3019) → Compare airborne tile against blocking threshold
- sub_1CFE                 → get_tile_at_pixels (Line 2544) - AX=x, BX=y, returns tile type in AX
- sub_18E0                 → setup_render_viewport (Line 2179)
- sub_1B8D                 → load_resource (Line 2337)
```

### Entity Management
```
Enemy Runtime Structure (8-byte active list at unk_256E0):
Offset  Size    Description
+0x00   word    X position (screen/world-relative in current room context)
+0x02   word    Y position
+0x04   word    Runtime flags / mapped object index (bit 8000h used as state)
+0x06   word    Sprite/object pointer/id used by draw path

Mapped Object Structure (12-byte source table at unk_25AD0):
Offset  Size    Description
+0x00   word    Room X index
+0x02   word    Room Y index
+0x04   word    Object X
+0x06   word    Object Y
+0x08   word    Sprite/object pointer
+0x0A   word    Flags (bit 8000h propagated into runtime flags)

Identified Functions:
- ent_build_room_entity_list (was sub_1A63)         → Build active room entity list from mapped object table
- ent_update_entities_in_viewport (was sub_437B)    → Scan active entities, draw visible ones, handle proximity interactions
- ent_update_object_behaviors (was sub_4B25)        → Tick/update mapped world object behaviors and activation states
- ent_spawn_transient_slot (was sub_3181)           → Spawn a transient runtime slot and register it in the active list
- ent_update_transient_slots (was sub_31EB)         → Update/transient-slot animation, draw, and cleanup loop
- update_projectiles (was sub_5D5F)                 → Update projectile movement/collision and impact effects
- ent_deactivate_at_coords (was sub_4579)           → Clear active entity flag by exact (x,y) match
- ent_activate_slot_into_runtime (was sub_46E6)     → Move mapped object slot into active runtime list near player interaction

Still to promote with higher confidence:
- handle_enemies            → Confirm split between mapped-object behavior tick and active-runtime interaction tick
- handle_item               → Item-specific animation/collision dispatcher
- spawn_enemy               → Respawn/path-dependent enemy creation routine(s)
```

## File I/O

### Resource Loading
```
Pattern (INT 21h DOS calls):
AX=3D00h                    → Open file (read-only)
AX=3F00h                    → Read from file
AX=3E00h                    → Close file

Resource Files:
- data.0, data.1            → Graphics/sprite data
- frdata.0, frdata.1        → Level layout data
- frdemo.0, frdemo.1, frdemo.2 → Demo sequences
- frpak.003 - frpak.007     → Packed resource archives

Functions (to locate):
- load_level                → Load .PT/.TT2 map files
- load_fullscreen_graphic   → Load .EGA graphics
- load_shp_files            → Load .SHP sprite animations
- rle_decode                → Decompress RLE graphics
```

## Sound System

### PC Speaker Sound
```
Confirmed in comic2.asm:

INT 3 vector install:
- sub_9C9 writes vector 03h to loc_8C7
- sub_A5E restores original vector on shutdown

INT 3 command handler (loc_8C7):
- AL=0 → speaker gate ON mode (byte_248=1), then speaker output forced OFF until playback enables it
- AL=1 → queue/play request if CL priority >= byte_248+1
         stores sound stream pointer in byte_248+2 and source segment in byte_248+6
- AL=2 → speaker gate OFF mode (byte_248=0), output forced OFF
- AL=3 → hard stop (byte_247=0, byte_248+1=0), output forced OFF
- AL=4 → query playback active flag (returns AL=byte_247)

Timer-driven playback (INT 08h hook loc_683):
- When byte_247=1, consumes word stream from ES:[byte_248+2]
- Stream format appears to be repeating pairs:
  word pitch_divisor, word tick_duration
- pitch_divisor=0 terminates playback (byte_247=0)
- Non-zero divisor programs PIT channel 2 via ports 43h/42h
- Speaker enable/disable uses port 61h bits 0-1

PIT Programming:
- Port 0x43 → Command register
- Port 0x40-0x42 → Counter channels
- Port 0x61 → PC speaker enable/disable
- Frequency = 1,193,182 / divisor

Known effect stream pointers at call sites (AX=1, CX=priority, INT 3):
- BX=00DBh (used during intro/startup)
- BX=965Eh (damage/hazard path at loc_2DC6)
- BX=9676h (jump/airborne transitions at loc_2E4F and loc_2E96)
- BX=96B6h (intro/startup)

Confidence:
- INT 3 command semantics: High
- Stream pair format (divisor,duration): High
- Effect-to-event names beyond listed call sites: Medium/Low
```

## Data Segments

### Segment Overview
```
seg000 (CODE)               → Lines 32-16486 - Game logic
seg001 (STRINGS)            → Lines 16486-48559 - Messages, prompts
seg002 (STACK)              → Lines 48559-48567 - Stack space
seg003 (DATA)               → Lines 48567-97662 - Game state variables
seg004 (GRAPHICS)           → Lines 97662-109829 - Palette, sprite data
seg005 (DATA)               → Lines 109829-153617 - More game data
seg006-seg009               → Additional segments
```

### Important String Data (seg001)
```
Line 16486+:
- "Calibrate Joystick"
- "Center Joystick and Press Button"
- "Move Left  : "
- "Move Right : "
- "Jump       : "
- "Trigger    : "

Line 149305:
- "  G A M E  S E L E C T I O N  "
- "       SAVE GAME "
```

## Assembly Patterns to Search For

### EGA Plane Selection (Write)
```asm
mov dx, 0x3C4      ; Sequencer Index register
mov al, 2          ; Map Mask register number
out dx, al
inc dx             ; Point to data register
mov al, [plane]    ; Plane mask: 1,2,4,8 for planes 0-3
out dx, al
```

### EGA Plane Selection (Read)
```asm
mov dx, 0x3CE      ; Graphics Controller Index
mov al, 4          ; Read Map Select register
out dx, al
inc dx             ; Point to data register
mov al, [plane]    ; Plane index: 0-3
out dx, al
```

### Vertical Retrace Wait
```asm
mov dx, 0x3DA      ; Input Status Register 1
.wait_loop:
  in al, dx
  test al, 8       ; Test bit 3 (vertical retrace)
  jz .wait_loop
```

### Sprite Blitting Loop (4 Planes)
```asm
mov ah, 1          ; Plane 0 mask
call blit_plane
mov ah, 2          ; Plane 1 mask
call blit_plane
mov ah, 4          ; Plane 2 mask
call blit_plane
mov ah, 8          ; Plane 3 mask
call blit_plane
```

### PIT Counter Read
```asm
mov al, 0
out 0x43, al       ; Latch counter command
in al, 0x40        ; Read low byte
mov ah, al
in al, 0x40        ; Read high byte
xchg ah, al        ; AX now has counter value
```

## Next Analysis Tasks

1. [ ] Locate main game loop exact structure
2. [ ] Find player state variable block
3. [ ] Identify enemy AI functions
4. [ ] Map collision detection code
5. [ ] Locate level data parser
6. [ ] Find sprite animation tables
7. [ ] Identify save/load game functions
8. [ ] Map sound effect data tables
9. [ ] Document level format
10. [ ] Create complete memory map

## Notes

- Comic 2 appears to be same engine as Comic 1 with enhancements
- Fragmented functions indicate complex control flow or anti-debugging
- File shows signs of being unpacked (note: "unpacked.exe")
- 13 levels vs 8 in Comic 1 (larger game)
- New save game system (not just passwords like Comic 1)
- Enhanced joystick calibration routines

---
*Generated: 2026-03-07*
*Status: Initial function mapping complete*
*Next: Deep dive into game loop and entity system*
