#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

usage(){
    echo "Usage: $0 [-l] [-p preset] [-h]"
    echo "  -l              List available CMake workflow presets."
    echo "  -p preset       Specify the CMake workflow preset to use."
    echo "  -h              Display this message."
    exit 1
}

list_presets(){
    cmake --workflow --list-presets
    exit 0
}

PRESET=""

while getopts 'lp:h' OPTION; do
    case ${OPTION} in
        l) list_presets;;
        p) PRESET=$OPTARG;;
        *) usage;;
    esac
done

if [ -z "$PRESET" ]; then
    echo "Error: No preset specified."
    usage
fi

echo "Selected preset: ${PRESET}"

cmake --workflow --preset ${PRESET} &&
echo "Workflow completed with preset '${PRESET}'." || {
    exit 2
}

if [[ "$PRESET" == *"examples"* ]]; then
    echo "Running examples..."
    cd build/${PRESET}/examples && ./magicxx_examples || {
        exit 3
    }
fi
