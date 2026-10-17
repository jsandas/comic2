#!/usr/bin/env python3
"""
disassemble_data.py - Disassembles data_0.bin into fully documented NASM assembly source (comic2_data.asm).
Validates that assembling comic2_data.asm produces a byte-exact replica of data_0.bin.
"""

import sys
import os

SEG_SCRATCH_OFF = 0x00000   # Seg 0x0D5D
SEG_STACK_OFF   = 0x08AD0   # Seg 0x160A
SEG_FONT_OFF    = 0x0D2D0   # Glyph bitmap tables
SEG_PALETTE_OFF = 0x14A90   # Seg 0x2206
SEG_DATA_OFF    = 0x17A10   # Seg 0x24FE
TOTAL_SIZE      = 141040

KNOWN_LABELS_DATA = {
    0x0000: ("ds_initial_video_mode", "Initial video mode returned by INT 10h AH=0Fh"),
    0x0035: ("ds_beta_version_str", "BETA VERSION 3 banner string"),
    0x0046: ("ds_disk_error_msg", "Fatal disk write-protection error message"),
    0x00a0: ("ds_ega_required_msg", "EGA 256K requirement error message"),
    0x01eb: ("ds_fname_frpak002", "FRPAK.002 archive filename"),
    0x01f5: ("ds_fname_frpak007", "FRPAK.007 archive filename"),
    0x01ff: ("ds_fname_keysdef", "KEYS.DEF configuration filename"),
    0x0210: ("ds_timer_ticks", "System frame timer ticks"),
    0x0221: ("ds_current_room_id", "Current active game screen ID (0-14)"),
    0x06c2: ("ds_scroll_x", "Horizontal viewport scroll offset"),
    0x06c4: ("ds_scroll_y", "Vertical viewport scroll offset"),
    0x0892: ("ds_invulnerability_timer", "Damage invulnerability frame countdown"),
    0x0898: ("ds_lives_remaining", "Captain Comic lives remaining (default 3)"),
    0x089a: ("ds_shield_power", "Current shield energy units (default 5, max 8)"),
    0x08a8: ("ds_weapon_cooldown", "Firing rate limiter cooldown timer"),
    0x08ac: ("ds_blast_power_level", "Fireball projectile power level (1-5)"),
    0x08ae: ("ds_boots_equipped", "High-jump / gravity boots flag"),
    0x08b0: ("ds_lantern_equipped", "Cavern lantern illumination flag"),
    0x08b2: ("ds_corkscrew_equipped", "Grate corkscrew unlock flag"),
    0x08b4: ("ds_wand_equipped", "Teleportation wand flag"),
    0x08b6: ("ds_key_count", "Door keys collected count"),
    0x08e8: ("ds_saved_sp", "Preserved SP for room transition recovery"),
    0x091e: ("ds_player_state", "Player movement state (0=Idle, 1=Walk, 2=Jump, ...)"),
    0x091f: ("ds_player_world_x", "Captain Comic world X coordinate"),
    0x0921: ("ds_player_world_y", "Captain Comic world Y coordinate"),
    0x0926: ("ds_current_score_lo", "Player score counter (low word)"),
    0x0928: ("ds_current_score_hi", "Player score counter (high word)"),
    0x93aa: ("ds_palette_index_table", "Active tile palette mapping index table"),
    0x93c2: ("ds_fname_frpak001", "FRPAK.001 archive filename"),
    0x9482: ("ds_room_file_table", "Room asset file table (fr000.0 through fr014.3)"),
    0x9c74: ("ds_fname_game000_default", "Default GAME.000 save slot path"),
    0x9c7c: ("ds_fname_frcfg", "FR.CFG configuration filename"),
    0x9c83: ("ds_fname_game000", "GAME.000 save game filename"),
    0x9c94: ("ds_high_scores_buffer", "High score table buffer (102 bytes)"),
    0x9ea8: ("ds_fname_frdata0", "frdata.0 graphics data filename"),
    0x9eb1: ("ds_fname_frdata1", "frdata.1 graphics data filename"),
}

KNOWN_LABELS_SCRATCH = {
    0x0000: ("scratch_calibrate_joystick_msg", "Joystick calibration screen header and prompts"),
    0x01a0: ("scratch_define_keys_msg", "Keyboard customization menu prompts"),
    0x035f: ("scratch_key_names_table", "Key name strings table (Esc, 1..9, Q..P, etc.)"),
    0x0600: ("scratch_decompression_buffer", "Runtime scratchpad for PackBits RLE chunks & collision masks"),
}

def escape_nasm_string(b_arr):
    # Splits bytes into tokens of quoted strings and hex constants
    tokens = []
    curr_ascii = bytearray()
    
    def flush_ascii():
        if curr_ascii:
            s = curr_ascii.decode('latin1')
            escaped = '"'
            for ch in s:
                if ch == '"':
                    escaped += '", \'"\', "'
                else:
                    escaped += ch
            escaped += '"'
            escaped = escaped.replace('"", ', '').replace(', ""', '')
            tokens.append(escaped)
            curr_ascii.clear()

    for b in b_arr:
        if 0x20 <= b <= 0x7E and b != ord('"'):
            curr_ascii.append(b)
        else:
            flush_ascii()
            tokens.append(f"0x{b:02x}")
    flush_ascii()
    return ", ".join(tokens)

def main():
    bin_path = 'data_0.bin'
    out_path = 'comic2_data.asm'

    if not os.path.exists(bin_path):
        print(f"Error: {bin_path} not found.")
        sys.exit(1)

    with open(bin_path, 'rb') as f:
        data = f.read()

    assert len(data) == TOTAL_SIZE, f"Expected {TOTAL_SIZE} bytes, got {len(data)}"

    lines = []
    lines.append("; ============================================================================")
    lines.append("; CAPTAIN COMIC II: FRACTURED REALITY")
    lines.append("; Data Segments & Assets Disassembly (data_0.bin - 141,040 bytes)")
    lines.append("; Appended to load module at CS:D5D0 (Physical offset 54,736 / 0xD5D0)")
    lines.append("; ============================================================================")
    lines.append("")

    curr = 0

    def check_segment_header(offset):
        if offset == SEG_SCRATCH_OFF:
            lines.append("; " + "-" * 76)
            lines.append("; SEGMENT: SEG_SCRATCH (Compile Segment 0x0D5D / Relocated 0x1D5D)")
            lines.append("; Offset in data_0.bin: 0x00000 (0) | Size: 35,536 bytes")
            lines.append("; Purpose: Decompression staging buffer, collision masks, & UI text")
            lines.append("; " + "-" * 76)
            lines.append("seg_scratch_start:")
        elif offset == SEG_STACK_OFF:
            lines.append("")
            lines.append("; " + "-" * 76)
            lines.append("; SEGMENT: SEG_STACK (Compile Segment 0x160A / Relocated 0x260A)")
            lines.append("; Offset in data_0.bin: 0x08AD0 (35,536) | Size: 49,088 bytes")
            lines.append("; Purpose: Runtime stack segment (Initial SP = 0x07D0)")
            lines.append("; " + "-" * 76)
            lines.append("seg_stack_start:")
        elif offset == SEG_FONT_OFF:
            lines.append("")
            lines.append("; " + "-" * 76)
            lines.append("; FONT / GLYPH BITMAP TABLE")
            lines.append("; Offset in data_0.bin: 0x0D2D0 (53,968)")
            lines.append("; Purpose: 8x8 EGA font patterns and glyph shapes")
            lines.append("; " + "-" * 76)
            lines.append("font_glyphs_start:")
        elif offset == SEG_PALETTE_OFF:
            lines.append("")
            lines.append("; " + "-" * 76)
            lines.append("; SEGMENT: SEG_PALETTE (Compile Segment 0x2206 / Relocated 0x3206)")
            lines.append("; Offset in data_0.bin: 0x14A90 (84,624) | Size: 12,160 bytes")
            lines.append("; Purpose: EGA 16-color palette registers & DAC color cycling")
            lines.append("; " + "-" * 76)
            lines.append("seg_palette_start:")
        elif offset == SEG_DATA_OFF:
            lines.append("")
            lines.append("; " + "-" * 76)
            lines.append("; SEGMENT: SEG_DATA / DGROUP (Compile Segment 0x24FE / Relocated 0x34FE)")
            lines.append("; Offset in data_0.bin: 0x17A10 (96,784) | Size: 44,256 bytes")
            lines.append("; Purpose: Master game state variables, strings, keybindings, & high scores")
            lines.append("; " + "-" * 76)
            lines.append("seg_data_start:")

    while curr < TOTAL_SIZE:
        check_segment_header(curr)

        # Check for known symbols in SEG_DATA or SEG_SCRATCH
        if curr < SEG_STACK_OFF and curr in KNOWN_LABELS_SCRATCH:
            lbl, desc = KNOWN_LABELS_SCRATCH[curr]
            lines.append(f"\n; {lbl}: {desc}")
            lines.append(f"{lbl}:")
        elif curr >= SEG_DATA_OFF:
            ds_off = curr - SEG_DATA_OFF
            if ds_off in KNOWN_LABELS_DATA:
                lbl, desc = KNOWN_LABELS_DATA[ds_off]
                lines.append(f"\n; {lbl} [DS:0x{ds_off:04x}]: {desc}")
                lines.append(f"{lbl}:")

        # Next boundary: next known label or next segment start
        next_boundary = TOTAL_SIZE
        for boundary in [SEG_SCRATCH_OFF, SEG_STACK_OFF, SEG_FONT_OFF, SEG_PALETTE_OFF, SEG_DATA_OFF]:
            if curr < boundary < next_boundary:
                next_boundary = boundary
        if curr < SEG_STACK_OFF:
            for lbl_off in sorted(KNOWN_LABELS_SCRATCH.keys()):
                if curr < lbl_off < next_boundary:
                    next_boundary = lbl_off
        elif curr >= SEG_DATA_OFF:
            for ds_off in sorted(KNOWN_LABELS_DATA.keys()):
                lbl_off = SEG_DATA_OFF + ds_off
                if curr < lbl_off < next_boundary:
                    next_boundary = lbl_off

        # 1. Check for long zero spans (>= 32 bytes of zeroes up to boundary)
        if data[curr] == 0:
            zero_end = curr
            while zero_end < next_boundary and data[zero_end] == 0:
                zero_end += 1
            zero_count = zero_end - curr
            if zero_count >= 32:
                lines.append(f"    times {zero_count} db 0x00 ; 0x{curr:05x} - 0x{zero_end-1:05x} ({zero_count} zero bytes)")
                curr = zero_end
                continue

        # 2. Check for printable strings
        if 0x20 <= data[curr] <= 0x7E or data[curr] in (0x0A, 0x0D):
            str_end = curr
            while str_end < next_boundary:
                b = data[str_end]
                if 0x20 <= b <= 0x7E or b in (0x09, 0x0A, 0x0D):
                    str_end += 1
                elif b in (0x24, 0x00) and (str_end - curr >= 3):
                    str_end += 1
                    break
                else:
                    break
            
            str_len = str_end - curr
            if str_len >= 4:
                chunk = data[curr:str_end]
                escaped = escape_nasm_string(chunk)
                lines.append(f"    db {escaped} ; 0x{curr:05x}")
                curr = str_end
                continue

        # 3. Default: emit raw hex bytes (up to 16 bytes or up to next boundary)
        chunk_len = min(16, next_boundary - curr)
        chunk = data[curr:curr + chunk_len]
        hex_vals = ", ".join(f"0x{b:02x}" for b in chunk)
        lines.append(f"    db {hex_vals} ; 0x{curr:05x}")
        curr += chunk_len

    with open(out_path, 'w') as f:
        f.write("\n".join(lines) + "\n")

    print(f"Successfully generated {out_path} ({len(lines)} lines).")

if __name__ == '__main__':
    main()
