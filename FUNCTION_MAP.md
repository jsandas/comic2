# Captain Comic 2 Function Map
# Cross-reference between disassembly addresses and identified functions

## Core System Functions

### Initialization and Setup
```
start (Line 71)             → main entry point, game initialization
sub_9C9 (Line 1427)         → install_interrupt_handlers
sub_A5E (Line 1482)         → restore_interrupt_handlers  
sub_27A (Line 399)          → wait_n_ticks(ax) - waits for game ticks
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
sub_96F (Line 1356)         → read_joystick_axis - analog stick reading
sub_9AF (Line 1407)         → pit_count - get PIT counter for timing

Key State Variables:
byte_245 / game_tick_flag   → Set to 1 by INT 08h each tick
byte_251-254                → Function key scancodes (F1-F4: 3Bh,3Ch,3Dh,3Eh)
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
sub_2B4 (Line 417)          → Main graphics/blitting function
sub_451C (Line 7443)        → Level/sprite rendering system
sub_79C7                    → EGA plane blit helper
sub_7A13                    → EGA plane blit with mask

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
sub_35DE (Line 5534)        → Main game loop (FRAGMENTED function)
                              - 7 non-contiguous code chunks
                              - Similar to Comic 1's game_loop

Loop Structure (Comic 1 pattern):
1. Wait for game tick flag
2. Handle input (keyboard/joystick)
3. Update player physics (gravity, jumping, movement)
4. Update enemies AI
5. Handle fireballs/projectiles
6. Check collisions
7. Update items/pickups
8. Render frame to offscreen buffer
9. Swap video buffers
10. Repeat
```

### Player Character
```
Expected Comic 1 patterns (need to locate exact addresses):

comic_y                     → Y position (game units)
comic_x                     → X position (game units)
comic_facing                → Direction (0=right, 5=left)
comic_animation             → Animation frame (0=stand, 1-3=run, 4=jump)
comic_hp                    → Health points
comic_num_lives             → Lives remaining
comic_x_momentum            → Horizontal velocity (-5 to +5)
comic_y_vel                 → Vertical velocity (fixed-point 1/8 units)
comic_jump_counter          → Jump power countdown
comic_firepower             → Blastola Colas collected (max 5)

Movement Functions (to locate):
- face_or_move_left
- face_or_move_right
- handle_fall_or_jump       → Physics: gravity, terminal velocity, ceiling collision
- move_left                 → Left movement with collision
- move_right                → Right movement with collision
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
Expected Comic 1 pattern:

INT 3 Handler               → Sound control (priority queue)
  Operation codes:
  AL=0 → SOUND_UNMUTE
  AL=1 → SOUND_PLAY (DX:BX=sound data, CL=priority)
  AL=2 → SOUND_MUTE
  AL=3 → SOUND_STOP
  AL=4 → SOUND_QUERY (returns AL=1 if playing)

PIT Programming:
- Port 0x43 → Command register
- Port 0x40-0x42 → Counter channels
- Port 0x61 → PC speaker enable/disable
- Frequency = 1,193,182 / divisor

Sound Effects (to locate):
- SOUND_JUMP
- SOUND_FIRE  
- SOUND_HIT_ENEMY
- SOUND_COLLECT_ITEM
- SOUND_DAMAGE
- SOUND_DEATH
- SOUND_EXTRA_LIFE
- SOUND_GAME_OVER
```

## Data Segments

### Segment Overview
```
seg000 (CODE)               → Lines 32-16449 - Game logic
seg001 (STRINGS)            → Lines 16449-48522 - Messages, prompts
seg002 (STACK)              → Lines 48522+ - Stack space
seg003 (DATA)               → Lines 48530-97625 - Game state variables
seg004 (GRAPHICS)           → Lines 97625-109792 - Palette, sprite data
seg005 (DATA)               → Lines 109792-153580 - More game data
seg006-seg009               → Additional segments
```

### Important String Data (seg001)
```
Line 16449+:
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
