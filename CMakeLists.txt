cmake_minimum_required(VERSION 3.20)

if(LINUX AND NOT APPLE)
    set(VCPKG_TARGET_TRIPLET "x64-linux-dynamic" CACHE STRING "")
endif()

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

set(VCPKG_LIBRARY_LINKAGE dynamic)

# General options
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# flecs options
set(FLECS_STATIC OFF CACHE BOOL "" FORCE)
set(FLECS_SHARED ON CACHE BOOL "" FORCE)

# glfw options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# raylib options
set(USE_EXTERNAL_GLFW "ON" CACHE STRING "" FORCE)

# spdlog options
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)

find_package(args CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(src)
