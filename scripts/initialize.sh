#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

echo "Initializing and updating git submodules..."
git submodule update --init --recursive || {
    echo "Failed to initialize and update git submodules."
    exit 1
}
echo "Pulling database files..."
git lfs pull || {
    echo "Failed to pull database files."
    exit 2
}
echo "Done."
