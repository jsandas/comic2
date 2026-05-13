# Implementation Plan: Game Loop & Player State Analysis

## Objective
To map the fragmented main game loop (`sub_35DE`) and identify the core player state variables (`comic_x`, `comic_y`, etc.) in the Captain Comic 2 disassembly (`comic2.asm`). This will establish the foundational understanding required to map the rest of the game's logic.

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

## Next Steps
- [x] Identify `comic_num_lives` and `comic_firepower` by tracing HUD rendering logic.
    - `comic_num_lives` = `word_2526F` (`ds:28Fh`).
    - `comic_firepower` = `byte_251FC` (`ds:1FCh`).
    - `comic_gems` = `word_25204` (`ds:204h`).
- Document the EGA blitting functions in `sub_2B4` and `sub_451C`.