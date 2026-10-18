# Captain Comic II: Fractured Reality

## Reverse Engineering and Function Annotation Summary

The 16-bit real-mode MS-DOS executable `unpacked.exe` has been reverse engineered, decomposed into its constituent subsystems, and fully annotated.

All changes and findings have been synchronized into:

1. The Ghidra automation script ([`scripts/ApplyAllAnnotations.java`](scripts/ApplyAllAnnotations.java)): automatically labels and documents all 182 functions in Ghidra with semantic names, subsystem categories, and purpose comments.
2. The annotated assembly source (`comic2_annotated.asm`): an 8,582-line 8086 assembly listing with function headers, caller lists, memory references, and line-by-line disassembly.
3. The comprehensive technical documentation ([docs/CAPTAIN_COMIC_2_REVERSE_ENGINEERING.md](docs/CAPTAIN_COMIC_2_REVERSE_ENGINEERING.md)): a full architecture breakdown of the graphics, audio, level streaming, physics, interrupt vectors, and file compression engines.
4. Compilable NASM assembly sources ([comic2_nasm.asm](comic2_nasm.asm), [comic2_data.asm](comic2_data.asm), [comic2_standalone.asm](comic2_standalone.asm)) and build guide ([docs/NASM.md](docs/NASM.md)): byte-exact real-mode 8086 code and data segments compilable with standard NASM to a runnable MS-DOS `.EXE`.

---

## Key Architectural Discoveries

### 1. Binary Structure and Memory Layout

- Language and compiler: handcrafted 8086 assembly by Michael A. Denio (1990).
- Segment allocation:
  - `CS = 0x1000`: code segment (`1000:0064`–`1000:7E46`), followed by embedded sprite tables, jump vectors, tile graphics, and audio frequency tables (`1000:7E4A`–`1000:D5CF`).
  - `DS = 0x34FE` (`DGROUP`): master state variables, strings, key bindings, and high scores.
  - `ES = 0xA000`: EGA video RAM (64 KB) with Page 0 (`0x0000`) and Page 1 (`0x8000`).
  - `Aux = 0x1D5D`: staging scratchpad for asset decompression and collision masks.
  - `Pal = 0x3206`: 16-color EGA palette tables and color-cycling sequences.

### 2. Hardware Interrupts and Custom ISRs

- `isr_keyboard_int09` (`1000:05c2`): custom non-blocking keyboard ISR reading port `0x60`, managing simultaneous multi-keypress and release matrices (allowing walking, jumping, and shooting together), and sending EOI to the 8259 PIC.
- `isr_timer_sound_int08` (`1000:0683`): dual-purpose timer ISR updating frame tick counters (`CS:[0x0245]`) and serving as the PC speaker sound synthesis engine via 8253 PIT channel 2 (port `0x42`) and port `0x61`.
- `snd_api_int03` (`1000:08c7`): custom sound engine API triggered via a 1-byte opcode `INT 3`:
  - `AL = 0`: stop/silence sound
  - `AL = 1`: play SFX (data pointer in `BX`, priority in `CL`)
  - `AL = 2`: query/stop background melody
  - `AL = 3`: toggle sound enable flag
- `isr_joystick_int15` (`1000:093a`): replaces BIOS `AH=84h`, directly measuring RC timing pulses on standard IBM Game Port `0x0201` for analog joystick axes and buttons.

### 3. EGA Mode 0Dh Planar Graphics and Double Buffering

- Operates in EGA 320x200 16-color mode.
- Implements flicker-free double buffering by toggling CRT start address registers on port `0x3D4` synchronized to vertical blanking (`vid_wait_vblank`, `1000:774e`).
- Employs both EGA Write Mode 0 (for fast planar block clears and aligned tiles) and Write Mode 2 (`1000:790c`) with latch registers and bitmasking (`spr_blit_masked`, `1000:4b46`) for masked sprite blitting.

### 4. File Streaming and Byte-Run RLE Decompression

- Room loading (`map_load_room`, `1000:1b8d`) streams four component files per room:
  - `FRxxx.0`: Byte-Run / PackBits RLE compressed tilemap matrix (unpacked by `file_load_pack_rle`, `1000:1fa4`)
  - `FRxxx.1`: raw collision geometry and obstacle bitmasks (`file_load_raw_mask`, `1000:1ffe`)
  - `FRxxx.2`: room-specific 16-color tile graphics (`file_load_tileset`, `1000:2022`)
  - `FRxxx.3`: entity, item, and doorway spawn records (`file_load_entity_data`, `1000:2049`)

### 5. Copy Protection and Hardware Fingerprinting (`1000:6545`)

The function `sys_generate_system_hash` checksums 64 words from the Video BIOS (`C000:0000`), System BIOS (`F000:0000`), and BIOS date/model string (`F000:FF80`), hashing them with the DOS system clock to validate manual lookups against `PROTCODE.TXT` and seed the PRNG.

---

## Function Category Map (All 182 Functions)

| Subsystem | Count | Key Function Examples |
| --- | ---: | --- |
| System, Startup & Lifecycle | 12 | `entry` (`1000:0064`), `sys_main_dispatcher` (`1000:5e52`), `sys_install_interrupts` (`1000:09c9`), `sys_restore_interrupts` (`1000:0599`), `sys_exit_to_dos` (`1000:0215`), `sys_fatal_error` (`1000:0202`) |
| Interrupt Handlers (ISRs) | 5 | `isr_keyboard_int09` (`1000:05c2`), `isr_timer_sound_int08` (`1000:0683`), `snd_api_int03` (`1000:08c7`), `isr_joystick_int15` (`1000:093a`), `isr_ctrl_break_int23` (`1000:067c`) |
| Video, EGA Registers & Blitting | 41 | `vid_set_ega_palette` (`1000:015d`), `vid_wait_vblank` (`1000:774e`), `vid_flip_buffers` (`1000:4b25`), `vid_set_write_mode_2` (`1000:790c`), `vid_blit_tile_aligned` (`1000:7b31`), `vid_blit_tile_shifted` (`1000:7b8b`), `vid_blit_planar_block` (`1000:7dbb`) |
| Audio Engine & SFX | 11 | `snd_play_blast` (`1000:3c13`), `snd_play_hit` (`1000:3cb1`), `snd_play_jump` (`1000:3db6`), `snd_play_pickup` (`1000:3bc6`), `snd_play_door_open` (`1000:3e1f`), `snd_play_level_music` (`1000:3f0a`) |
| Tilemap, Levels & Streaming | 16 | `map_load_room` (`1000:1b8d`), `file_load_pack_rle` (`1000:1fa4`), `map_init_tileset` (`1000:198e`), `map_draw_background` (`1000:1d2c`), `map_get_tile_at` (`1000:1cfe`), `map_redraw_dirty_tiles` (`1000:5d5f`) |
| Player Physics & Collision | 24 | `game_main_loop` (`1000:2341`), `player_update_physics` (`1000:2a10`), `player_do_jump` (`1000:2d61`), `player_check_ground_collision` (`1000:2b8e`), `player_check_swimming` (`1000:2c9d`), `player_take_damage` (`1000:2fee`) |
| Weapons & Projectiles | 5 | `player_fire_projectile` (`1000:30c6`), `proj_init_trajectory` (`1000:3107`), `proj_spawn_burst` (`1000:3181`), `proj_update_all` (`1000:31eb`), `proj_check_monster_hit` (`1000:327d`) |
| Monsters & Enemy AI | 18 | `ent_update_all` (`1000:330f`), `ent_spawn_enemy` (`1000:3468`), `ent_update_ai` (`1000:34a9`), `ent_check_player_contact` (`1000:35de`), `boss_update_state` (`1000:409c`), `boss_render_sprites` (`1000:4d41`) |
| Items, Inventory & HUD | 14 | `item_collect_pickup` (`1000:36c0`), `hud_render_all` (`1000:378e`), `hud_update_score` (`1000:385f`), `hud_draw_shield_bar` (`1000:3a5f`), `item_add_to_inventory` (`1000:48aa`) |
| UI, Menus, Dialogs & Saves | 36 | `ui_main_menu` (`1000:6df4`), `ui_title_screen` (`1000:7298`), `ui_story_screen` (`1000:7164`), `ui_high_scores_screen` (`1000:6f47`), `game_save_load_menu` (`1000:6668`), `file_load_keys_def` (`1000:62a4`), `game_pause_handler` (`1000:46e6`) |

---

## Deliverable Files & Documentation

### Assembly Sources & Build Files
- [`comic2_nasm.asm`](comic2_nasm.asm): 100% compilable NASM source for the code segment (`CS:0000`–`CS:D5D0`).
- [`comic2_data.asm`](comic2_data.asm): Disassembled and annotated NASM source for data segments, font glyphs, EGA palettes, and state tables (`0xD5D0`–`0x2FD40`, 141,040 bytes).
- [`comic2_standalone.asm`](comic2_standalone.asm): Standalone NASM source packaging MZ header, relocations, code, and data into a playable `comic2.exe`.
- [`comic2_annotated.asm`](comic2_annotated.asm): Standalone annotated assembly listing with procedure cross-references.
- [`Makefile`](Makefile): Automated build, verification, and execution workflow.
- [`scripts/extract_data.py`](scripts/extract_data.py): Asset extractor generating `data_0.bin` from `original/unpacked.exe`.
- [`scripts/disassemble_data.py`](scripts/disassemble_data.py): Disassembler transforming `data_0.bin` into documented `comic2_data.asm`.
- [`run-dosbox.sh`](run-dosbox.sh): Script to automatically sync and run the re-assembled game in DOSBox-X.
- [`dosbox_deterministic.conf`](dosbox_deterministic.conf): DOSBox-X configuration for deterministic gameplay validation.

### Technical Documentation
- [`docs/CAPTAIN_COMIC_2_REVERSE_ENGINEERING.md`](docs/CAPTAIN_COMIC_2_REVERSE_ENGINEERING.md): Comprehensive 182-function reverse engineering architecture report.
- [`docs/NASM.md`](docs/NASM.md): NASM assembly rebuild and relocation verification guide.
- [`docs/FILE_FORMATS.md`](docs/FILE_FORMATS.md): Detailed specifications for `KEYS.DEF`, `FR.CFG`, `GAME.000`–`GAME.003`, and room quad-files.
- [`docs/MEMORY_MAP.md`](docs/MEMORY_MAP.md): Real-mode segment allocations, EGA VRAM double-buffering, and `DS`/`CS` state variable catalog.
- [`docs/MODDING.md`](docs/MODDING.md): Practical recipes for infinite lives, super-jump, rapid fire, and copy-protection bypass.
- [`LICENSE.md`](LICENSE.md): MIT tool license and historical software preservation statement.

---

## Quick Start (Build & Run)

Using `make`:
```bash
make all      # Builds comic2_code.bin, comic2.exe, and verifies exact byte parity
make run      # Launches the assembled game in DOSBox-X
```

Or manually using NASM:
```bash
# 1. Extract data asset binary and disassemble data objects (if not already generated):
python3 scripts/extract_data.py
python3 scripts/disassemble_data.py

# 2. Assemble code segment binary (54,736 bytes):
nasm -f bin comic2_nasm.asm -o comic2_code.bin

# 3. Assemble complete standalone MS-DOS executable (196,288 bytes):
nasm -f bin comic2_standalone.asm -o comic2.exe

# 4. Launch in DOSBox-X:
./run-dosbox.sh
```
