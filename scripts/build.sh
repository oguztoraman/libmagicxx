#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

set -e

BUILD_DIR="release_build"
BUILD_TYPE="Release"
COMPILER="g++"
RUN_TESTS="OFF"
RUN_EXAMPLES="OFF"

usage(){
    echo "Usage: $0 [-d build_dir] [-b build_type] [-c compiler] [-t] [-e] [-r] [-h]"
    echo "  -d build_dir   Specify the build directory (default: ${BUILD_DIR})."
    echo "  -b build_type  Specify the CMake build type (default: ${BUILD_TYPE})."
    echo "  -c compiler    Specify the compiler (g++ or clang++, default: ${COMPILER})."
    echo "  -t             Build and run tests (default: ${RUN_TESTS})."
    echo "  -e             Build and run examples (default: ${RUN_EXAMPLES})."
    echo "  -h             Display this message."
    exit 1
}

DISPLAY_USAGE=true

while getopts 'd:b:c:htre' OPTION; do
    case ${OPTION} in
        d) BUILD_DIR=$OPTARG  DISPLAY_USAGE=false;;
        b) BUILD_TYPE=$OPTARG DISPLAY_USAGE=false;;
        c) COMPILER=$OPTARG   DISPLAY_USAGE=false;;
        e) RUN_EXAMPLES="ON"  DISPLAY_USAGE=false;;
        t) RUN_TESTS="ON"     DISPLAY_USAGE=false;;
        *) usage;;
    esac
done

if [ "$DISPLAY_USAGE" = true ] && [ $# -gt 0 ]; then
    usage
fi

echo "Selected options: build_dir=${BUILD_DIR}, build_type=${BUILD_TYPE}, compiler=${COMPILER}, build and run tests=${RUN_TESTS}, build and run examples=${RUN_EXAMPLES}"

cmake -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -DBUILD_MAGICXX_TESTS=${RUN_TESTS} -DBUILD_MAGICXX_EXAMPLES=${RUN_EXAMPLES} -DCMAKE_CXX_COMPILER:FILEPATH=${COMPILER} -G Ninja -S . -B ${BUILD_DIR}

cd ${BUILD_DIR} && ninja -j10 &&
echo "Build completed in '${BUILD_DIR}' with build type '${BUILD_TYPE}' using '${COMPILER}'."

if [ "$RUN_TESTS" == "ON" ]; then
    echo "Running tests..."
    cd tests && ctest --output-on-failure -j10 && cd ..
fi

if [ "$RUN_EXAMPLES" == "ON" ]; then
    echo "Running examples..."
    cd examples && ./magicxx_examples && cd ..
fi
