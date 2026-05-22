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
sub_2B4 (Line 454)          → Main graphics/blitting function
sub_1D2C (Line 2586)        → Viewport row setup + 4-plane dispatch
sub_1DC0 (Line 2666)        → Per-plane viewport copy to A000h
sub_451C (Line 7506)        → Object slot resolve + mapped sprite draw
sub_79C7                    → EGA plane blit helper
sub_7A13                    → EGA plane blit with mask
sub_7A89 (Line 13949)       → CRTC start update synchronized to retrace

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
sub_35DE (Line 5571)        → Main game loop (FRAGMENTED function)
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
3. Dispatch state handlers: loc_275C, loc_2EDC, loc_2A10, loc_3107, loc_34A9, etc.
4. Input handling: loc_2341 block (if no state active)
5. Update projectiles: sub_5D5F (Projectile Update Loop - movement, collision, and interaction)
6. Scan viewport entities/interactions: sub_437B
7. Draw sprites: sub_7DBB
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
Movement and Collision Functions:
- sub_2BDC                 → move_player_left (Line 4335)
- sub_2C39                 → move_player_right (Line 4363)
- sub_2C9D                 → check_floor_collision (Line 4421)
- sub_1CFE                 → get_tile_at_pixels (Line 2544) - AX=x, BX=y, returns tile type in AX
- sub_18E0                 → setup_render_viewport (Line 2179)
- sub_1B8D                 → load_resource (Line 2337)
```

### Entity Management
```
Enemy Structure (12-byte format from analysis):
Offset  Size    Description
+0x00   word    X position (screen-relative)
+0x02   word    Y position (screen-relative)  
+0x04   word    Sprite/resource index
+0x06   word    State flags (animated, alive, etc.)
+0x08   word    Absolute X coordinate (world space)
+0x0A   word    Absolute Y coordinate (world space)
+0x0C   ...     Collision box data

Functions (to locate):
- handle_enemies            → Update all enemy AI per tick
- handle_fireballs          → Update projectile physics
- handle_item               → Item animation and collision
- spawn_enemy               → Enemy respawn logic
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
