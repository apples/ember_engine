# Ember

## Prerequisites

- CMake
- Ninja
- Emscripten
- Blender 2.8 (for integrated 3D pipeline)

### CMake and Ninja

An easy way to get these installed is via [MSYS2](https://www.msys2.org/):
```
pacman -S \
    mingw64/mingw-w64-x86_64-cmake \
    mingw64/mingw-w64-x86_64-ninja
```

Otherwise, they can be installed manually, just make sure that they're available on your PATH.

Emscripten Windows:
```
cd C:\
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
emsdk install latest
```

## VSCode Setup

1. Install the "C/C++" and "CMake Tools" extensions.
2. In either your global or project settings, set the `"cmake.cmakePath"` setting to the path to your CMake executable.
3. 

## Building

### Option 1 - Command line scripts
1. Make sure you're running in an Emscripten environment, latest version
2. Run `./configure_all.sh` to generate the build scripts in `build/`
3. Run `./build.sh`, it will build release by default

### Option 2 - VSCode CMake Tools extension
1. Using the kit in `cmake-tools-kits.json` as a guide, ensure that the proper kits are added to your CMake Tools config
2. That's it I guess, use CMake Tools to build

## Running

Build output is in `build/<target>/www`, host this directory as a static site.

You can use the included `serve.sh` to automatically download nginx and host the release build.
Edit `nginx.conf` as desired.
