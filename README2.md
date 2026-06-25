# Phase 7 Reimplementation

This directory contains the C++ reimplementation of Captain Comic II: Fractured Reality.

## Documentation

See the [`docs/`](docs/) folder for comprehensive documentation:

- **[Quick Start](docs/SDL2_QUICKSTART.md)** - Get started with SDL2 visual presenter
- **[SDL2 Integration](docs/SDL2_INTEGRATION.md)** - Complete SDL2 documentation
- **[Phase 7.6 Summary](docs/PHASE_7.6_SUMMARY.md)** - Rendering implementation details
- **[All Docs](docs/README.md)** - Documentation index

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
- Test tree under `tests/` covering decoders, renderer primitives, and dispatcher stage ordering.
- Dispatcher tests also validate deterministic replay for fixed input sequences.

## Build

### Headless (Default)
```bash
cmake -S . -B build
cmake --build build
```

### With SDL2 Visual Output
```bash
# Install SDL2 first (e.g., brew install sdl2)
cmake -S . -B build
cmake --build build
```

See [docs/SDL2_QUICKSTART.md](docs/SDL2_QUICKSTART.md) for details.

## Run tests

```bash
ctest --test-dir build --output-on-failure
```

All tests pass in both headless and SDL2 modes.

## Run validator against original files

```bash
./build/comic2_reimpl reference/original
```

Expected: each FRPAK file decodes with row span `0x1f40`.

## Run renderer demo

```bash
# Headless mode (prints frame data)
./build/comic2_renderer_demo

# Visual mode (shows window, requires SDL2 build)
./build/comic2_renderer_demo

Press ESC to close the demo window.