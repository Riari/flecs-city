# flecs-city

This is a C++ sandbox project for exploring Flecs v4 and raylib (as well as other libraries that I'll inevitably add as it grows). The long term goal is to develop a basic city simulation with vehicles. It won't necessarily be a game, but will contain game-like systems and features. I'll probably also use it to implement other things that I want to explore in the context of Flecs, such as networking.

## Requirements

* [CMake](https://github.com/Kitware/CMake) 3.20 or newer
* [Ninja](https://github.com/ninja-build/ninja)
* [vcpkg](https://github.com/microsoft/vcpkg) (included as a submodule)

## Building

### Step 1: Bootstrap vcpkg
Run `./vcpkg/bootstrap-vcpkg.sh` (`./vcpkg/bootstrap-vcpkg.bat` on Windows) to bootstrap it.

### Step 2: Generate build configuration
On Linux, the vcpkg triplet needs to be explicitly set to `x64-linux-dynamic` when running cmake: `cmake --preset=vcpkg -DVCPKG_TARGET_TRIPLET=x64-linux-dynamic` (alternatively, set the `VCPKG_TARGET_TRIPLET` environment variable before running `cmake --preset=vcpkg`).

On Windows, run `cmake --preset=vcpkg`.

### Step 3: Build
Run `cmake --build build`.
