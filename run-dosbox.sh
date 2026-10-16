#!/usr/bin/env bash
set -euo pipefail

# scripts/run-dosbox.sh
# Start DOSBox-X and mount $PWD/reference/original as C:
# Usage: ./scripts/run-dosbox.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

BUILD_DIR=$PWD/build
ORIGINAL_DIR="$PWD/original"
EXE_NAME="comic2.exe"

if [[ -f "$PWD/comic2.exe" ]]; then
  if [[ ! -f "$ORIGINAL_DIR/comic2.exe" ]] || [[ "$PWD/comic2.exe" -nt "$ORIGINAL_DIR/comic2.exe" ]]; then
    echo -e "${GREEN}Copying newly assembled comic2.exe to $ORIGINAL_DIR...${RESET}"
    cp "$PWD/comic2.exe" "$ORIGINAL_DIR/comic2.exe"
  fi
fi

if ! command -v dosbox-x >/dev/null 2>&1; then
  echo -e "${RED}Error: 'dosbox-x' not found in PATH. Install it (e.g. 'brew install dosbox-x').${RESET}" >&2
  exit 1
fi

# Start DOSBox, mount the assets directory as C: and switch to C:
exec dosbox-x -conf $PWD/dosbox_deterministic.conf \
  -savedir $PWD/savestates \
  -c "mount c \"$ORIGINAL_DIR\" -freesize 1024" -c "c:" -c "$EXE_NAME"
