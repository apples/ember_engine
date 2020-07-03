#!/usr/bin/env bash
set -e

COMMON="-G Ninja"
EMSCRIPTEN="-DCMAKE_TOOLCHAIN_FILE=${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
RELEASE="-DCMAKE_BUILD_TYPE=Release"
DEBUG="-DCMAKE_BUILD_TYPE=Debug"

function mkdircd () { mkdir -p $1 && cd $1; }

(mkdircd build/release && cmake ../.. $COMMON $EMSCRIPTEN $RELEASE)
(mkdircd build/debug && cmake ../.. $COMMON $EMSCRIPTEN $DEBUG)
