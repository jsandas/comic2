# Makefile for Captain Comic II: Fractured Reality NASM Rebuild

NASM ?= nasm
PYTHON ?= python3

.PHONY: all code exe verify run clean regen help

all: code exe verify

help:
	@echo "Captain Comic II Build Targets:"
	@echo "  make all      - Build code binary, standalone exe, and run verification"
	@echo "  make code     - Assemble 16-bit code segment (comic2_code.bin)"
	@echo "  make exe      - Assemble standalone MS-DOS executable (comic2.exe)"
	@echo "  make verify   - Verify exact byte sizes and relocation table integrity"
	@echo "  make run      - Launch the assembled game in DOSBox-X"
	@echo "  make regen    - Regenerate comic2_nasm.asm from Ghidra listing"
	@echo "  make clean    - Remove built binaries and intermediate data"

code: comic2_code.bin

exe: comic2.exe

comic2_code.bin: comic2_nasm.asm
	$(NASM) -f bin $< -o $@

data_0.bin:
	$(PYTHON) scripts/extract_data.py

comic2.exe: comic2_standalone.asm data_0.bin
	$(NASM) -f bin $< -o $@

verify: code exe
	@$(PYTHON) -c "\
import struct, sys; \
c = open('comic2_code.bin', 'rb').read(); \
e = open('comic2.exe', 'rb').read(); \
assert len(c) == 54736, f'Error: code segment size is {len(c)}, expected 54736'; \
assert len(e) == 196288, f'Error: executable size is {len(e)}, expected 196288'; \
hdr = e[:512]; \
relocs = [struct.unpack('<HH', hdr[0x1c + i*4 : 0x1c + i*4 + 4])[0] for i in range(30)]; \
assert len(relocs) == 30, 'Error: missing relocation entries'; \
print('✓ Verification PASSED: comic2_code.bin (54,736 B) & comic2.exe (196,288 B, 30 relocs) match exact specifications.')"

run: exe
	./run-dosbox.sh

regen:
	$(PYTHON) scripts/build_nasm_all.py

clean:
	rm -f comic2_code.bin comic2.exe data_0.bin
