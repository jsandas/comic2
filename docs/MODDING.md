# Captain Comic II: Fractured Reality — Modding & Customization Guide

Because the disassembled codebase compiles directly with standard NASM into a byte-accurate MS-DOS binary, you can modify gameplay mechanics, physics, audio, and player stats with immediate turnaround.

---

## 1. Quick Modding Workflow

```bash
# 1. Edit assembly routines in comic2_nasm.asm or comic2_standalone.asm
# 2. Re-assemble and verify:
make all

# 3. Test immediately in DOSBox-X:
make run
```

---

## 2. Practical Modding Recipes

### Recipe A: Infinite Lives & Shields

#### 1. Starting Lives
In `comic2_nasm.asm`, search for `player_init` / startup initialization:
```nasm
; Default: Sets starting lives to 3 (DS:[0x0898] = 0x03)
    mov          byte [0x898], 0x03

; Mod: Start with 99 lives:
    mov          byte [0x898], 0x63
```

#### 2. God Mode (Disable Damage)
In function `player_take_damage` (`1000:2fee`):
```nasm
player_take_damage:
    ; Normal logic decrements shield byte [0x89a]:
    ; dec   byte [0x89a]

    ; Mod: Exit immediately with no damage:
    ret
```

---

### Recipe B: Super Jump & Moon Gravity

Comic's jump arc is controlled by `player_do_jump` (`1000:2d61`):
```nasm
player_do_jump:
    ; Normal initial vertical velocity:
    mov          word [bp - 0x2], 0x000c   ; Vertical jump power

    ; Mod: Higher leap:
    mov          word [bp - 0x2], 0x0014   ; Higher jump impulse
```

In the gravity update routine `player_update_physics` (`1000:2a10`):
```nasm
    ; Normal downward acceleration per frame:
    add          word [0x921], 0x0004      ; Y-position increment (falling)

    ; Mod: Floatier "Moon Gravity":
    add          word [0x921], 0x0002
```

---

### Recipe C: Machine-Gun Fireball (Rapid Fire)

Projectiles are handled by `player_fire_projectile` (`1000:30c6`). The firing cooldown timer is stored in `DS:[0x08a8]`:

```nasm
player_fire_projectile:
    ; Normal cooldown delay:
    mov          byte [0x8a8], 0x14        ; 20-frame firing delay (~1 second)

    ; Mod: Zero-delay continuous rapid fire:
    mov          byte [0x8a8], 0x02        ; 2-frame firing delay
```

To give Comic maximum blast projectiles from the start without collecting blast power items, modify initial blast count `DS:[0x08ac]`:
```nasm
    mov          byte [0x8ac], 0x05        ; 5 simultaneous fireballs (maximum power)
```

---

### Recipe D: Unlocking All Items at Game Start

During startup initialization in `sys_main_dispatcher` (`1000:5e52`):
```nasm
    ; Enable all inventory tools from the beginning:
    mov          byte [0x8ae], 0x01        ; Boots (High Jump)
    mov          byte [0x8b0], 0x01        ; Lantern (Dark Rooms)
    mov          byte [0x8b2], 0x01        ; Corkscrew (Grate Openers)
    mov          byte [0x8b4], 0x01        ; Wand (Teleportation)
    mov          byte [0x8b6], 0x05        ; 5 Keys
```

---

### Recipe E: Custom Sound Effects & Music Pitches

PC Speaker sound effects are defined in the embedded frequency table at `1000:7E4A` (towards the end of `comic2_nasm.asm`).

Frequencies are set by 16-bit **8253 PIT counter divisors**:
$$\text{Frequency (Hz)} = \frac{1,193,180}{\text{Divisor}}$$

| Divisor (Hex) | Divisor (Decimal) | Produced Frequency |
| :---: | :---: | :---: |
| `0x0120` | 288 | 4,143 Hz (High beep) |
| `0x02D0` | 720 | 1,657 Hz (Mid-range tone) |
| `0x06D0` | 1,744 | 684 Hz (Low blast) |
| `0x0A50` | 2,640 | 452 Hz (Deep rumble) |

To alter a sound effect's pitch:
1. Locate the sound pointer invoked before `int3` (e.g. `LEA BX, [0x96DE]`).
2. Search for the label or offset `loc_96de` or offset in the audio table.
3. Modify the 16-bit divisor words to create custom pitches, chirps, or siren effects.

---

### Recipe F: Bypassing Copy Protection

In `sys_generate_system_hash` (`1000:6545`), the game asks a manual lookup question from `PROTCODE.TXT`:
```nasm
sys_generate_system_hash:
    ; Normal behavior displays question and compares user string hash

    ; Mod: Always return success (AX = 1):
    mov          ax, 0x0001
    ret
```
