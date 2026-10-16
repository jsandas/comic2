# Captain Comic II: Fractured Reality — Memory Map & State Variables

This document defines the real-mode memory layout, segment registers, hardware port interfaces, and primary game state variables used throughout *Captain Comic II: Fractured Reality*.

---

## 1. Real-Mode Segment Allocations

When loaded by MS-DOS, the game establishes several dedicated segments referenced by the 30 relocation words in the MZ header:

| Segment Constant | Compile Base | Runtime Relocation | Purpose |
| :--- | :---: | :---: | :--- |
| `CS` (Code) | `0x1000` | Load Segment `S` | Executable code, input/audio state table, and PC speaker frequency LUTs |
| `SEG_DATA` (`DS`) | `0x34FE` | `S + 0x24FE` | Master game state, player stats, projectile pools, UI strings, save records |
| `SEG_STACK` (`SS`) | `0x160A` | `S + 0x160A` | Runtime stack segment (Initial `SP = 0x07D0`, 2,000 bytes) |
| `SEG_SCRATCH` (`Aux`) | `0x1D5D` | `S + 0x0D5D` | Decompression staging buffer for PackBits RLE chunks and collision masks |
| `SEG_PALETTE` (`Pal`) | `0x3206` | `S + 0x2206` | 16-color EGA Attribute Controller palette definitions |
| `SEG_BUFFER_1` | `0x2FCC` | `S + 0x1FCC` | Planar sprite buffer: Blue plane |
| `SEG_BUFFER_2` | `0x37CC` | `S + 0x27CC` | Planar sprite buffer: Green plane |
| `SEG_BUFFER_3` | `0x3FCC` | `S + 0x2FCC` | Planar sprite buffer: Red plane |
| `SEG_BUFFER_4` | `0x47CC` | `S + 0x37CC` | Planar sprite buffer: Intensity plane |
| `VRAM` (`ES`) | `0xA000` | `0xA000` | Physical EGA video RAM (64 KB planar memory) |

---

## 2. EGA Video Memory Map (`0xA000:0000`)

The game operates in **EGA Mode 0Dh** (320x200, 16 colors, 4 planar bitplanes):
- Each 320x200 screen requires **40 bytes per scanline × 200 scanlines = 8,000 bytes per plane**.
- Four parallel bitplanes (Blue, Green, Red, Intensity) occupy **32,000 bytes** of memory per complete page.
- Double buffering is achieved by alternating between two 32 KB pages:

```
Address Range          Size       Page Allocation
-----------------------------------------------------------------------------
A000:0000 - A000:7CFF  32,000 B   Page 0 (Visible while Page 1 is drawn)
A000:7D00 - A000:7FFF     768 B   Unused scanline padding
A000:8000 - A000:FCFF  32,000 B   Page 1 (Visible while Page 0 is drawn)
A000:FD00 - A000:FFFF     768 B   Unused scanline padding
```

Double buffering is controlled by `vid_flip_buffers` (`1000:4b25`), which updates CRT Controller registers **0x0C (Start Address High)** and **0x0D (Start Address Low)** on I/O port `0x03D4` synchronized to vertical blanking on port `0x03DA`.

---

## 3. In-Code State Table (`CS:0245` – `CS:027A`)

Unlike modern code, real-mode DOS games frequently kept high-frequency interrupt flags inside `CS` to allow immediate access within interrupt service routines without saving and restoring `DS`:

| Offset in `CS` | Size | Symbol / Purpose | Accessed By |
| :---: | :---: | :--- | :--- |
| `CS:[0x0245]` | Byte | **Frame Tick Flag**: Set to `1` by `INT 08h` timer ISR; polled and cleared by `timer_delay_ticks`. | `1000:007d`, `1000:027a`, `1000:0695` |
| `CS:[0x0246]` | Byte | **Sound Enabled**: 1 = audio active, 0 = audio muted. | `1000:0688`, `1000:07d6` |
| `CS:[0x0247]` | Byte | **SFX Playing Flag**: 1 if a sound effect is actively executing. | `1000:07e0`, `1000:08ec`, `1000:0918` |
| `CS:[0x0248]` | Byte | **Melody Playing Flag**: 1 if background music score is active. | `1000:066b`, `1000:0827`, `1000:0907` |
| `CS:[0x0249]` | Byte | **Current SFX Priority**: CL priority of running sound. | `1000:083d`, `1000:08df`, `1000:08e6` |
| `CS:[0x024A]` | Word | **SFX Note Data Pointer**: Offset into `CS:[0x7E4A]` sound table. | `1000:07ef`, `1000:07f4`, `1000:08f2` |
| `CS:[0x024C]` | Word | **Note Duration Counter**: Decremented on each `INT 08h` tick. | `1000:07e8`, `1000:0823`, `1000:08fd` |
| `CS:[0x024E]` | Word | **Active Melody Frequency**: Current PIT frequency divisor. | `1000:07da`, `1000:08f9` |
| `CS:[0x024F..0x0257]` | 9 B | **Keyboard Scancode LUT**: Defaults for F1-F4, PgUp, PgDn, Home, Space, Ins. | `1000:07b2`, `1000:07c3` |
| `CS:[0x0258]` | Byte | **Action: Move Left**: 1 = Key pressed / Joystick Left. | `1000:0647`, `1000:254e`, `1000:2f48` |
| `CS:[0x0259]` | Byte | **Action: Move Right**: 1 = Key pressed / Joystick Right. | `1000:064e`, `1000:255f`, `1000:2f5e` |
| `CS:[0x025A]` | Byte | **Action: Jump / Up**: 1 = Key pressed / Joystick Up. | `1000:0655`, `1000:23c7`, `1000:2ee5` |
| `CS:[0x025B]` | Byte | **Action: Crouch / Down**: 1 = Key pressed / Joystick Down. | `1000:065c`, `1000:2453`, `1000:2f8e` |
| `CS:[0x025C]` | Byte | **Action: Fire / Shoot**: 1 = Key pressed / Joystick Button 1. | `1000:0639`, `1000:243f`, `1000:2fc6` |
| `CS:[0x025D]` | Byte | **Action: Item / Action**: 1 = Key pressed / Joystick Button 2. | `1000:0640`, `1000:23cf`, `1000:310e` |
| `CS:[0x0264..0x0269]` | 6 B | **Debounce Buffers**: Previous frame key state (used for edge triggers). | `1000:06aa`–`1000:06f0` |
| `CS:[0x026E..0x0275]` | 8 B | **Joystick Axis Thresholds**: Calibrated X-min, X-max, Y-min, Y-max words. | `1000:071b`, `1000:072a`, `1000:0737` |
| `CS:[0x0276]` | Word | **Joystick Active Flag**: 1 if analog joystick detected and calibrated. | `1000:069b` |
| `CS:[0x0278]` | Word | **Current Melody Score Pointer**: Offset to active music track. | `1000:0851`, `1000:19b0`, `1000:66b6` |

---

## 4. Master Data Segment Variables (`DS = 0x34FE`)

Variables in the primary data segment manage Comic's inventory, stats, physics, and world state:

| Offset in `DS` | Size | Symbol / Purpose |
| :---: | :---: | :--- |
| `DS:[0x0221]` | Byte | **Current Room ID**: Current active game screen (0 through 14). |
| `DS:[0x0892]` | Word | **Invulnerability Timer**: Frame countdown after taking damage. |
| `DS:[0x0898]` | Byte | **Lives Remaining**: Number of Captain Comic lives (default 3). |
| `DS:[0x089A]` | Byte | **Shield Power**: Current shield units (default 5, max 8). |
| `DS:[0x08A8]` | Byte | **Weapon Cooldown Timer**: Firing delay between projectile bursts. |
| `DS:[0x08AC]` | Byte | **Blast Power Level**: Number of simultaneous fireballs Comic can emit (1–5). |
| `DS:[0x08AE]` | Byte | **Boots Item Flag**: 1 = High-jump / gravity boots equipped. |
| `DS:[0x08B0]` | Byte | **Lantern Item Flag**: 1 = Illuminates dark cavern rooms. |
| `DS:[0x08B2]` | Byte | **Corkscrew Item Flag**: 1 = Allows opening locked grates. |
| `DS:[0x08B4]` | Byte | **Wand Item Flag**: 1 = Teleportation wand enabled. |
| `DS:[0x08B6]` | Byte | **Key Count**: Number of keys collected. |
| `DS:[0x08E8]` | Word | **Saved Stack Pointer (SP)**: Preserved SP for long-jump recovery during room transitions. |
| `DS:[0x091E]` | Byte | **Player State**: 0=Idle, 1=Walk, 2=Jump, 3=Fall, 4=Climb, 5=Swim, 6=Hit. |
| `DS:[0x091F]` | Word | **Player World X Coordinate**: Horizontal position on map. |
| `DS:[0x0921]` | Word | **Player World Y Coordinate**: Vertical position on map. |
| `DS:[0x0926]` | Word | **Current Score (Low Word)**: Accumulated player points. |
| `DS:[0x0928]` | Word | **Current Score (High Word)**: 32-bit score overflow. |

---

## 5. Hardware I/O Port Map

| Port Range | Device / Chip | Access | Purpose |
| :---: | :--- | :---: | :--- |
| `0x0040`–`0x0043` | **Intel 8253 PIT** | Read/Write | Channel 2 square-wave frequency divisor for PC speaker audio synthesis. |
| `0x0060` | **Intel 8042 Keyboard** | Read | Raw XT/AT keyboard make/break scancodes read by `isr_keyboard_int09`. |
| `0x0061` | **System Control Port B** | Read/Write | Gate and data bit 1 toggled to enable/disable speaker output from PIT Ch 2. |
| `0x0201` | **Standard Game Port** | Read/Write | Discharges 555 timers to measure analog joystick X/Y axes and read 2 buttons. |
| `0x03C0`–`0x03C1` | **EGA Attribute Controller** | Write | Palette registers (0–15) controlling 16 active colors out of 64-color gamut. |
| `0x03CE`–`0x03CF` | **EGA Graphics Controller** | Read/Write | Selects Write Mode 0 vs Write Mode 2, bitmasking, and plane read maps. |
| `0x03D4`–`0x03D5` | **EGA CRT Controller** | Write | Registers 0x0C/0x0D used to toggle VRAM display page start address. |
| `0x03DA` | **EGA Input Status 1** | Read | Bit 3 polled for vertical retrace synchronization (`vid_wait_vblank`). |
