# SDL2 Visual Presenter

This document describes the optional SDL2 presenter integration for visual output in the Captain Comic II reimplementation.

## Overview

The rendering system uses a presenter abstraction (`IFramePresenter`) that allows swapping between different output backends:

- **MemoryFramePresenter** (default): Headless presenter that stores frames in memory for testing and validation
- **Sdl2FramePresenter** (optional): Visual presenter using SDL2 for windowed display with EGA-to-SDL conversion

## Building with SDL2

### Prerequisites

Install SDL2 development libraries:

**macOS (Homebrew):**
```bash
brew install sdl2
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
```

**Fedora:**
```bash
sudo dnf install SDL2-devel
```

**Arch Linux:**
```bash
sudo pacman -S sdl2
```

### Build Configuration

**With SDL2 support:**
```bash
cd reimpl
rm -rf build && mkdir build && cd build
cmake -DENABLE_SDL2=ON ..
make -j8
```

**Without SDL2 (headless, default):**
```bash
cd reimpl
rm -rf build && mkdir build && cd build
cmake -DENABLE_SDL2=OFF ..  # or omit the flag (default is OFF)
make -j8
```

## Usage

### Renderer Demo

A demo executable is provided to test the visual presenter:

```bash
# With SDL2 - shows a window with test pattern for 5 seconds
./comic2_renderer_demo

# Without SDL2 - runs in headless mode and prints frame data
./comic2_renderer_demo
```

### Integration with Game Loop

The presenter is used through the `IFramePresenter` interface:

```cpp
#ifdef COMIC2_USE_SDL2
    std::unique_ptr<comic2::IFramePresenter> presenter = 
        std::make_unique<comic2::Sdl2FramePresenter>(640, 480);
#else
    std::unique_ptr<comic2::IFramePresenter> presenter = 
        std::make_unique<comic2::MemoryFramePresenter>();
#endif

// Use presenter throughout the game loop
presenter->present(frame);
```

## Technical Details

### EGA to SDL Conversion

The `Sdl2FramePresenter` converts 4-plane EGA data to 32-bit RGBA:

1. **EGA Format**: 320x200 resolution, 4 planes, 16 colors (4 bits per pixel)
2. **SDL Format**: 640x480 window (2x scaling), 32-bit RGBA texture
3. **Palette**: Standard EGA 16-color palette mapped to RGBA values

### EGA Palette

The presenter uses the standard EGA palette:

| Index | Color       | RGB Values     |
|-------|-------------|----------------|
| 0     | Black       | (0, 0, 0)      |
| 1     | Blue        | (0, 0, 200)    |
| 2     | Green       | (0, 200, 0)    |
| 3     | Cyan        | (0, 200, 200)  |
| 4     | Red         | (200, 0, 0)    |
| 5     | Magenta     | (200, 0, 200)  |
| 6     | Brown       | (200, 100, 0)  |
| 7     | Light Gray  | (200, 200, 200)|
| 8     | Dark Gray   | (100, 100, 100)|
| 9     | Light Blue  | (50, 50, 255)  |
| 10    | Light Green | (50, 255, 50)  |
| 11    | Light Cyan  | (50, 255, 255) |
| 12    | Light Red   | (255, 50, 50)  |
| 13    | Light Magenta| (255, 50, 255)|
| 14    | Yellow      | (255, 255, 50) |
| 15    | White       | (255, 255, 255)|

### Features

- **VSync enabled**: Prevents screen tearing
- **Hardware acceleration**: Uses GPU when available
- **Window scaling**: 2x scaling (640x480 window for 320x200 content)
- **Event handling**: Supports ESC key or window close to exit
- **Memory safety**: RAII cleanup of SDL resources

## Testing

### Unit Tests

All existing unit tests work in both SDL2 and headless modes:

```bash
cd reimpl/build
ctest --output-on-failure
```

When SDL2 is enabled, additional tests verify the `Sdl2FramePresenter` can present frames without errors.

### Visual Testing

Run the renderer demo to visually verify the presenter:

```bash
cd reimpl/build
./comic2_renderer_demo
```

The demo displays a test pattern for 5 seconds, or until ESC is pressed.

## Architecture

### Interface

```cpp
class IFramePresenter {
public:
    virtual ~IFramePresenter() = default;
    virtual void present(const EgaPlanarSurface& frame) = 0;
};
```

### Implementations

**MemoryFramePresenter**:
- Stores last presented frame in memory
- Thread-safe for testing
- No external dependencies
- Used by default in unit tests

**Sdl2FramePresenter**:
- Creates SDL window and renderer
- Converts 4-plane EGA to RGBA
- Handles SDL event loop
- Optional dependency (disabled by default)

## Future Enhancements

Potential improvements for the SDL2 presenter:

1. **Configurable window size**: Command-line arguments for window dimensions
2. **Multiple scaling modes**: Nearest-neighbor, linear, etc.
3. **Fullscreen support**: Toggle between windowed and fullscreen
4. **Custom palettes**: Load palette from game data
5. **Screenshot capture**: Save frames to disk
6. **Video recording**: Record gameplay sessions
7. **Input handling**: Keyboard/joystick input through SDL
8. **Sound output**: SDL_mixer integration for audio

## Troubleshooting

### Build Errors

**"SDL2/SDL.h not found"**:
- Install SDL2 development libraries (see Prerequisites)
- Verify with: `pkg-config --cflags sdl2`

**"Undefined references to SDL functions"**:
- Ensure SDL2 is properly linked
- Check CMake output for "SDL2 found" message

### Runtime Errors

**"SDL_Init failed"**:
- Check SDL2 installation
- Verify DISPLAY environment variable (Linux)

**Window appears blank**:
- Verify EGA surface data is being populated
- Check palette conversion logic

## License

SDL2 is licensed under zlib/libpng license. See SDL2 documentation for details.
