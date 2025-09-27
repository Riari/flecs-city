# flecs-city

This is a C++ sandbox project for exploring Flecs v4 and raylib (as well as other libraries that I'll inevitably add as it grows). The long term goal is to develop a basic city simulation with vehicles. It won't necessarily be a game, but will contain game-like systems and features. I'll probably also use it to implement other things that I want to explore in the context of Flecs, such as networking.

## Requirements

* [CMake](https://github.com/Kitware/CMake) 3.20 or newer
* [Ninja](https://github.com/ninja-build/ninja)
* [vcpkg](https://github.com/microsoft/vcpkg) (included as a submodule)

## Building

Run `./vcpkg/bootstrap-vcpkg.sh` (`./vcpkg/bootstrap-vcpkg.bat` on Windows) to bootstrap it.

Run `cmake --preset=vcpkg` to generate the build configuration.

Run `cmake --build build` to build.
