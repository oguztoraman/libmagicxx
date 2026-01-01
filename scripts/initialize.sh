#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR="$(dirname -- "$(realpath "${BASH_SOURCE[0]}")")"
cd -- "${SCRIPT_DIR}/.."

echo "Initializing and updating git submodules..."
if ! git submodule update --init --recursive; then
    echo "Failed to initialize and update git submodules." >&2
    exit 1
fi
echo "Pulling database files..."
if ! git lfs pull; then
    echo "Failed to pull database files." >&2
    exit 2
fi
echo "Done."
