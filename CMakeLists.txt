cmake_minimum_required(VERSION 3.20)
project(comic2_reimpl LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure consistent runtime library on Windows
if(MSVC)
    # Use static runtime to match vcpkg's x64-windows-static triplet
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Optional SDL2 support for visual presentation
option(ENABLE_SDL2 "Enable SDL2 presenter for visual output" OFF)

if(ENABLE_SDL2)
    # Platform-aware SDL2 detection (matches captain-comic-refactor approach)
    if(WIN32)
        # Windows: Use CONFIG mode for vcpkg compatibility
        find_package(SDL2 CONFIG REQUIRED)
        if(TARGET SDL2::SDL2)
            set(SDL2_TARGET SDL2::SDL2)
        elseif(TARGET SDL2::SDL2-static)
            set(SDL2_TARGET SDL2::SDL2-static)
        else()
            message(FATAL_ERROR "SDL2 was requested but neither SDL2::SDL2 nor SDL2::SDL2-static target found")
        endif()
    else()
        # Unix-like: Use CMake's find_package (works with Homebrew and system installs)
        find_package(SDL2 REQUIRED)
        if(TARGET SDL2::SDL2)
            set(SDL2_TARGET SDL2::SDL2)
        else()
            message(FATAL_ERROR "SDL2 was requested but SDL2::SDL2 target not found")
        endif()
    endif()
    
    message(STATUS "SDL2 found - enabling visual presenter (target: ${SDL2_TARGET})")
    add_definitions(-DCOMIC2_USE_SDL2)
endif()

add_library(comic2_core
    src/default_handlers.cpp
    src/dispatcher.cpp
    src/entity_runtime.cpp
    src/game_state.cpp
    src/player_controller.cpp
    src/projectiles.cpp
    src/renderer.cpp
    src/renderer_validation.cpp
    src/room_loader.cpp
    src/resource_loader.cpp
    src/tile_collision.cpp
)

target_include_directories(comic2_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(ENABLE_SDL2 AND SDL2_TARGET)
    target_link_libraries(comic2_core PUBLIC ${SDL2_TARGET})
endif()

add_executable(comic2_reimpl
    src/main.cpp
)

target_link_libraries(comic2_reimpl
    PRIVATE
        comic2_core
)

# Optional renderer demo executable (only when SDL2 is enabled)
if(ENABLE_SDL2 AND SDL2_TARGET)
    add_executable(comic2_renderer_demo src/renderer_demo.cpp)
    if(WIN32 AND TARGET SDL2::SDL2main)
        target_link_libraries(comic2_renderer_demo PRIVATE SDL2::SDL2main comic2_core)
    elseif(WIN32 AND TARGET SDL2::SDL2main-static)
        target_link_libraries(comic2_renderer_demo PRIVATE SDL2::SDL2main-static comic2_core)
    else()
        target_link_libraries(comic2_renderer_demo PRIVATE comic2_core)
    endif()
endif()

add_executable(comic2_unit_tests
    tests/dispatcher_tests.cpp
    tests/integration_gates_tests.cpp
    tests/player_controller_tests.cpp
    tests/renderer_tests.cpp
    tests/renderer_validation_tests.cpp
    tests/subsystem_scaffold_tests.cpp
    tests/tile_collision_tests.cpp
    tests/unit_tests_main.cpp
)

target_link_libraries(comic2_unit_tests
    PRIVATE
        comic2_core
)

# No need to link SDL2 again - inherited from comic2_core via PUBLIC

if(BUILD_TESTING)
    add_test(NAME comic2_unit_tests COMMAND comic2_unit_tests)
endif()
