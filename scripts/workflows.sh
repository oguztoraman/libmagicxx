#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only
#
# @file workflows.sh
# @brief CMake workflow preset runner with build cache management.
#
# This script provides a convenient wrapper around CMake's workflow preset
# system. It simplifies the process of running predefined build, test, and
# packaging workflows for the libmagicxx project.
#
# @section wf_features Features
# - List all available workflow presets from CMakePresets.json
# - Execute workflow presets by name
# - Optionally clear the build cache before running
# - Automatic execution of example binaries after examples preset
#
# @section wf_usage Usage
#     ./scripts/workflows.sh -l                              # List presets
#     ./scripts/workflows.sh -p linux-x86_64-clang-tests     # Run preset
#     ./scripts/workflows.sh -p linux-x86_64-gcc-tests -c    # Clear + run
#     ./scripts/workflows.sh -h                              # Show help
#
# @section wf_exit Exit Codes
# - 0: Success
# - 1: Missing required -p argument
# - Non-zero: CMake or example execution failure
#
# @section wf_environment Environment
# - Requires CMake 3.21+ with workflow preset support
# - Expects CMakePresets.json in project root
# - Build directory: ./build/ (relative to project root)
#
# @see CMakePresets.json for complete preset definitions
# @see AGENTS.md for workflow usage guidelines

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
