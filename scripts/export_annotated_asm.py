import sys
import os
import re
sys.path.append('scripts')
from generate_annotated_assembly_and_ghidra import ANNOTATIONS
from parse_functions import parse_file

functions = parse_file('all_functions_detail.txt')
f_map = {f['addr']: f for f in functions}

out_path = '/Users/hedge/workspace/comic2-rea/comic2_annotated.asm'
with open(out_path, 'w') as out:
    out.write("; " + "=" * 78 + "\n")
    out.write("; CAPTAIN COMIC II: FRACTURED REALITY (1990 by Michael A. Denio)\n")
    out.write("; Comprehensive Annotated DOS 16-bit 8086 Assembly Disassembly\n")
    out.write("; Reverse Engineered and Annotated via REA & Ghidra Analysis\n")
    out.write("; Target: unpacked.exe (MS-DOS Executable, 16-bit Real Mode, EGA 320x200 16-color)\n")
    out.write("; " + "=" * 78 + "\n\n")

    out.write("; ARCHITECTURAL OVERVIEW:\n")
    out.write("; - CPU Target: Intel 8086 / 8088 / 80286+ Real Mode (16-bit)\n")
    out.write("; - Graphics: IBM Enhanced Graphics Adapter (EGA) Mode 0Dh (320x200 16-color planar)\n")
    out.write("; - Video RAM: 0xA000:0000 with 64KB planar double-buffering (Page 0 @ 0x0000, Page 1 @ 0x8000)\n")
    out.write("; - Sound: IBM PC Speaker 8253 PIT Timer / Port 0x61 bit-banging driven by INT 08h ISR\n")
    out.write("; - Audio API: Custom Software Interrupt 03h (AL=cmd, BX=sfx_struct, CL=priority)\n")
    out.write("; - Input: Custom hardware INT 09h Keyboard ISR + Game Port 0x201 / BIOS INT 15h Joystick\n")
    out.write("; - Compression: Byte-Run / PackBits RLE decompression engine for level and tile packs\n")
    out.write("; - Segments:\n")
    out.write(";     CS = 0x1000  (All game engine executable code: 0x0064 - 0x7E46, data tables 0x7E4A - 0xD5CF)\n")
    out.write(";     DS = 0x34FE  (Main game variables, strings, state flags, save slot paths)\n")
    out.write(";     ES = 0xA000  (EGA video display buffer / active page)\n")
    out.write(";     Aux Segment = 0x1D5D (Decompression scratchpad & collision mask staging)\n")
    out.write(";     Palette Segment = 0x3206 (EGA color palette & DAC tables)\n")
    out.write("; " + "=" * 78 + "\n\n")

    for f in functions:
        addr = f['addr']
        info = ANNOTATIONS.get(addr)
        name = info[0] if info else f['name']
        category = info[1] if info else "Uncategorized"
        desc = info[2] if info else "Disassembled game routine."

        out.write("; " + "-" * 78 + "\n")
        out.write(f"; FUNCTION: {name}  [{category}]\n")
        out.write(f"; ADDRESS:  {addr}  (Length: 0x{f['size']:x} bytes)\n")
        if f['callers']:
            out.write(f"; CALLERS:  {', '.join(f['callers'])}\n")
        out.write(f"; PURPOSE:  {desc}\n")
        out.write("; " + "-" * 78 + "\n")
        out.write(f"{name}:\n")

        for inst in f['instructions']:
            # format: '  1000:0064    CLD'
            # or with references
            parts = inst.split(' -> ')
            code_part = parts[0].strip()
            ref_part = " ; -> " + parts[1] if len(parts) > 1 else ""

            # split address and instruction
            m = re.match(r'^(1000:[0-9a-fA-F]+)\s+(.*)$', code_part)
            if m:
                iaddr, icode = m.group(1), m.group(2)
                out.write(f"    {icode:<36} ; {iaddr}{ref_part}\n")
            else:
                out.write(f"    {code_part}{ref_part}\n")
        out.write("\n")

print(f"Annotated assembly exported to: {out_path}")
