#!/bin/bash

# C++ Code Review Agent Script
# Usage: ./tools/review_agent.sh <source_dir>

SOURCE_DIR=$1

if [ -z "$SOURCE_DIR" ]; then
    echo "Usage: $0 <source_dir>"
    exit 1
fi

echo "=== C++ Code Review Starting ==="
echo "Target directory: $SOURCE_DIR"
echo "-------------------------------"

REPORT_FILE="review_report.md"
echo "# Code Review Report" > "$REPORT_FILE"
echo "Generated on: $(date)" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

# 1. Static Analysis: cppcheck
if command -v cppcheck >/dev/null 2>&1; then
    echo "[+] Running cppcheck..."
    echo "## Static Analysis (cppcheck)" >> "$REPORT_FILE"
    cppcheck --enable=all --inconclusive "$SOURCE_DIR" >> "$REPORT_FILE" 2>&1 || true
else
    echo "[!] cppcheck not found. Skipping."
fi

# 2. Static Analysis: clang-format
if command -v clang-format >/dev/null 2>&1; then
    echo "[+] Running clang-format..."
    echo "## Static Analysis (clang-format)" >> "$REPORT_FILE"
    # Note: clang-format often requires a compilation database (compile_commands.json)
    # For simplicity, we'll try to run it on individual files if possible.
    find "$SOURCE_DIR" -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.cxx" | while read -r file; do
        echo "Checking $file..."
        clang-format "$file" -- >> "$REPORT_FILE" 2>&1 || true
    done
else
    echo "[!] clang-format not found. Skipping."
fi

# 3. Compiler Warnings (as a fallback)
echo "[+] Running compiler-based check (g++ warnings)..."
echo "## Compiler Warnings (g++)" >> "$REPORT_FILE"
# We'll attempt to compile the files in the source dir just to see warnings.
# We won't actually link them into an executable here to avoid link errors.
find "$SOURCE_DIR" -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" | while read -r file; do
    g++ -Wall -Wextra -Wpedantic -Werror -c "$file" -o /dev/null >> "$REPORT_FILE" 2>&1 || true
done

# 4. Dynamic Analysis (Sanitizers)
# This part requires a build system. We'll look for a Makefile or CMake.
if [ -d "reimpl/build" ]; then
    echo "[+] Attempting to run tests with Sanitizers..."
    echo "## Dynamic Analysis (Sanitizers)" >> "$REPORT_FILE"
    # Try to run existing tests if they are built. 
    # This is a simplified approach.
    if [ -f "reimpl/build/cmake_install.cmake" ]; then
        echo "Found CMake build directory. Running tests..."
        cd reimpl/build && make test >> "../../$REPORT_FILE" 2>&1 || true
        cd ../..
    fi
else
    echo "[!] No build directory found for dynamic analysis."
fi

echo "-------------------------------"
echo "Review complete. Report generated: $REPORT_FILE"
echo "=== C++ Code Review Finished ==="
