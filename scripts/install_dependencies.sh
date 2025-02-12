#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

set -e

echo "Installing the dependencies..."
sudo dnf install -y cmake make ninja-build g++ clang clang-tools-extra libcxx-devel git autoconf libtool doxygen
echo "Done"
