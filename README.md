# Ember

## Prerequisites

- CMake
- Ninja
- Emscripten
- Blender 2.8 (for integrated 3D pipeline)

### CMake and Ninja

Note: You can use any build tool other than Ninja, but Ninja is recommended due to its speed and simplicity.

An easy way to get these installed is via [MSYS2](https://www.msys2.org/):
```
pacman -S \
    mingw64/mingw-w64-x86_64-cmake \
    mingw64/mingw-w64-x86_64-ninja
```

Otherwise, they can be installed manually, just make sure that they're available on your PATH.

### Emscripten

Follow the instructions [here](https://emscripten.org/docs/getting_started/downloads.html).

Depending on which shell you're using, you may want to make different decisions about which scripts to run.

If you're using MSYS2, you may want to `source /c/emsdk/emsdk_env.sh` in your `~/.bash_profile`.

## VSCode Setup

1. Install the "C/C++" and "CMake Tools" extensions.
2. Ensure that CMake and Ninja are available in your PATH and/or you've configured VS Code to know where they are.
    - In either your global or project settings,
        set the `"cmake.cmakePath"` setting to the path to your CMake executable.
3. Using the included `cmake-tools-kits.json` as an example,
    add a new kit using the "CMake: Edit User-Local CMake Kits" command.
3. Configure the project with CMake Tools, using the Emscripten kit in Release mode.
    - This will create a `build/` directory for you.
4. Set up your C/C++ extension settings to find the compiler.
    - Normally this will happen automatically when configuring CMake, but some issues may arise.
    - If Intellisense is having trouble finding even local files,
        ensure that your C/C++ configuration provider is set to `ms-vscode.cmake-tools`.
    - If Intellisense is having trouble with files in `ext/` or system includes, you may need to bypass some settings.
        - Run the "C/C++: Edit Configurations" command and ensure that your configuration at least has these settings:
            ```json
            {
                "name": "Emscripten",
                "includePath": [
                    "${env:USERPROFILE}/.emscripten_cache/wasm-obj/include"
                ],
                "compilerPath": "C:/emsdk/upstream/emscripten/em++.bat",
                "cStandard": "c11",
                "cppStandard": "c++17",
                "intelliSenseMode": "clang-x64",
                "configurationProvider": "ms-vscode.cmake-tools"
            }
            ```

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

## Debugging

Oof.

Debugging in-browser is not really feasible. The most you'll get is a stack trace.

Once the native build is finished, it should make debugging way easier.
