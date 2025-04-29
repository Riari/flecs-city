cmake_minimum_required(VERSION 3.20)

project(
    flecs-city
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG v4.0.5
)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.2
)

# flecs options
set(FLECS_STATIC OFF CACHE BOOL "" FORCE)
set(FLECS_SHARED ON CACHE BOOL "" FORCE)

# raylib options
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# spdlog options
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(flecs raylib spdlog)

add_subdirectory(src)
