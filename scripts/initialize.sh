#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only
#
# @file initialize.sh
# @brief Initialize the libmagicxx development environment.
#
# This script performs first-time setup of the development environment after
# cloning the repository. It initializes Git submodules (external dependencies)
# and downloads Git LFS-tracked binary files (magic database).
#
# @section init_features Features
# - Recursive Git submodule initialization and update
# - Git LFS file retrieval for binary assets
# - Error handling with informative messages
#
# @section init_usage Usage
#     ./scripts/initialize.sh
#
# @section init_submodules Submodules Initialized
# - external/file: libmagic source (for building magic database)
# - external/googletest: Google Test framework
# - external/libgnurx: GNU regex library (Windows support)
#
# @section init_lfs LFS Files Retrieved
# - databases/magic.mgc: Pre-compiled magic database
#
# @section init_exit Exit Codes
# - 0: Initialization completed successfully
# - 1: Git submodule initialization/update failed
# - 2: Git LFS pull failed
#
# @section init_prereq Prerequisites
# - Git with LFS extension installed
# - Network access for submodule and LFS downloads
# - Run from repository root or scripts directory
#
# @section init_when When to Run
# - After initial git clone
# - After container rebuild (via launch_container.py)
# - After switching to a branch with submodule updates
#
# @note This script is automatically run inside the development container
# @see CONTRIBUTING.md for development environment setup
# @see launch_container.py for container-based development

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
