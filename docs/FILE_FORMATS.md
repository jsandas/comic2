# Captain Comic II: Fractured Reality — File Formats Specification

This document provides a comprehensive technical specification of all external data files used by *Captain Comic II: Fractured Reality* (Michael A. Denio, 1990).

---

## 1. Overview of File Assets

The game executable operates within a working directory containing several classes of binary assets:

| File Pattern | Type | Size | Primary Function |
| :--- | :--- | :--- | :--- |
| `KEYS.DEF` | User Input | 6 bytes | Player keyboard control bindings |
| `FR.CFG` | Hardware Config | 102 bytes | Audio state, joystick calibration, CPU timing |
| `GAME.000`–`GAME.003` | Save Slots | 3,127 bytes | Save game state snapshot across all rooms |
| `FRDATA.0` / `FRDATA.1` | UI Graphics | 16,644 B / 2,412 B | Title art, game borders, and status bar graphics |
| `FRPAK.001`–`FRPAK.007` | Sprite Bundles | 1.2 KB – 17.4 KB | PackBits RLE compressed sprite frames and items |
| `FRxxx.0` | Room Tilemap | Variable | RLE-compressed 2D background tilemap |
| `FRxxx.1` | Room Collision | Variable | Raw collision mask bitplanes and solid walls |
| `FRxxx.2` | Room Tileset | Variable | Room-specific 16-color EGA 8x8 tile graphics |
| `FRxxx.3` | Room Entities | Variable | Monster spawn coordinates, doors, and pickups |
| `PROTCODE.TXT` | Documentation | 14,523 bytes | Copy protection question and answer database |

---

## 2. Keyboard Definition File (`KEYS.DEF`)

`KEYS.DEF` is a 6-byte binary file read during startup by `file_load_keys_def` (`1000:62a4`). It stores raw IBM PC XT/AT keyboard make scancodes for the six primary gameplay actions:

| Offset | Size | Name | Default Scancode | Default Key |
| :---: | :---: | :--- | :---: | :--- |
| `0x00` | Byte | `KEY_LEFT` | `0x4B` | Left Arrow |
| `0x01` | Byte | `KEY_RIGHT` | `0x4D` | Right Arrow |
| `0x02` | Byte | `KEY_UP` | `0x48` | Up Arrow (Jump) |
| `0x03` | Byte | `KEY_DOWN` | `0x50` | Down Arrow (Crouch / Enter Door) |
| `0x04` | Byte | `KEY_FIRE` | `0x39` | Spacebar (Shoot Blast) |
| `0x05` | Byte | `KEY_ACTION` | `0x36` | Right Shift / Insert (Use Item) |

When the player enters the **"Define Keys"** menu in-game (`ui_define_keys_menu`), the user input routine intercepts scancodes from port `0x60` and overwrites this 6-byte file via DOS `INT 21h AH=3Ch/40h`.

---

## 3. Configuration File (`FR.CFG`)

`FR.CFG` is a 102-byte hardware configuration record read during engine startup:

```
Offset  Size  Field Description
-----------------------------------------------------------------------------
0x00    Word  Sound Enabled flag (0x0001 = On, 0x0000 = Muted)
0x02    3 B   Magic signature ASCII: "DFG" (Denio Family Games)
0x05   77 B   Reserved / padding (zero-filled)
0x52    Byte  Joystick X-Axis Minimum calibration threshold
0x53    Byte  Joystick X-Axis Maximum calibration threshold
0x54    Byte  Joystick Y-Axis Minimum calibration threshold
0x55    Byte  Joystick Y-Axis Maximum calibration threshold
0x56    Word  Joystick center deadzone thresholds
0x58    Word  CPU Speed Calibration Timing Divisor (e.g. 0x7D78)
0x5A    Word  Joystick Present & Enabled flag (0x0001 = Yes, 0x0000 = No)
0x5C    8 B   Reserved
0x64    Word  Configuration Checksum (validates file integrity against corruption)
```

---

## 4. Save Game Slots (`GAME.000` – `GAME.003`)

The game provides 4 persistent save slots (`GAME.000` to `GAME.003`), each exactly **3,127 bytes**.

Handled by `game_save_load_menu` (`1000:6668`), each save file preserves:
1. **Header Record (Bytes `0x0000`–`0x003F`)**:
   - Current Room ID (0 through 14).
   - Captain Comic stats: Lives remaining, shield level, blast power, inventory bits (Boots, Lantern, Corkscrew, Wand, Keys).
   - Current Score (32-bit BCD/binary accumulator).
   - Player X and Y world coordinates.
2. **Room Entity Persistence Table (Bytes `0x0040`–`0x0C36`)**:
   - Array of 16-byte state records for all monsters, collected gems, open doors, and unlocked chests across all 15 game worlds.
   - Prevents collected items and defeated bosses from respawning when re-entering a room.

---

## 5. Room Asset Streaming Quad-Files (`FRxxx.y`)

Each room in the game world is indexed by a 3-digit number `xxx` from `000` to `014` (e.g. `FR000` for the starting surface, `FR001` for the underground cavern). Room transitions invoke `map_load_room` (`1000:1b8d`), streaming four separate component files:

### A. Tilemap Matrix (`FRxxx.0`)
- **Format**: Byte-Run / PackBits RLE compressed stream.
- **Decompressor**: `file_load_pack_rle` (`1000:1fa4`) using scratch segment `Aux = 0x1D5D`.
- **Structure**: Unpacks into a 2D grid of 8x8 tile indices referencing the room's tileset. Control bytes with the high bit set (`flag >= 0x80`) represent repeated tile runs (`count = 257 - flag`), while bytes `< 0x80` represent literal tile byte sequences.

### B. Collision Bitmask (`FRxxx.1`)
- **Format**: Raw uncompressed binary bitmap.
- **Handler**: `file_load_raw_mask` (`1000:1ffe`).
- **Function**: Defines solid terrain, ladders, water (swim zones), deadly spikes, and climbable ropes. Evaluated by `player_check_ground_collision` (`1000:2b8e`) and `player_check_swimming` (`1000:2c9d`).

### C. 16-Color EGA Tileset (`FRxxx.2`)
- **Format**: EGA Mode 0Dh Planar Bitplanes.
- **Handler**: `file_load_tileset` (`1000:2022`).
- **Function**: Contains raw 8x8 pixel tile graphics stored in 4-plane planar format (Plane 0: Blue, Plane 1: Green, Plane 2: Red, Plane 3: Intensity). Blitted to video RAM `0xA000` via Write Mode 0.

### D. Entity Spawn Table (`FRxxx.3`)
- **Format**: Fixed-size structured records.
- **Handler**: `file_load_entity_data` (`1000:2049`).
- **Record Layout**:
  - Word 0: Spawn X coordinate
  - Word 1: Spawn Y coordinate
  - Byte 4: Entity Type ID (enemy type, treasure chest, key, door, teleport vortex)
  - Byte 5: Initial direction / movement state machine parameter
  - Byte 6: Persistence flag index (maps into `GAME.xxx` state table)

---

## 6. Sprite Packages (`FRPAK.001` – `FRPAK.007`)

Sprite animations are partitioned into 7 external archives:
- `FRPAK.001`: Captain Comic player sprites (walking, jumping, shooting, climbing, falling, dying).
- `FRPAK.002`–`FRPAK.006`: World-specific enemy sprites and animated hazards.
- `FRPAK.007`: Shared inventory items, blast projectiles, and explosion particle bursts.

Each sprite frame contains:
- 1 byte: Width in 8-pixel bytes (e.g. 2 = 16 pixels wide).
- 1 byte: Height in scanlines (e.g. 24 = 24 pixels tall).
- Planar pixel mask: 1 bit per pixel (0 = transparent background, 1 = opaque sprite pixel).
- Planar color data: 4 bits per pixel across EGA planes 0–3.
- Rendered via EGA Write Mode 2 latch masking (`spr_blit_masked`, `1000:4b46`).
