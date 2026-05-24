cmake_minimum_required(VERSION 3.20)
project(comic2_reimpl LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(comic2_core
    src/default_handlers.cpp
    src/dispatcher.cpp
    src/renderer.cpp
    src/resource_loader.cpp
)

target_include_directories(comic2_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(comic2_reimpl
    src/main.cpp
)

target_link_libraries(comic2_reimpl
    PRIVATE
        comic2_core
)

add_executable(comic2_unit_tests
    tests/dispatcher_tests.cpp
    tests/renderer_tests.cpp
    tests/resource_loader_tests.cpp
)

target_link_libraries(comic2_unit_tests
    PRIVATE
        comic2_core
)

if(BUILD_TESTING)
    add_test(NAME comic2_unit_tests COMMAND comic2_unit_tests)
endif()
