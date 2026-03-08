# Quick Reference: Common Code Patterns in Captain Comic 2

## How to Identify Functions

When you see these patterns in comic2.asm, they likely correspond to these functions:

### 1. Game Tick Wait Pattern
```asm
cmp  cs:byte_245, 1    ; or similar game tick flag
jnz  <loop>
mov  cs:byte_245, 0
dec  ax
jnz  <loop>
```
**→ This is `wait_n_ticks` (sub_27A at line 399)**

### 2. EGA Plane Write Setup
```asm
mov  dx, 3C4h          ; Sequencer Index
mov  al, 2             ; Map Mask register
out  dx, al
inc  dx
mov  al, <mask>        ; 1, 2, 4, or 8
out  dx, al
```
**→ Part of graphics blitting functions (sub_2B4, sub_451C)**

### 3. EGA Plane Read Setup
```asm
mov  dx, 3CEh          ; Graphics Controller Index  
mov  al, 4             ; Read Map Select
out  dx, al
inc  dx
mov  al, <plane>       ; 0-3
out  dx, al
```
** Also part of graphics/blitting**

### 4. Vertical Retrace Sync
```asm
mov  dx, 3DAh          ; Input Status Register
loop:
  in   al, dx
  test al, 8           ; Check VR bit
  jz   loop
```
**→ Video buffer swap sync (wait for safe time to change visible buffer)**

### 5. File Open/Read/Close
```asm
mov  ax, 3D00h         ; Open file
int  21h
jb   <error>
mov  bx, ax            ; Save handle
mov  ax, 3F00h         ; Read
int  21h
mov  ax, 3E00h         ; Close
int  21h
```
**→ Resource loading functions (graphics, levels, sprites)**

### 6. Joystick Reading
```asm
cli                    ; Disable interrupts
out  <port>, al        ; Trigger joystick
mov  cx, <timeout>
loop <check>
test al, <bit>
sti                    ; Re-enable interrupts
```
**→ Joystick input (sub_96F at line 1356)**

### 7. PC Speaker Sound
```asm
in   al, 61h           ; Read PPI port B
and/or al, <bits>      ; Manipulate speaker bits 0-1
out  61h, al
```
**→ Sound control (INT 3 handler, disable in start())**

### 8. PIT Counter Read
```asm
mov  al, 0
out  43h, al           ; Latch command
in   al, 40h           ; Low byte
mov  ah, al  
in   al, 40h           ; High byte
xchg ah, al
```
**→ Timing/speed detection (sub_9AF at line 1407)**

## Key Memory Locations

### Flags and State
```
byte_245 (game_tick_flag)    - Set by INT 08h each tick
byte_246-byte_250            - Unknown state flags
```

### Key Mappings
```
byte_251-254                 - F1-F4 function keys (3Bh,3Ch,3Dh,3Eh)
byte_255 (key_inventory)     - 'I' = 49h
byte_256 (key_quit)          - 'Q' = 51h
byte_257 (key_game_menu)     - 'G' = 47h
byte_25E (scancode_left)     - Left arrow = 4Bh
byte_25F (scancode_right)    - Right arrow = 4Dh
byte_260 (scancode_up)       - Up arrow = 48h
byte_261 (scancode_down)     - Down arrow = 50h
byte_262 (scancode_jump)     - Space = 39h
byte_263 (scancode_fire)     - 'R' = 52h
```

### Interrupt Handlers (Look for INT 21h AX=2500h+vector)
```
INT 08h - Timer (18.2 Hz tick)
INT 09h - Keyboard scancodes
INT 1Ch - User timer hook
INT 3  - Sound control
```

## Common Register Usage (Comic 1 patterns)

### Function Entry/Exit
```
Caller-saved: AX, BX, CX, DX (may be trashed)
Callee-saved: SI, DI, BP (should be preserved)
Stack grows down (SP decrements)
```

### Typical Parameter Passing
```
AX      - Primary input/output, counts, sizes
BX      - Pointers, handles, secondary input
CX      - Loop counters, sizes
DX      - Port I/O, addresses, secondary data
SI      - Source pointer for string ops
DI      - Destination pointer for string ops
DS:DX   - Filename strings for INT 21h
ES:DI   - Video memory destinations
```

### Fixed-Point Math
```
Velocities in 1/8 game units per tick
Example: comic_y_vel = 8 means 1 pixel/tick
         comic_y_vel = 23 is terminal velocity
```

## How to Annotate

### Step 1: Identify Function Purpose
Look at what it does:
- Does it touch ports 0x3C4, 0x3CE, 0x3DA? → Graphics
- Does it use INT 21h? → File I/O
- Does it use INT 10h? → Video
- Does it manipulate port 0x61? → Sound
- Does it check byte_245? → Timing/sync

### Step 2: Name the Function
Use Captain Comic 1 naming when possible:
```
sub_9C9  → install_interrupt_handlers
sub_27A  → wait_n_ticks
sub_2B4  → (graphics operation, need more analysis)
sub_451C → (level/sprite rendering)
```

### Step 3: Add Comments
```asm
; ============================================================================
; FUNCTION_NAME - Brief description
; Input:  Register parameters
; Output: Modified registers, memory
; Calls:  Other functions
; Pattern matches: Comic 1 equivalent
; ============================================================================
```

### Step 4: Label Key Sections
```asm
.initialize:
    ; Setup code
    
.main_loop:
    ; Loop body
    
.cleanup:
    ; Restore state
    
.error:
    ; Error handling
```

## Quick Search Commands in Editor

### Find all EGA port usage:
```
Search regex: (in|out).*3C[0-9A-F]h
```

### Find all INT calls:
```
Search regex: int\s+[0-9A-Fh]+
```

### Find all procedure definitions:
```
Search regex: ^[a-zA-Z_][a-zA-Z0-9_]*\s+proc
```

### Find all jumps to specific location:
```
Search: j[a-z]+\s+loc_XXXX
```

### Find comparison with specific value:
```
Search: cmp.*byte_245
```

##Important Code Landmarks

### File Start
- Line 1-30: IDA header
- Line 32: Copyright string (identifies as Comic 2 v1.5)
- Line 71: `start` procedure (main entry)

### Core Functions
- Line 399: sub_27A (wait_n_ticks)
- Line 417: sub_2B4 (graphics operation)
- Line 1356: sub_96F (joystick)
- Line 1407: sub_9AF (PIT timer)  
- Line 1427: sub_9C9 (install interrupts)
- Line 5534: sub_35DE (main game loop - fragmented)
- Line 7443: sub_451C (rendering)

### Data Sections
- Line 16449+: String data (seg001)
- Line 48522+: Stack (seg002)
- Line 97625+: Graphics data (seg004)

## Validation Checklist

When you think you've identified a function, verify:

- [ ] Does it match the pattern from Comic 1?
- [ ] Do the registers used make sense for this operation?
- [ ] Are there similar calls elsewhere with similar parameters?
- [ ] Does it fit in the overall program flow?
- [ ] Can you trace where it's called from?

## Example: Annotating a Graphics Function

**Before:**
```asm
sub_2B4 proc near
    push ds
    mov ax, seg seg004
    mov ds, ax
    mov si, 1CD0h
    call sub_79C7
    ...
```

**After:**
```asm
; ============================================================================  
; LOAD_AND_DISPLAY_LEVEL - Load level graphics and render to screen
; Input:  None (uses global level number)
; Output: Graphics rendered to video buffer
; Calls:  sub_79C7 (blit_ega_plane), sub_7A13 (blit_with_mask)
; Pattern: Similar to Comic 1's render_map + load_level
; ============================================================================
load_and_display_level proc near    ; sub_2B4
    push ds
    
    ; Point DS to graphics data segment
    mov  ax, seg seg004
    mov  ds, ax
    assume ds:seg004
    
    ; Load tile graphics from offset 1CD0h
    mov  si, 1CD0h              ; Source: tile graphic data
    call sub_79C7               ; blit_ega_plane
    ...
```

## Testing Your Annotations

1. **Run the game** (if you have DOSBox)
2. **Set breakpoints** at annotated functions
3. **Verify behavior** matches your annotations
4. **Watch registers** confirm expected values
5. **Trace calls** to build call graph

## Final Tips

- **Start with obvious functions** (video mode set, interrupt install)
- **Work outward** from known functions
- **Cross-reference Comic 1** constantly
- **Document unknowns** - mark with "TODO" or "UNKNOWN"
- **Be patient** - 186K lines takes time!
- **Save often** - use version control
-  **Ask for help** - the retro-computing community is friendly!

---
*This guide assumes familiarity with x86 assembly and DOS programming*
*Refer to FUNCTION_MAP.md for detailed function locations*
