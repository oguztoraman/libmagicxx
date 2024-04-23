#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

which dnf &> /dev/null || {
    echo "Error: Unsupported system. DNF does not exist on this system."
    echo "You need to install the requirements manually.";
    exit 1;
}

echo "Installing the requirements..."
sudo dnf install cmake make g++ clang git autoconf libtool
echo

echo "Initializing and updating the git submodule file..."
git submodule update --init --recursive

echo "Configuring the file..."
cd file && autoreconf -f -i && ./configure
echo

echo "Building the file command and the Magic Number Recognition Library..."
make

echo "Done"
