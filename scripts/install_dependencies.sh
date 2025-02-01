#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

which dnf &> /dev/null || {
    echo "Error: Unsupported system. DNF does not exist on this system."
    echo "You need to install the dependencies manually."
    exit 1
}

echo "Installing the dependencies..."
sudo dnf install -y cmake make ninja-build g++ clang clang-tools-extra libcxx-devel git autoconf libtool || {
    exit 2
}
echo "Done"
