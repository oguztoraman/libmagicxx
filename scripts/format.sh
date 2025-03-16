#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

echo "Formatting source files..."
clang-format -i -style=file inc/*.hpp src/*.cpp tests/*.cpp examples/*.cpp || {
    exit 1
}
echo "Done."
