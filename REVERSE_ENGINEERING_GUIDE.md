# Captain Comic 2 Reverse Engineering Guide

## Project Overview

This document tracks the reverse engineering progress of Captain Comic 2: Fractured Reality.

### Source Files
- **comic2.asm** - IDA Pro 5 disassembly (186,369 lines)
- **unpacked.exe.test.c** - Ghidra C representation (9,357 lines)
- **Reference:** Captain Comic 1 disassembly from https://github.com/jsandas/comic-c

## Key Findings from Analysis

### Game Information
- **Title:** Captain Comic II - Fractured Reality, Version 1.5
- **Copyright:** Michael A. Denio, 1990
- **Architecture:** x86 16-bit real mode, large model
- **Video Mode:** 320×200, 16-color EGA (Mode 0Dh)
- **Size:** ~300 KB unpacked

### Common Patterns Between Comic 1 and Comic 2

#### 1. **Initialization Sequence**
```
- Disable PC speaker (port 0x61)
- Install interrupt handlers (INT 08h, 09h, 1Ch)
- Set EGA video mode (Mode 0Dh)
- Calibrate joystick
- Load resources
```

#### 2. **EGA Graphics Programming**
Both games use identical EGA port sequences:
- **0x3C0/0x3C1** - Attribute Controller
- **0x3C4/0x3C5** - Sequencer (plane masks)
- **0x3CE/0x3CF** - Graphics Controller
- **0x3DA** - Input Status Register (vertical retrace sync)

#### 3. **Interrupt Handlers**
- **INT 08h** - Timer (18.2 Hz) for game tick management
- **INT 09h** - Keyboard for real-time input
- **INT 1Ch** - User timer callback
- **INT 3** - Sound system control (Comic 1 pattern)

#### 4. **File I/O**
- DOS INT 21h for file operations:
  - AX=3D00h - Open file (read-only)
  - AX=3F00h - Read from file
  - AX=3E00h - Close file

#### 5. **Sound System**
- PC speaker control via PIT (Programmable Interval Timer)
- Frequency = 1,193,182 / divisor
- Priority-based sound queueing

### Entity Management

#### Player Character State (from Comic 1 patterns)
```
comic_y             - Y position (game units)
comic_x             - X position (game units)
comic_facing        - Direction (0=right, 5=left)
comic_animation     - Frame index (0=standing, 1-3=running, 4=jumping)
comic_hp            - Health points
comic_firepower     - Number of Blastola Colas collected
comic_num_lives     - Lives remaining
comic_x_momentum    - Horizontal velocity
comic_y_vel         - Vertical velocity (fixed-point 1/8 units)
comic_jump_counter  - Jump acceleration counter
```

#### Enemy Structure (12-byte format)
```
Offset  Size    Description
+0x00   word    X position (screen-relative)
+0x02   word    Y position (screen-relative)
+0x04   word    Sprite/resource index
+0x06   word    State flags
+0x08   word    Absolute X coordinate
+0x0A   word    Absolute Y coordinate
+0x0C   ...     Collision data
```

### Graphics System

#### Double Buffering
- Video buffer 0x0000 (onscreen)
- Video buffer 0x2000 (offscreen rendering)
- Swap during vertical retrace

#### Blitting Functions
Pattern from Comic 1:
```asm
blit_16xH_plane_with_mask:
    ; Enable plane
    mov dx, 0x3C4      ; SC Index register
    mov al, 2          ; Map Mask register
    out dx, al
    inc dx
    mov al, ah         ; Plane mask (1,2,4,8)
    out dx, al
    
    ; Copy with mask
    ; ... blitting loop ...
```

### Resource Files

#### Comic 2 Resources
- **data.0, data.1** - Graphics/sprite data
- **frdata.0, frdata.1** - Level data
- **frdemo.0, frdemo.1, frdemo.2** - Demo sequences
- **frpak.003 - frpak.007** - Packed resource archives

## Function Mapping Progress

### Identified Functions (Comic 2 → Comic 1 equivalent)

| Comic 2 | Comic 1 | Purpose |
|---------|---------|---------|
| `start` | `main` | Entry point, initialization |
| `sub_27A` | `wait_n_ticks` | Wait for game ticks (AX=num_ticks) |
| `sub_599` | - | Secondary wait loop (clears input) |
| `sub_9C9` | `install_interrupt_handlers` | Hook INT handlers |
| `sub_A5E` | `restore_interrupt_handlers` | Restore original handlers |
| `sub_9AF` | `pit_count` | Get timer counter |
| `sub_96F` | `read_joystick_axis` | Read analog joystick |
| `sub_2B4` | `blit_*_plane*` | Graphics blitting |
| `sub_18E0` | `setup_viewport` | Setup render viewport |
| `sub_1B8D` | `load_resource` | Load sprite/area data |
| `sub_35DE` | `game_loop` | Main game loop (FRAGMENTED) |
| `sub_1CFE` | `get_tile_attr` | Check tile collision at (AX, BX) |
| `sub_2BDC` | `move_left` | Handle left movement input |
| `sub_2C39` | `move_right` | Handle right movement input |
| `sub_5D5F` | `update_entities` | Iterate and update entity table |
| `sub_7DBB` | `draw_sprite` | Draw sprite at (AX, BX) |

### Key Code Locations in comic2.asm

| Lines | Label / Address | Purpose |
|-------|-----------------|---------|
| 68 | `start` | Game entry point |
| 435 | `sub_27A` | `wait_n_ticks` function |
| 3167 | `sub_35DE` Chk 1 | Main loop dispatcher (`loc_2341`) |
| 3815 | `loc_275C` | Level transition handler |
| 4118 | `loc_2A10` | Player physics update (grounded) |
| 4600 | `loc_2DC6` | Player death handler |
| 4698 | `loc_2EDC` | Player physics update (airborne) |
| 10145| `sub_5D5F` | Entity update loop |
| 111536| `word_256A6` | `comic_x` (Player X) |
| 111537| `word_256A8` | `comic_y` (Player Y) |
| 112006| `byte_2588E` | `comic_state` (0:idle, 1:move, 2:jump, 3:die) |

### Game Loop Structure (sub_35DE)
The main loop is fragmented into 7 primary chunks that handle different states:
1. **Dispatcher (`loc_2341`)**: Checks state flags (airborne, physics active, animation, etc.).
2. **Physics Handler (`loc_2A10` / `loc_2EDC`)**: Applies gravity and updates (X, Y) based on velocity.
3. **Collision Detection**: Uses `sub_1CFE` to check tile attributes and triggers death if needed.
4. **Entity Update**: Calls `sub_5D5F` to update all active enemies/objects.
5. **Rendering**: Updates viewport via `sub_18E0` and draws sprites via `sub_7DBB`.
6. **Tick Sync**: Calls `sub_27A` (wait_n_ticks) to maintain constant frame rate.

## Annotation Strategy

### Phase 1: Core Infrastructure ✓
1. Entry point and initialization
2. Interrupt handlers
3. Video mode setup
4. Joystick calibration

### Phase 2: Game Loop & Player State ✓
1. Identify fragmented loop chunks
2. Map input variables to physics logic
3. Locate player X/Y and velocity variables
4. Identify core movement and collision functions

### Phase 3: Graphics System (In Progress)
1. EGA plane manipulation
2. Blitting functions
3. Double-buffer swapping
4. Palette operations

### Phase 4: Specific Systems
1. Enemy AI
2. Collision detection
3. Item collection
4. Sound effects

### Phase 5: Data Structures
1. Level format
2. Sprite definitions
3. Map tiles
4. Animation sequences

## Assembly Patterns Reference

### EGA Plane Selection
```asm
; Write to plane
mov dx, 0x3C4      ; Sequencer Index
mov al, 2          ; Map Mask register
out dx, al
inc dx
mov al, [plane]    ; 1,2,4,8 for planes 0-3
out dx, al

; Read from plane
mov dx, 0x3CE      ; Graphics Controller Index
mov al, 4          ; Read Map Select
out dx, al
inc dx
mov al, [plane]    ; 0-3 for planes
out dx, al
```

### Vertical Retrace Wait
```asm
mov dx, 0x3DA      ; Input Status Register
wait_vr_start:
    in al, dx
    test al, 8     ; Bit 3 = vertical retrace
    jz wait_vr_start
```

### Timer Calibration
```asm
; Get PIT counter
mov al, 0
out 0x43, al       ; Latch command
in al, 0x40        ; Low byte
mov ah, al
in al, 0x40        ; High byte
xchg ah, al
```

## Next Steps

1. ✓ Complete core analysis
2. ✓ Map common functions
3. → Annotate comic2.asm with function names
4. → Create detailed data structure definitions
5. → Extract and document level format
6. → Build function call graph
7. → Verify functionality equivalence

## Tools Used

- **IDA Pro 5** - Initial disassembly
- **Ghidra** - C decompilation
- **Reference** - Captain Comic 1 complete disassembly

## Notes

- Comic 2 uses similar but enhanced engine from Comic 1
- Save game feature is new (not just passwords)
- More levels (13 vs. 8 in Comic 1)
- Packed resource format (frpak archives)
- Enhanced joystick calibration
- Demo mode support

---

*Last Updated: 2026-03-07*
*Status: Phase 2 - Graphics System Annotation*
