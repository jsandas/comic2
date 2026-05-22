# Implementation Plan

## Project Objective
Reverse-engineer **Captain Comic II: Fractured Reality** into a clean C++ reimplementation, using the annotated `comic2.asm` disassembly as the primary reference. `unpacked.exe.test.c` (Ghidra pseudo-C) is a secondary cross-reference only — neither source file is intended to compile.

---

## Phase 1–4: Game Loop & Player State Analysis (COMPLETED)

### Original Objective
Map the fragmented main game loop (`sub_35DE`) and identify the core player state variables (`comic_x`, `comic_y`, etc.).

## Status
- **Phase 1: Game Loop Defragmentation** - [COMPLETED]
- **Phase 2: Player State Identification** - [COMPLETED]
- **Phase 3: Function Mapping** - [COMPLETED]

## Background & Motivation
The main game loop (`sub_35DE`) was fragmented into 21 chunks in the disassembly. Understanding this loop is critical for identifying how input is processed, physics are applied, and entities are updated. Concurrently, locating the player state variables is necessary to trace how functions interact with the player.

## Scope
- Analyze and document the control flow of `sub_35DE` and its fragments. [DONE]
- Identify memory addresses corresponding to the player's X/Y coordinates, velocity, facing direction, state (standing, running, jumping), health, and inventory. [DONE]
- Identify the core movement and physics functions (e.g., `handle_fall_or_jump`, `move_left`, `move_right`) called within the loop. [DONE]
- Update project documentation (`FUNCTION_MAP.md`, `REVERSE_ENGINEERING_GUIDE.md`, `CORE_ENGINE.md`) with the findings. [DONE]

## Implementation Steps

### Phase 1: Game Loop Defragmentation [COMPLETED]
1.  **Trace `sub_35DE`**: Followed the control flow starting from `sub_35DE` in `comic2.asm`.
2.  **Identify Fragments**: Documented the primary fragments and their roles in the dispatcher.
3.  **Map Loop Sequence**: Created a high-level sequence diagram in `CORE_ENGINE.md`.

### Phase 2: Player State Identification [COMPLETED]
1.  **Trace Input Variables**: Followed references to input variables into the game loop.
2.  **Locate Coordinates**: Identified `comic_x`, `comic_y`, `comic_x_vel`, `comic_y_vel`.
3.  **Locate Attributes**: Found `comic_hp` (`byte_258CD`).
4.  **Correlate with C Decompilation**: Verified offsets using `unpacked.exe.test.c`.

### Phase 3: Function Mapping [COMPLETED]
1.  **Identify Physics Functions**: Located `sub_2A10` (grounded) and `sub_2EDC` (airborne).
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
- Annotate the EGA blit pipeline (`sub_2B4`, `sub_1D2C`, `sub_1DC0`, `sub_451C`, `sub_79C7`, `sub_7A13`, `sub_7A89`)
- Locate and annotate entity management (`handle_enemies`, `handle_fireballs`, `handle_item`, `spawn_enemy`)
- Locate and annotate file I/O / resource loading (`load_level`, `load_fullscreen_graphic`, `load_shp_files`, `rle_decode`)
- Identify sound system: INT 3 handler, sound effect dispatch, effect addresses

### Steps
1. [ ] Batch-rename all confirmed `sub_XXXX` labels with known identities
2. [ ] Annotate EGA blit pipeline with full function headers
3. [ ] Trace `sub_437B` and `sub_5D5F` callers to locate entity management
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