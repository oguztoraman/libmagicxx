#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

echo "Installing the dependencies..."
sudo dnf install -y awk cmake make ninja-build mold g++ clang clang-tools-extra libcxx-devel git autoconf libtool doxygen || {
    exit 1
}
echo "Done"
