# Makefile for Captain Comic II: Fractured Reality
#
# NOTE: comic2.asm is an IDA Pro MASM-style disassembly and cannot be
# assembled with NASM. The goal of this project is a C++ reimplementation,
# not a reassemblable binary. This Makefile is retained for the djlink/bindiff
# tools only, which may be useful for future structural binary comparison.

# Build the djlink/bindiff tools from source
tools: tools/djlink/djlink tools/djlink/bindiff

tools/djlink/djlink tools/djlink/bindiff:
	$(MAKE) -C tools/djlink djlink bindiff

clean-tools:
	$(MAKE) -C tools/djlink clean

.PHONY: tools clean-tools
