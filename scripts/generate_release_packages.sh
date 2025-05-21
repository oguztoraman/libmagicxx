#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

echo "Generating release packages..."

rm -rf packages/

./scripts/workflows.sh -p generate-source-package -c || {
    exit 1
}

./scripts/workflows.sh -p linux-x86_64-gcc-packages -c || {
    exit 2
}

./scripts/workflows.sh -p windows-x86_64-mingw64-packages -c || {
    exit 3
}

echo "Release packages generated."