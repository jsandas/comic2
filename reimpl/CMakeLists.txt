cmake_minimum_required(VERSION 3.20)
project(comic2_reimpl LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optional SDL2 support for visual presentation
option(ENABLE_SDL2 "Enable SDL2 presenter for visual output" OFF)

if(ENABLE_SDL2)
    find_package(SDL2 REQUIRED)
    if(SDL2_FOUND)
        message(STATUS "SDL2 found - enabling visual presenter")
        add_definitions(-DCOMIC2_USE_SDL2)
    else()
        message(FATAL_ERROR "SDL2 was requested but not found. Install SDL2-dev to enable.")
    endif()
endif()

add_library(comic2_core
    src/default_handlers.cpp
    src/dispatcher.cpp
    src/entity_runtime.cpp
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

if(ENABLE_SDL2 AND SDL2_FOUND)
    target_link_libraries(comic2_core PRIVATE SDL2::SDL2)
endif()

add_executable(comic2_reimpl
    src/main.cpp
)

target_link_libraries(comic2_reimpl
    PRIVATE
        comic2_core
)

# Optional renderer demo executable
add_executable(comic2_renderer_demo src/renderer_demo.cpp)
target_link_libraries(comic2_renderer_demo PRIVATE comic2_core)

add_executable(comic2_unit_tests
    tests/dispatcher_tests.cpp
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

if(ENABLE_SDL2 AND SDL2_FOUND)
    target_link_libraries(comic2_unit_tests PRIVATE SDL2::SDL2)
endif()

if(BUILD_TESTING)
    add_test(NAME comic2_unit_tests COMMAND comic2_unit_tests)
endif()
