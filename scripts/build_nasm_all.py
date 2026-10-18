import sys
import re
import struct

sys.path.append('scripts')
from generate_annotated_assembly_and_ghidra import ANNOTATIONS
from disassembled_overrides import DISASSEMBLED_OVERRIDES

# Build function label map
func_labels = {}
for addr, (name, cat, desc) in ANNOTATIONS.items():
    off = addr.split(':')[1].lower()
    func_labels[off] = (name, cat, desc)

import os
listing_paths = ['scripts/data/ghidra_code0_listing.txt', 'ghidra_code0_listing.txt']
listing_file = next((p for p in listing_paths if os.path.exists(p)), None)
if not listing_file:
    raise FileNotFoundError(f"Ghidra listing file not found in: {listing_paths}")

with open(listing_file, 'r') as f:
    raw_lines = f.readlines()

JUMP_MNEMS = {
    'JMP', 'JZ', 'JNZ', 'JC', 'JNC', 'JA', 'JAE', 'JB', 'JBE',
    'JG', 'JGE', 'JL', 'JLE', 'JS', 'JNS', 'JO', 'JNO', 'JP', 'JNP',
    'LOOP', 'LOOPE', 'LOOPNE', 'LOOPZ', 'LOOPNZ', 'JCXZ', 'CALL'
}

branch_targets = set()
for line in raw_lines:
    parts = line.strip().split('\t')
    if parts[0] == 'INST':
        mnem = parts[3].upper()
        ops = parts[4] if len(parts) > 4 else ''
        if mnem in JUMP_MNEMS:
            m = re.search(r'(?:0x)?1000:([0-9a-fA-F]{4})', ops)
            if m:
                branch_targets.add(m.group(1).lower())

def get_label(tgt_hex):
    tgt_hex = tgt_hex.lower()
    if tgt_hex in func_labels:
        return func_labels[tgt_hex][0]
    return f'loc_{tgt_hex}'

def nasmify(mnem, ops, orig_hex):
    orig_mnem = mnem
    orig_len = len(bytes.fromhex(orig_hex))
    
    if '.' in mnem:
        base, pfx = mnem.split('.', 1)
        pfx = pfx.upper()
        if pfx in ('REP', 'REPE', 'REPZ'):
            mnem = 'rep ' + base.lower()
        elif pfx in ('REPNE', 'REPNZ'):
            mnem = 'repne ' + base.lower()
        else:
            mnem = pfx.lower() + ' ' + base.lower()
        ops = ''
    else:
        mnem = mnem.lower()

    if mnem in ('lodsb', 'lodsw', 'stosb', 'stosw', 'movsb', 'movsw', 'cmpsb', 'cmpsw', 'scasb', 'scasw'):
        ops = ''

    if mnem == 'int3':
        return 'int3', ''

    if mnem == 'callf':
        mnem = 'call far'
    elif mnem == 'jmpf':
        mnem = 'jmp far'

    # Remove ptr
    ops = re.sub(r'\bbyte\s+ptr\b', 'byte', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\bword\s+ptr\b', 'word', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\bdword\s+ptr\b', 'dword', ops, flags=re.IGNORECASE)

    # CS:[...] -> [cs:...], ES:[...] -> [es:...]
    def fix_seg(m):
        sz = m.group(1) or ''
        seg = m.group(2).lower()
        inner = m.group(3)
        sz_str = f'{sz} ' if sz else ''
        return f'{sz_str}[{seg}:{inner}]'
    ops = re.sub(r'\b(byte|word|dword)?\s*([CSDE]{1}S):\[(.*?)\]', fix_seg, ops, flags=re.IGNORECASE)

    if mnem in ('call far', 'jmp far') and orig_hex.startswith('2e') and 'cs:' not in ops.lower():
        inner = ops.strip(' []')
        ops = f'[cs:{inner}]'

    # Replace segment constants
    ops = re.sub(r'\b0x34fe\b', 'SEG_DATA', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x260a\b', 'SEG_STACK', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x3206\b', 'SEG_PALETTE', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x1d5d\b', 'SEG_SCRATCH', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x3fcc\b', 'SEG_BUFFER_1', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x47cc\b', 'SEG_BUFFER_2', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x4fcc\b', 'SEG_BUFFER_3', ops, flags=re.IGNORECASE)
    ops = re.sub(r'\b0x57cc\b', 'SEG_BUFFER_4', ops, flags=re.IGNORECASE)

    # Replace 1000:xxxx branch targets
    if orig_mnem in JUMP_MNEMS:
        m = re.search(r'(?:0x)?1000:([0-9a-fA-F]{4})', ops)
        if m:
            tgt_hex = m.group(1).lower()
            lbl = get_label(tgt_hex)
            ops = lbl

    if orig_mnem == 'JMP':
        if orig_len == 3:
            mnem = 'jmp near'
        elif orig_len == 2:
            mnem = 'jmp short'

    return mnem, ops

def generate_asm(out_filename, is_standalone=False):
    with open(out_filename, 'w') as out:
        out.write('; ' + '=' * 78 + '\n')
        out.write('; CAPTAIN COMIC II: FRACTURED REALITY (1990 by Michael A. Denio)\n')
        out.write('; 100% Compilable 16-bit 8086 MS-DOS NASM Assembly Source Code\n')
        if is_standalone:
            out.write('; Output: Complete MS-DOS MZ Executable (comic2.exe)\n')
            out.write('; Assemble with: nasm -f bin comic2_standalone.asm -o comic2.exe\n')
        else:
            out.write('; Output: Code Segment Binary (comic2_code.bin, 54,736 bytes)\n')
            out.write('; Assemble with: nasm -f bin comic2_nasm.asm -o comic2_code.bin\n')
        out.write('; ' + '=' * 78 + '\n\n')

        out.write('[BITS 16]\n')

        if is_standalone:
            out.write('\n; ' + '-' * 76 + '\n')
            out.write('; MS-DOS MZ EXECUTABLE HEADER (32 paragraphs / 512 bytes)\n')
            out.write('; ' + '-' * 76 + '\n')
            out.write('section .header vstart=0\n')
            out.write("""mz_header:
    db 'MZ'                       ; Magic signature
    dw 196288 % 512               ; Bytes on last 512-byte page (192)
    dw (196288 + 511) // 512      ; Total 512-byte pages in file (384)
    dw 30                         ; Number of relocation items
    dw 32                         ; Header size in 16-byte paragraphs (512 bytes)
    dw 0x2000                     ; Minimum extra paragraphs needed
    dw 0xffff                     ; Maximum extra paragraphs needed
    dw 0x158d                     ; Initial relative SS value
    dw 0x07d0                     ; Initial SP value
    dw 0x9e6f                     ; Checksum
    dw 0x0064                     ; Initial IP value (Entry point offset)
    dw 0x0000                     ; Initial relative CS value
    dw 0x001c                     ; Offset of relocation table
    dw 0x0000                     ; Overlay number

; Relocation table: 30 entries (offset:segment)
relocation_table:
""")
            with open('original/unpacked.exe', 'rb') as f:
                f.seek(0x1c)
                for _ in range(30):
                    off, seg = struct.unpack('<HH', f.read(4))
                    out.write(f'    dw 0x{off:04x}, 0x{seg:04x}\n')
            out.write('    times 512 - ($ - mz_header) db 0\n\n')
            out.write('section .text vstart=0 follows=.header\n\n')
        else:
            out.write('[ORG 0x0000]\n\n')

        out.write('; ' + '-' * 76 + '\n')
        out.write('; SEGMENT DEFINITIONS & CONSTANTS\n')
        out.write('; ' + '-' * 76 + '\n')
        out.write('SEG_DATA        equ 0x24fe   ; Primary game data, strings, state segment\n')
        out.write('SEG_STACK       equ 0x160a   ; Initial runtime stack segment\n')
        out.write('SEG_PALETTE     equ 0x2206   ; EGA 16-color DAC palette definition table\n')
        out.write('SEG_SCRATCH     equ 0x0d5d   ; Decompression scratchpad & collision buffer\n')
        out.write('SEG_BUFFER_1    equ 0x2fcc   ; Sprite plane buffer 1\n')
        out.write('SEG_BUFFER_2    equ 0x37cc   ; Sprite plane buffer 2\n')
        out.write('SEG_BUFFER_3    equ 0x3fcc   ; Sprite plane buffer 3\n')
        out.write('SEG_BUFFER_4    equ 0x47cc   ; Sprite plane buffer 4\n\n')

        mov_ax_relocs = {
            0x01c4: 'SEG_PALETTE',
            0x0342: 'SEG_SCRATCH',
            0x0462: 'SEG_SCRATCH',
            0x766f: 'SEG_PALETTE',
            0x76d6: 'SEG_PALETTE',
            0x770c: 'SEG_PALETTE'
        }
        skip_offsets = set([0x005c, 0x005d, 0x005e, 0x005f])
        for base in mov_ax_relocs:
            skip_offsets.add(base + 1)
            skip_offsets.add(base + 2)

        pending_bytes = []
        pending_start_off = None

        def flush_bytes():
            nonlocal pending_bytes, pending_start_off
            if not pending_bytes:
                return
            for i in range(0, len(pending_bytes), 16):
                chunk = pending_bytes[i:i+16]
                hex_s = ', '.join(f'0x{b:02x}' for b in chunk)
                off_str = f'1000:{pending_start_off + i:04x}'
                out.write(f'    db {hex_s:<44} ; {off_str}\n')
            pending_bytes = []
            pending_start_off = None

        skip_until_off = None

        for line in raw_lines:
            parts = line.strip().split('\t')
            record_type = parts[0]
            off_hex = parts[1].lower()
            off_int = int(off_hex, 16)

            if skip_until_off is not None:
                if off_int < skip_until_off:
                    continue
                skip_until_off = None

            if off_int in DISASSEMBLED_OVERRIDES:
                flush_bytes()
                end_off, block_text = DISASSEMBLED_OVERRIDES[off_int]
                out.write(block_text)
                skip_until_off = end_off
                continue

            has_func = off_hex in func_labels
            has_branch = off_hex in branch_targets

            if has_func or has_branch:
                flush_bytes()
                if has_func:
                    fname, fcat, fdesc = func_labels[off_hex]
                    out.write('\n; ' + '-' * 76 + '\n')
                    out.write(f'; FUNCTION: {fname}  [{fcat}]\n')
                    out.write(f'; ADDRESS:  1000:{off_hex}\n')
                    out.write(f'; PURPOSE:  {fdesc}\n')
                    out.write('; ' + '-' * 76 + '\n')
                    out.write(f'{fname}:\n')
                elif has_branch:
                    out.write(f'loc_{off_hex}:\n')

            if off_int == 0x005c:
                flush_bytes()
                out.write('    dw SEG_DATA                                  ; 1000:005c: Relocation segment\n')
                out.write('    dw SEG_STACK                                 ; 1000:005e: Relocation segment\n')
                continue

            if off_int in mov_ax_relocs:
                flush_bytes()
                equ_name = mov_ax_relocs[off_int]
                out.write(f'    mov          ax, {equ_name:<32} ; 1000:{off_hex}: b8 (relocated segment)\n')
                continue

            if off_int in skip_offsets:
                continue

            if record_type == 'INST':
                flush_bytes()
                hex_code = parts[2]
                mnem = parts[3]
                ops = parts[4] if len(parts) > 4 else ''
                n_mnem, n_ops = nasmify(mnem, ops, hex_code)
                formatted = f'    {n_mnem:<12} {n_ops}'.rstrip()
                out.write(f'{formatted:<48} ; 1000:{off_hex}: {hex_code}\n')
            elif record_type in ('DATA', 'BYTE'):
                hex_bytes = parts[2]
                byte_vals = [int(hex_bytes[i:i+2], 16) for i in range(0, len(hex_bytes), 2)]

                if pending_start_off is None:
                    pending_start_off = off_int
                pending_bytes.extend(byte_vals)

        flush_bytes()

        if is_standalone:
            out.write('\n; ' + '-' * 76 + '\n')
            out.write('; DATA SEGMENTS & ASSETS (141,040 bytes)\n')
            out.write('; Appended at load module offset 0xD5D0\n')
            out.write('; ' + '-' * 76 + '\n')
            out.write('%include "comic2_data.asm"\n')

if __name__ == '__main__':
    generate_asm('comic2_nasm.asm', is_standalone=False)
    generate_asm('comic2_standalone.asm', is_standalone=True)
    print('Generated comic2_nasm.asm and comic2_standalone.asm successfully.')
