#!/usr/bin/env python3
"""
extract_data.py - Extracts data_0.bin (the 141,040-byte data segment)
from original/unpacked.exe to allow comic2_standalone.asm to assemble.
"""
import os
import sys

EXE_PATHS = ['original/unpacked.exe', 'original/Fr.exe', 'unpacked.exe']

def main():
    target_exe = None
    for p in EXE_PATHS:
        if os.path.exists(p):
            target_exe = p
            break
    
    if not target_exe:
        print(f"Error: Original game executable not found in: {EXE_PATHS}", file=sys.stderr)
        print("Please place original/unpacked.exe in the repository.", file=sys.stderr)
        sys.exit(1)

    with open(target_exe, 'rb') as f:
        # Load module starts after 512-byte header; code segment is 54,736 bytes
        f.seek(512 + 54736)
        data = f.read(141040)

    if len(data) != 141040:
        print(f"Warning: Extracted {len(data)} bytes, expected 141040 bytes.", file=sys.stderr)

    with open('data_0.bin', 'wb') as f:
        f.write(data)

    print(f"Successfully extracted data_0.bin ({len(data)} bytes) from {target_exe}.")

if __name__ == '__main__':
    main()
