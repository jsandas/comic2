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
- **INT 3** - Sound command dispatcher (confirmed at `loc_8C7`, installed by `sub_9C9`)

#### 4. **File I/O**
- DOS INT 21h for file operations:
  - AX=3D00h - Open file (read-only)
  - AX=3F00h - Read from file
  - AX=3E00h - Close file

#### 5. **Sound System**
- PC speaker control via PIT (Programmable Interval Timer)
- Frequency = 1,193,182 / divisor
- Priority-gated playback request (INT 3 AL=1, priority in CL)
- Command API confirmed in handler (`loc_8C7`):
  - AL=0: set speaker gate mode ON
  - AL=1: play request with BX=stream offset, DS=stream segment, CL=priority
  - AL=2: set speaker gate mode OFF
  - AL=3: stop playback
  - AL=4: query playback active flag
- Playback is advanced from INT 08h hook (`loc_683`) using stream word pairs:
  - word pitch_divisor, word tick_duration
  - pitch_divisor 0 ends the stream
  - non-zero divisor is written to PIT channel 2 (ports 43h/42h)

Verified effect stream pointers used by call sites:
- 0x965E (hazard/death path)
- 0x9676 (jump/airborne transition path)
- 0x00DB and 0x96B6 (startup/intro paths)

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

#### Mapped Object Record (12-byte format at `unk_25AD0`)
Confirmed from `ent_build_room_entity_list`, `ent_update_object_behaviors`, and `ent_activate_slot_into_runtime`.

```
Offset  Size    Description
+0x00   word    room_x index (copied from room map key)
+0x02   word    room_y index (copied from room map key)
+0x04   word    descriptor_ptr (points to per-object bounds/behavior descriptor)
+0x06   word    activation/state flags (low byte used for state, high bits for timers/masks)
+0x08   word    world_x (base/persistent x used by activation and culling)
+0x0A   word    world_y (base/persistent y used by activation and culling)
```

Related runtime formats used by the same pipeline:

```
; Active entity list (8-byte records at unk_256E0)
Offset  Size    Description
+0x00   word    x
+0x02   word    y
+0x04   word    flags or mapped-slot index (bit 15 used as state marker)
+0x06   word    sprite/object id pointer used by draw path

; Runtime behavior slot (0x20-byte records at 0x48ED)
Offset  Size    Description
+0x00   word    hitbox_w
+0x02   word    hitbox_h
+0x04   word    type_flags (ah is primary behavior discriminator)
+0x06   word    behavior_state (0 = inactive)
+0x08   word    anim_cycle_len_or_span
+0x0A   word    anim_period
+0x0C   word    unknown_0c
+0x0E   word    unknown_0e
+0x10   word    x
+0x12   word    y
+0x14   word    vel_x_or_param_a
+0x16   word    vel_y_or_param_b
+0x18   word    dir_or_toggle
+0x1A   word    anim_tick
+0x1C   word    unknown_1c
+0x1E   word    mapped_object_ptr (back-reference into unk_25AD0)
```

C-equivalent declarations for currently confirmed fields:

```c
typedef struct {
  uint16_t room_x;
  uint16_t room_y;
  uint16_t descriptor_ptr;
  uint16_t state_flags;
  uint16_t world_x;
  uint16_t world_y;
} MappedObject12;

typedef struct {
  int16_t x;
  int16_t y;
  uint16_t flags_or_slot;
  uint16_t sprite_or_obj;
} ActiveEntity8;

typedef struct {
  uint16_t hitbox_w;
  uint16_t hitbox_h;
  uint16_t type_flags;
  uint16_t behavior_state;
  uint16_t anim_span;
  uint16_t anim_period;
  uint16_t unknown_0c;
  uint16_t unknown_0e;
  int16_t x;
  int16_t y;
  int16_t param_a;
  int16_t param_b;
  uint16_t dir_toggle;
  uint16_t anim_tick;
  uint16_t unknown_1c;
  uint16_t mapped_object_ptr;
} RuntimeEntitySlot32;
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

### Resource Files (Phase 6)

#### Confirmed Decode Formats

1. Signed-RLE stream (`io_load_rle_resource_to_e978`, `load_room_tilemap_from_resource_buffer`):
```
while (1) {
    int8_t control = *src++;
    if (control == 0) break;                // terminator
    if (control < 0) {                      // run
        uint8_t value = *src++;
        repeat(-control) *dst++ = value;
    } else {                                // literal
        copy(control) bytes from src to dst;
    }
}
```

2. 4-plane EGA blit stream (`gfx_rle_blit_opaque_4plane`, `gfx_rle_blit_masked_or_4plane`):
- Stream starts with `uint16_t row_span_bytes`.
- For each of 4 planes, decode packets until produced bytes reach `row_span_bytes`.
- Packet encoding:
  - `packet & 0x80 == 0`: copy `packet` literal bytes.
  - `packet & 0x80 != 0`: repeat next byte `packet & 0x7f` times.
- Masked variant ORs decoded bytes into VRAM; opaque variant writes bytes directly.

#### FR* Family Current Findings

- `frdata.0` / `frdata.1`:
  - Loaded via level resource table (`load_resource`) into `seg001:0600` and other work segments.
  - `load_resource` uses `unk_2E3CC` as an indirection table:
    - level id `ax` indexes a word pointer table at `unk_2E3CC`.
    - each entry points to an 8-byte tuple of 4 filename pointers:
      1) signed-RLE stream file, 2) room-table file, 3) seg5E blob file, 4) optional XOR-0x25 block file.
    - filename pool includes `fr000.*` ... `fr014.*` and `frdata.*` names.
  - `load_room_tilemap_from_resource_buffer` parses header/room entries as:
    - `+0x0002`: `level_id` (must match AX from caller)
    - `+0x0004`: room table of 6-byte entries, indexed by room id BX
    - room entry layout:
      - `+0x00`: `tile_w` (stored to `ds:298h`)
      - `+0x02`: `tile_h` (stored to `ds:29Ah`)
      - `+0x04`: `rle_data_off` (offset from base `0x0600`)
  - Room payload at `0x0600 + rle_data_off` uses signed-RLE decode into tile buffer `ds:09A8`.
  - After decode, engine builds row pointer table at `ds:02A0` and derived pixel extents:
    - `ds:29Ch = tile_w * 16`
    - `ds:29Eh = tile_h * 16`

- `frpak.001`..`frpak.007`:
  - In currently confirmed paths, payloads are loaded by direct filename constants rather than archive-entry parsing.
  - Intro/cinematic path uses fixed string-offset calls:
    - `0xAC12` -> `"frpak.003"` (opaque blit source)
    - `0xAC1C` -> `"frpak.004"` (opaque blit source)
    - `0xAC26` -> `"frpak.005"` (masked blit source)
    - `0xAC30` -> `"frpak.006"` (masked blit source)
  - Adjacent demo loads in same path:
    - `0xABF7` -> `"frdemo.0"`
    - `0xAC00` -> `"frdemo.1"`
    - `0xAC09` -> `"frdemo.2"`
  - Byte-level validation using engine packet rules confirms:
    - all `FRPAK.001`..`FRPAK.007` begin with `row_span=0x1F40` and decode as one complete 4-plane RLE stream from offset 0,
    - decoded stream length consumes entire file for each `.00x` file.
    - validated consumed lengths: `001=17391`, `002=6193`, `003=14568`, `004=10992`, `005=5842`, `006=6643`, `007=1223` bytes.
  - Conclusion: in this code path, FRPAK files behave as direct full-image 4-plane payload files (not directory-based multi-entry archives).
  - Confidence: High.

- `frdemo.0`..`frdemo.2`:
  - Referenced by intro/cinematic resource string table.
  - Byte patterns are consistent with the same packetized RLE families above.
  - Confidence: Medium.

#### .SHP / .EGA Layout Equivalence (engine-side)

- No explicit `.SHP`/`.EGA` filename strings have been promoted yet in this binary branch.
- Engine-side decode now shows three concrete payload classes that map to the expected external concepts:
  - Full-screen/image file payload (`.EGA`-equivalent):
    - `uint16_t row_span_bytes`, then packetized 4-plane stream.
    - Packet rule: literal (`0x00..0x7F`) or run (`0x80..0xFF`, count=`b&0x7F`, repeated next byte).
  - Opaque sprite record (`.SHP` opaque-equivalent in loaded blobs):
    - `uint16_t width_pixels`, `uint16_t height_rows`, then 4 plane blocks of raw row bytes.
  - Masked sprite record (`.SHP` masked-equivalent in loaded blobs):
    - `uint16_t width_pixels`, `uint16_t height_rows`, `uint16_t image_data_off`,
    - mask block first, image block at `record_base + image_data_off`, both organized in plane-major row order.

Reference C-style declarations:

```c
typedef struct {
    uint16_t row_span_bytes;
    /* followed by packetized bytes for plane0..plane3 */
} EgaRle4PlaneFile;

typedef struct {
    uint16_t width_pixels;
    uint16_t height_rows;
    /* followed by opaque plane-major row bytes for 4 planes */
} OpaqueSpriteRecord;

typedef struct {
    uint16_t width_pixels;
    uint16_t height_rows;
    uint16_t image_data_off;
    /* mask bytes start immediately after header; image bytes at +image_data_off */
} MaskedSpriteRecord;
```

- These are the C++ reimplementation-facing layout contracts regardless of final extension-name promotion.

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

### Phase 3: Graphics System (Completed)
1. EGA plane manipulation
2. Blitting functions
3. Double-buffer swapping
4. Palette operations

### Phase 4: Specific Systems (Completed)
1. Enemy AI
2. Collision detection
3. Item collection
4. Sound effects

### Phase 5: Annotation and System Mapping (Completed)
1. Confirmed high-value rendering, input, and entity function names
2. Confirmed loader wrappers and sound handler command API
3. Promoted stable naming for major intro/UI/transition routines

### Phase 6: Data Structures and Resource Format (Completed)
1. [x] Confirm 12-byte mapped object layout
2. [x] Confirm active-runtime 8-byte list and 0x20-byte behavior slot shape
3. [x] Document signed-RLE and 4-plane EGA packet formats used by engine
4. [x] Document FRDATA room-table/header semantics with offset-level detail
5. [x] Document FRPAK `.00x` file behavior and stream boundaries
6. [x] Promote stable `.SHP`/`.EGA`-equivalent payload contracts for reimplementation

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

1. Export Phase 6 structs and resource contracts into dedicated C++ headers.
2. Start Phase 7 platform abstraction and renderer bring-up using the verified packet/sprite formats.
3. Implement loader stubs that mirror `load_resource` tuple semantics and room decode flow.

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

*Last Updated: 2026-05-23*
*Status: Phase 6 - Data Structures and Resource Format Documentation (Completed)*
