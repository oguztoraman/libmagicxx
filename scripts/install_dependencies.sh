#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

echo "Installing the dependencies..."
sudo dnf install -y            \
     git python                \
     awk make autoconf libtool \
     cmake ninja-build         \
     g++ mold                  \
     clang libcxx-devel        \
     mingw64-gcc-c++           \
     clang-tools-extra doxygen || {
    exit 1
}
echo "Done"
