# Phase 7 Reimplementation Starter

This directory contains the initial C++ scaffolding for Phase 7.

## What is implemented

- Confirmed data layouts from Phase 6 in `include/comic2/types.hpp`.
- Renderer abstraction baseline in `include/comic2/renderer.hpp` and `src/renderer.cpp`.
- Dispatcher skeleton and state containers in `include/comic2/dispatcher.hpp`, `include/comic2/game_state.hpp`, and `src/dispatcher.cpp`.
- Default stage handler hooks for input and physics in `include/comic2/default_handlers.hpp` and `src/default_handlers.cpp`.
- Resource format contracts in `include/comic2/resource_formats.hpp`.
- Loader/decoder implementations in `src/resource_loader.cpp`:
  - Signed-RLE decode (FRDATA-style payloads)
  - 4-plane EGA packet decode (FRPAK-style payloads)
- Validation executable in `src/main.cpp` that parses `FRPAK.001`..`FRPAK.007`.
- Test tree under `tests/` covering decoders and renderer primitives.
- Test tree under `tests/` covering decoders, renderer primitives, and dispatcher stage ordering.
- Dispatcher tests also validate deterministic replay for fixed input sequences.

## Build

```bash
cmake -S reimpl -B reimpl/build
cmake --build reimpl/build
```

## Run tests

```bash
ctest --test-dir reimpl/build --output-on-failure
```

## Run validator against original files

```bash
./reimpl/build/comic2_reimpl reference/original
```

Expected: each FRPAK file decodes with row span `0x1f40`.
