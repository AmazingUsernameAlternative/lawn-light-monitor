cmake_minimum_required(VERSION 3.16)
project(lawn_light_monitor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings on. Treat them as errors in CI for clean builds.
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core library — pure C++, no hardware dependencies.
# Builds on any host (Linux CI runner, macOS, Windows).
add_library(lawn_core STATIC
    src/core/conversion.cpp
    src/core/dli_accumulator.cpp
    src/core/species_verdict.cpp
)
target_include_directories(lawn_core PUBLIC src/core)

# Tests live behind an option so the same CMake can be reused if we ever
# wrap the core into a non-test consumer.
option(LAWN_BUILD_TESTS "Build unit and functional tests" ON)
if(LAWN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
