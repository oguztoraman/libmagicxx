#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR="$(dirname -- "$(realpath "${BASH_SOURCE[0]}")")"
cd -- "${SCRIPT_DIR}/.."

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
    case "${OPTION}" in
        l) list_presets ;;
        p) PRESET="${OPTARG}" ;;
        c) CLEAR_CACHE=true ;;
        *) usage ;;
    esac
done

if [[ -z "${PRESET}" ]]; then
    echo "Error: No preset specified."
    usage
fi

echo "Selected preset: ${PRESET}."

args=(--workflow --preset "${PRESET}")
if [[ "${CLEAR_CACHE}" == true ]]; then
    args+=(--fresh)
fi
cmake "${args[@]}" || exit 2

echo "Workflow completed with preset: '${PRESET}'."

if [[ "${PRESET}" == *"examples"* ]]; then
    echo "    ========== Running Examples ==========   "
    pushd build/examples > /dev/null || exit 3
    if [[ -x ./magicxx_examples ]]; then
        echo "========== Shared Library Examples =========="
        ./magicxx_examples || exit 4
    else
        echo "Shared library examples not found."
    fi

    if [[ -x ./magicxx_examples_static ]]; then
        echo "========== Static Library Examples =========="
        ./magicxx_examples_static || exit 5
    else
        echo "Static library examples not found."
    fi
    popd > /dev/null || true
fi
