# Captain Comic II Reimplementation Documentation

This folder contains documentation for the Captain Comic II reimplementation project.

## Quick Links

- **[SDL2 Quick Start](SDL2_QUICKSTART.md)** - Get started with SDL2 visual presenter in 5 minutes
- **[SDL2 Integration Guide](SDL2_INTEGRATION.md)** - Comprehensive SDL2 documentation
- **[Phase 7.6 Summary](PHASE_7.6_SUMMARY.md)** - Implementation details for rendering phase

## Project Documentation

### Root Directory
- **[PLAN.md](../PLAN.md)** - Master implementation plan and roadmap
- **[README.md](../README.md)** - Project overview and build instructions

### Core Documentation
- **[ANNOTATION_GUIDE.md](../ANNOTATION_GUIDE.md)** - Assembly annotation standards
- **[REVERSE_ENGINEERING_GUIDE.md](../REVERSE_ENGINEERING_GUIDE.md)** - Reverse engineering methodology
- **[FUNCTION_MAP.md](../FUNCTION_MAP.md)** - Function address mappings
- **[CORE_ENGINE.md](../CORE_ENGINE.md)** - Game engine architecture

### Implementation Notes
- **[IMPLEMENTATION_NOTES_7_5.md](../IMPLEMENTATION_NOTES_7_5.md)** - Phase 7.5 details

## SDL2 Visual Presenter

The optional SDL2 presenter provides visual output for the rendering system.

### Quick Start
```bash
# Install SDL2
brew install sdl2

# Build with SDL2 support
cd reimpl
cmake -S . -B build -DENABLE_SDL2=ON
cmake --build build

# Run demo
./build/comic2_renderer_demo
```

See **[SDL2_QUICKSTART.md](SDL2_QUICKSTART.md)** for more details.

### Features
- ✅ Optional dependency (disabled by default)
- ✅ 4-plane EGA to RGBA conversion
- ✅ Standard EGA 16-color palette
- ✅ 2x scaling (640x480 window)
- ✅ VSync and hardware acceleration
- ✅ Headless testing support

See **[SDL2_INTEGRATION.md](SDL2_INTEGRATION.md)** for comprehensive documentation.

## Build System

### Standard Build (Headless)
```bash
cd reimpl
cmake -S . -B build
cmake --build build
ctest
```

### With SDL2
```bash
cd reimpl
cmake -S . -B build -DENABLE_SDL2=ON
cmake --build build
ctest
```

## Testing

All tests pass in both headless and SDL2 modes:

```bash
cd reimpl/build
ctest --output-on-failure
```

## Architecture

### Renderer Abstraction
- `IFramePresenter` - Abstract presenter interface
- `MemoryFramePresenter` - Headless presenter (default)
- `Sdl2FramePresenter` - Visual presenter (optional)

### Core Modules
- `resource_loader` - RLE decoding and resource loading
- `renderer` - EGA surface and blitting
- `dispatcher` - Game loop and stage management
- `player_controller` - Player movement and physics
- `tile_collision` - Tile-based collision detection
- `entity_runtime` - Entity activation and updates
- `projectiles` - Projectile spawning and updates
- `room_loader` - Room data loading and setup

## Contributing

When adding new features:
1. Update relevant documentation
2. Add tests (must pass in headless mode)
3. Update PLAN.md with progress
4. Follow existing code style

## License

See main repository LICENSE file.

SDL2 is licensed under zlib/libpng license.
