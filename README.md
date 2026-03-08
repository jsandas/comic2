# Captain Comic 2 Reverse Engineering Project

## Overview

This project aims to create a fully-annotated, functionally-equivalent assembly disassembly of **Captain Comic II: Fractured Reality** (1990) by Michael A. Denio.

### Project Status: **Active Development - Phase 2**

✅ Phase 1: Analysis & Mapping Complete  
🔄 Phase 2: Function Annotation In Progress  
⏸️ Phase 3: Data Structure Documentation  
⏸️ Phase 4: Verification & Testing  

## Source Files

### Working Files
- **comic2.asm** (186,369 lines) - IDA Pro 5.0 disassembly  
- **unpacked.exe.test.c** (9,357 lines) - Ghidra decompilation
- **unpacked.exe.test copy.c** - Backup copy

### Original Game Files (in `original/` directory)
- FR*.* - Level data files (FR000.0 through FR014.2)
- FRDATA.* - Game data files
- FRDEMO.* - Demo mode data
- FRPAK.* - Packed resource archives
- GAME.* - Additional game files
- ComicFAQ.txt - Original FAQ document

### Reference Material
- **Captain Comic 1 disassembly**: https://github.com/jsandas/comic-c/blob/main/reference/disassembly/R5sw1991.asm
  - Complete, annotated disassembly by jsandas
  - Used as template for common engine patterns

## Documentation

### Primary Documentation
1. **[REVERSE_ENGINEERING_GUIDE.md](REVERSE_ENGINEERING_GUIDE.md)** - Master guide
   - Project overview and progress tracking
   - Common patterns between Comic 1 & 2
   - Entity structures and data formats
   - Assembly pattern reference
   - Next steps and roadmap

2. **[FUNCTION_MAP.md](FUNCTION_MAP.md)** - Function cross-reference
   - Complete mapping of sub_XXXX → actual function names
   - Input/output parameters
   - Call locations and relationships
   - Memory address references
   - Data segment breakdown

3. **[ANNOTATION_GUIDE.md](ANNOTATION_GUIDE.md)** - Practical how-to guide
   - How to identify common code patterns
   - Annotation syntax and style
   - Register usage conventions
   - Quick reference patterns
   - Testing and validation tips

## Key Findings

### Game Information
- **Title**: Captain Comic II - Fractured Reality
- **Version**: 1.5
- **Copyright**: 1990 Michael A. Denio
- **Engine**: Enhanced version of Captain Comic 1 engine
- **Platform**: DOS, x86 16-bit real mode
- **Graphics**: EGA 320×200, 16-color (Mode 0Dh)
- **Size**: ~300 KB (unpacked)

### Technical Details
- **Memory Model**: Large
- **Interrupts Used**: INT 08h (timer), INT 09h (keyboard), INT 1Ch (user timer), INT 3 (custom sound)
- **Video Hardware**: EGA with double-buffering (0x0000 and 0x2000 buffers)
- **Sound**: PC speaker via PIT (Programmable Interval Timer)
- **Input**: Keyboard + analog joystick support

### New Features vs. Comic 1
- **Save Game System** - Not just password-based
- **13 Levels** - More than Comic 1's 8 levels
- **Demo Mode** - Attract mode sequences (frdemo files)
- **Packed Resources** - frpak.* archive format
- **Enhanced Joystick** - Improved calibration routines

## Function Mapping Summary

### ✅ Identified Functions (Comic 2 → Purpose)

| Address | Function Name | Purpose |
|---------|---------------|---------|
| `start` | Main Entry Point | Game initialization, similar to Comic 1's main() |
| `sub_9C9` | install_interrupt_handlers | Hook INT 08h, 09h, 1Ch, 3 |
| `sub_A5E` | restore_interrupt_handlers | Restore original DOS vectors |
| `sub_27A` | wait_n_ticks(ax) | Block until N game ticks elapse |
| `sub_9AF` | pit_count() | Read PIT counter for timing |
| `sub_96F` | read_joystick_axis() | Analog joystick input |
| `sub_2B4` | Graphics operation | Sprite/level rendering (needs more analysis) |
| `sub_451C` | Level renderer | Full level/stage rendering system |
| `sub_35DE` | Main game loop | Core game tick (FRAGMENTED - 7 chunks) |

### 🔍 Functions to Locate

Based on Comic 1 patterns, these functions exist but need addresses:
- `face_or_move_left` / `face_or_move_right` - Player movement
- `handle_fall_or_jump` - Physics simulation (gravity, collision)
- `handle_enemies` - Enemy AI update
- `handle_fireballs` - Projectile physics
- `blit_16xH_plane_with_mask` - Masked sprite blitting
- `load_level` - Level data loading
- `rle_decode` - RLE graphics decompression
- `collect_item` - Item pickup logic
- `award_points` / `award_extra_life` - Scoring system

## Code Patterns Documented

### Core Patterns
1. **Game Tick Synchronization** - Flag-based blocking wait
2. **EGA Plane Manipulation** - 4-plane write/read sequences
3. **Vertical Retrace Sync** - Safe video buffer swapping
4. **File I/O** - DOS INT 21h open/read/close
5. **Joystick Reading** - Timing-based analog conversion
6. **PC Speaker Sound** - PIT frequency programming
7. **Fixed-Point Math** - 1/8 unit velocities

### Assembly Signature Patterns
```asm
; EGA Write Pattern
mov dx, 3C4h; out dx, al; inc dx; mov al, <mask>; out dx, al

; EGA Read Pattern  
mov dx, 3CEh; out dx, al; inc dx; mov al, <plane>; out dx, al

; Vertical Retrace Wait
mov dx, 3DAh; .loop: in al, dx; test al, 8; jz .loop

; Game Tick Wait
.tick: cmp cs:byte_245, 1; jnz .tick; mov cs:byte_245, 0
```

## Memory Map

### Code Segments
- **seg000** (Lines 32-16449) - Main game logic, procedures
- **seg001** (Lines 16449-48522) - String constants, UI text
- **seg002** (Lines 48522+) - Stack space

### Data Segments
- **seg003** (Lines 48530-97625) - Game state variables
- **seg004** (Lines 97625-109792) - Graphics, palettes, sprites
- **seg005** (Lines 109792-153580) - Additional game data
- **seg006-seg009** - Miscellaneous segments

### Key Variables
```
byte_245 (game_tick_flag) - Set by INT 08h each 18.2Hz tick
byte_251-254 - Function keys (F1-F4): 3Bh, 3Ch, 3Dh, 3Eh
byte_25E-263 - Arrow keys and action buttons
word_26A-278 - Unknown purpose (interrupt handler state?)
```

## Next Steps

### Immediate Tasks
1. **Deep-dive into sub_35DE** - Map the fragmented main game loop
2. **Locate player state block** - Find comic_x, comic_y, comic_hp, etc.
3. **Identify collision code** - Platform and enemy collision detection
4. **Map enemy AI** - Locate and document behavior patterns
5. **Document level format** - Parse FR*.* file structures

### Medium-term Goals
1. **Complete function annotations** - Name all sub_XXXX procedures
2. **Data structure definitions** - C struct definitions for all entities
3. **Build call graph** - Visual map of function relationships
4. **Extract resource formats** - Document .EGA, .PT, .SHP, frpak formats
5. **Create memory dump tool** - Extract runtime state for analysis

### Long-term Vision
1. **Functionally equivalent assembly** - Assembles to working game
2. **Complete documentation** - Every function and variable documented
3. **C port foundation** - Clean enough for C translation
4. **Modern build system** - NASM/JWASM compatibility
5. **Preservation** - Ensure game can run on modern systems

## Tools & Resources

### Development Tools
- **IDA Pro 5** - Generated initial disassembly
- **Ghidra** - Generated C decompilation
- **Text Editor** - For annotation work
- **DOSBox** - For testing the original game

### Reference Materials
- [Captain Comic 1 Source](https://github.com/jsandas/comic-c)
- [DOS Interrupts Reference](http://www.ctyme.com/intr/int.htm)
- [EGA Programming](https://en.wikipedia.org/wiki/Enhanced_Graphics_Adapter)
- [x86 Assembly Reference](https://www.felixcloutier.com/x86/)
- [8253 PIT Datasheet](https://en.wikipedia.org/wiki/Intel_8253)

### Community
- [Abandonia Forums](https://www.abandonia.com/en/games/108/Captain+Comic+II+-+Fractured+Reality.html)
- [VOGONS](https://www.vogons.org/) - Vintage computer gaming
- [GitHub - jsandas/comic-c](https://github.com/jsandas/comic-c) - Original reverser

## Usage

### Viewing the Disassembly
```bash
# Open in your preferred text editor
code comic2.asm       # VS Code
vim comic2.asm        # Vim
less comic2.asm       # Terminal pager
```

### Searching for Patterns
```bash
# Find all EGA port access
grep -n "3C[0-9A-F]h" comic2.asm

# Find all interrupt calls  
grep -n "int.*" comic2.asm

# Find specific function
grep -n "sub_27A" comic2.asm
```

### Building (Future)
```bash
# Not yet ready to build, but eventually:
# nasm -f bin comic2.asm -o comic2.com
# dosbox comic2.com
```

## Contributing

This is primarily a personal reverse engineering project, but contributions are welcome:

1. **Function identification** - Match sub_XXXX to actual names
2. **Pattern documentation** - Document new patterns found
3. **Testing** - Verify annotations against running game
4. **Documentation** - Improve guides and references
5. **Tools** - Scripts to help with analysis

## Legal

This project is for **educational and preservation purposes only**.

- **Captain Comic II** is © 1990 Michael A. Denio
- Original game is available as freeware
- This disassembly is a transformative work for study
- No commercial use intended
- Respect original author's rights

## Acknowledgments

- **Michael A. Denio** - Original game author
- **jsandas** - Captain Comic 1 disassembly reference
- **IDA Pro/Hex-Rays** - Disassembly tools
- **NSA/Ghidra Team** - Decompilation tools
- **Retro Computing Community** - Knowledge and support

## Contact

For questions about this reverse engineering effort:
- Open an issue if this becomes a public repository
- Refer to documentation files for technical details
- Check FUNCTION_MAP.md for specific code locations

---

**Project Started**: March 7, 2026  
**Status**: Phase 2 - Function Annotation  
**Last Updated**: March 7, 2026  

**Goal**: Create a fully-annotated, functionally-equivalent assembly disassembly
**Progress**: ~15% (Core infrastructure mapped, detailed annotation in progress)

*The adventure continues... just like Captain Comic himself!* 🚀
