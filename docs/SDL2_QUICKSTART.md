# Quick Start: SDL2 Visual Presenter

## TL;DR

```bash
# Install SDL2
brew install sdl2  # macOS
# or
sudo apt-get install libsdl2-dev  # Ubuntu/Debian

# Build with SDL2 support
cmake -S . -B build
cmake --build build

# Run demo (shows window with test pattern)
./build/comic2_renderer_demo
```

## Build Options

### Headless (Default - No SDL2)
```bash
cmake -S . -B build
cmake --build build
```
- ✅ Fast builds
- ✅ No dependencies
- ✅ Perfect for CI/CD
- ✅ All unit tests pass

### With Visual Output (SDL2)
```bash
cmake -S . -B build
cmake --build build
```
- ✅ Visual rendering
- ✅ Windowed mode (640x480)
- ✅ VSync enabled
- ✅ All unit tests pass + SDL2 tests

## Usage Examples

### In Code

```cpp
#include "comic2/renderer.hpp"

#ifdef COMIC2_USE_SDL2
    // Visual presenter
    auto presenter = std::make_unique<comic2::Sdl2FramePresenter>(640, 480);
#else
    // Headless presenter
    auto presenter = std::make_unique<comic2::MemoryFramePresenter>();
#endif

// Use presenter
presenter->present(frame);
```

### Running Tests

```bash
# Both modes work identically for testing
cd build
ctest --output-on-failure
```

### Running Demo

```bash
# Headless mode - prints frame data
./comic2_renderer_demo

# SDL2 mode - shows window for 5 seconds
./comic2_renderer_demo
# Press ESC to exit early
```

## Troubleshooting

**Problem:** `SDL2/SDL.h not found`
```bash
# Solution: Install SDL2
brew install sdl2  # macOS
sudo apt-get install libsdl2-dev  # Ubuntu
```

**Problem:** Build fails with SDL2 errors
```bash
# Solution: Clean rebuild
rm -rf build
cmake -S . -B build
cmake --build build
```

**Problem:** Window appears but is blank
- Verify frame data is being populated
- Check EGA surface has non-zero pixel data

## Features

- **Automatic fallback**: Code compiles with or without SDL2
- **Same interface**: `IFramePresenter` abstraction
- **No breaking changes**: All existing code works unchanged
- **Testable**: Unit tests work in both modes
- **Documented**: See `SDL2_INTEGRATION.md` for details

## Next Steps

1. Load actual game assets (sprites, tilemaps)
2. Connect game loop to presenter
3. Implement input handling
4. Add sound support

See `PLAN.md` Phase 7.7 for integration roadmap.
