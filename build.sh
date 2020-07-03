#!/usr/bin/env bash
set -e

TARGET=release

while [[ $# -ge 1 ]]
do
    case $1 in
        release)
            ;;
        debug)
            TARGET=debug
            ;;
        *)
            echo "ERROR: Unknown target '$1'"
            exit 1
            ;;
    esac
    shift
done

ninja -C build/${TARGET}
