#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

usage(){
    echo "Usage: $0 [-l] [-p preset] [-c] [-h]"
    echo "  -l              List available CMake workflow presets."
    echo "  -p preset       Specify the CMake workflow preset to use."
    echo "  -c              Clear the CMake cache before running the selected preset."
    echo "  -h              Display this message."
    exit 1
}

list_presets(){
    cmake --workflow --list-presets
    exit 0
}

PRESET=""
CLEAR_CACHE=false

while getopts 'lp:ch' OPTION; do
    case ${OPTION} in
        l) list_presets;;
        p) PRESET=$OPTARG;;
        c) CLEAR_CACHE=true;;
        *) usage;;
    esac
done

if [ "$CLEAR_CACHE" = true ]; then
    echo "Clearing CMake cache files..."
    rm -rf build/CMakeCache.txt build/CMakeFiles || {
        echo "Error: Failed to clear cache files."
        exit 2
    }
    echo "CMake cache files cleared."
fi

if [ -z "$PRESET" ]; then
    echo "Error: No preset specified."
    usage
fi

echo "Selected preset: ${PRESET}"

cmake --workflow --preset ${PRESET} &&
echo "Workflow completed with preset '${PRESET}'." || {
    exit 3
}

if [[ "$PRESET" == *"examples"* ]]; then
    echo "    ========== Running Examples ==========   "
    cd build/examples
    echo "========== Shared Library Examples =========="
    ./magicxx_examples || {
        exit 4
    }
    echo "========== Static Library Examples =========="
    ./magicxx_examples_static || {
        exit 5
    }
fi
