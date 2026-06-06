# Copilot Instructions for Captain Comic 2 Reverse Engineering

This repository is reverse engineering Captain Comic II - Fractured Reality.
Treat `comic2.asm` as the primary source of truth and use `unpacked.exe.test.c`
only as a supporting cross-reference.

## Scope and Priorities

- Primary target: `comic2.asm` (IDA-style 16-bit x86 disassembly).
- Supporting references:
	- `REVERSE_ENGINEERING_GUIDE.md`
	- `ANNOTATION_GUIDE.md`
	- `FUNCTION_MAP.md`
	- `reference/comic/` (Captain Comic 1 patterns/equivalents)
- Current analysis focus is graphics-system annotation and stable naming.

## Ground Rules

- Preserve original behavior and instruction flow. Do not rewrite logic.
- Prefer annotation, naming, comments, and structure over code mutation.
- Keep labels and comments consistent with existing style in `comic2.asm`.
- Use ASCII only in annotations.
- If uncertain, mark as `UNKNOWN_*` or `TODO` instead of guessing.
- Every non-trivial rename or purpose claim must include evidence in comments.

## Naming Conventions

- Prefer Captain Comic 1 equivalent names when confidence is high.
	- Example mappings already established:
		- `sub_27A` -> `wait_n_ticks`
		- `sub_9C9` -> `install_interrupt_handlers`
		- `sub_A5E` -> `restore_interrupt_handlers`
		- `sub_96F` -> `read_joystick_axis`
		- `sub_9AF` -> `pit_count`
		- `sub_35DE` -> `game_loop` (fragmented)
		- `sub_5D5F` -> `update_entities`
		- `sub_7DBB` -> `draw_sprite`
- For uncertain functions, use descriptive provisional names:
	- `gfx_*`, `io_*`, `snd_*`, `ent_*`, `tile_*`, `input_*`, `timer_*`.
- Keep original symbol in comments when renaming:
	- `; draw_sprite (was sub_7DBB)`

## Annotation Template

When documenting a function, use this header shape:

```
; ============================================================================
; FUNCTION_NAME - Brief purpose
; Input:  Registers/memory assumptions
; Output: Registers/memory side effects
; Calls:  Known callees
; Evidence: Ports/INT calls/control-flow patterns/Comic1 equivalent
; Confidence: High|Medium|Low
; ============================================================================
```

Inside functions, use short section labels where useful:

- `.initialize`
- `.main_loop`
- `.physics`
- `.render`
- `.cleanup`
- `.error`

## Pattern Recognition Rules

Map common instruction signatures to likely subsystems:

- Tick wait loop on `byte_245` -> timing sync (`wait_n_ticks` pattern).
- EGA write setup on ports `3C4h/3C5h` with map mask 2 -> plane write/blit.
- EGA read setup on ports `3CEh/3CFh` with read map select 4 -> plane read.
- Vertical retrace polling on `3DAh`, bit 3 -> safe buffer swap timing.
- DOS `int 21h` with `3D00h/3F00h/3E00h` -> file open/read/close path.
- Port `61h` bit manipulation -> PC speaker/sound control.
- PIT latch via ports `43h/40h` -> timer calibration/count read.

## High-Value Targets

Prioritize maintaining and extending annotations around these landmarks:

- `start` (entry/init)
- `sub_27A` (wait/tick sync)
- `sub_2B4` and `sub_451C` (graphics pipeline)
- `sub_35DE` fragmented main loop and key chunks:
	- `loc_2341` dispatcher
	- `loc_2A10` grounded physics
	- `loc_2EDC` airborne physics
	- `loc_2DC6` death handler
	- `loc_275C` level transition
- `sub_5D5F` entity updates
- `sub_1CFE` tile attribute/collision query

## Evidence Standards

Before finalizing a function purpose or rename, verify:

- Register usage aligns with expected calling convention intent.
- Call sites pass consistent argument patterns.
- Side effects match subsystem behavior (graphics/input/io/sound/physics).
- Flow fits nearby game-loop stage.
- Comic 1 equivalent is structurally plausible when claimed.

If any of the above is weak, reduce confidence and keep provisional naming.

## Data/State Documentation

- Track player-state globals (`comic_x`, `comic_y`, velocity/state fields).
- Track input mappings/scancodes and tick flags (for timing/input analysis).
- Track entity-table structure updates and offsets as they are confirmed.
- Add concise comments near memory locations instead of long prose blocks.

## Editing Workflow for Copilot

1. Identify candidate function/region in `comic2.asm`.
2. Match known instruction patterns from annotation guides.
3. Cross-check against `unpacked.exe.test.c` and Comic 1 reference where helpful.
4. Add/adjust function header, local labels, and comments.
5. Rename only when evidence is sufficient; otherwise keep provisional name.
6. Update project docs (`FUNCTION_MAP.md` or guide files) when a mapping is promoted.

## What to Avoid

- Do not mass-rename symbols without evidence.
- Do not collapse fragmented control flow into speculative high-level pseudocode.
- Do not remove original labels if they are still useful for cross-reference.
- Do not introduce comments that restate obvious single instructions.

## Code Review Agent

For code review tasks, use the repository's reverse engineering standards and evidence rules as your evaluation guide. Review comments should focus on behavior preservation, annotation accuracy, naming confidence, and the project-specific ground rules in `comic2.asm`.

When reviewing changes, check whether a patch:

- preserves original behavior instead of replacing it with higher-level abstractions,
- adds or updates comments consistent with existing annotation style,
- supplies evidence for renames or subsystem claims,
- follows ASCII-only annotations and naming conventions,
- updates supporting docs when new function mappings or subsystem behaviors are identified.

## Expected Output Quality

- Comments should explain intent and verification evidence, not syntax.
- Terminology should stay consistent across `comic2.asm` and docs.
- Unknowns must remain clearly marked rather than implied as solved.
- Keep progress incremental and verifiable.
