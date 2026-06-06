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

quality: cppcheck clang-format

cppcheck:
	@docker run --name cppcheck_container --rm -v ${PWD}:/workspace \
		--workdir /workspace \
		alpine:edge \
		sh -c "apk add --no-cache cppcheck > /dev/null 2>&1; \
		cppcheck --enable=all \
		--inconclusive \
		--suppress=missingIncludeSystem \
		--check-level=exhaustive \
		--error-exitcode=1 \
		--inline-suppr \
		-I include \
		src tests"

clang-format:
	@if ! command -v clang-format > /dev/null; then \
		echo "Error: clang-format is not installed." >&2; \
		exit 1; \
	fi
	@find src include tests \
        \( -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.h" \) \
        -print0 | xargs -0 clang-format --dry-run --Werror
