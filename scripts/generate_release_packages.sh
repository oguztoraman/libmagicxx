#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only
#
# @file generate_release_packages.sh
# @brief Generate distribution packages for all supported platforms.
#
# This script orchestrates the creation of release packages for source
# distribution and all supported target platforms. It wraps the workflows.sh
# script to run the appropriate CPack presets in sequence.
#
# @section grp_features Features
# - Source tarball generation (cpack-source)
# - Linux package generation (DEB, RPM)
# - Windows cross-compiled packages (NSIS installer)
# - Automatic cleanup of previous package artifacts
#
# @section grp_usage Usage
#     ./scripts/generate_release_packages.sh
#
# @section grp_output Output Location
# All generated packages are placed in the ./packages/ directory:
# - packages/libmagicxx-X.Y.Z-Source.tar.gz
# - packages/libmagicxx-X.Y.Z-Linux-x86_64.deb
# - packages/libmagicxx-X.Y.Z-Linux-x86_64.rpm
# - packages/libmagicxx-X.Y.Z-Windows-x86_64.exe
#
# @section grp_exit Exit Codes
# - 0: All packages generated successfully
# - 1: Source package generation failed
# - 2: Linux package generation failed
# - 3: Windows package generation failed
#
# @section grp_prereq Prerequisites
# - Clean build environment (caches are cleared automatically)
# - For Windows: MinGW-w64 cross-compiler and NSIS installed
# - For Linux: rpmbuild and dpkg-deb available
#
# @see workflows.sh for preset execution
# @see RELEASE_PROCEDURE.md for release workflow
# @see cmake/cpack*.cmake for package configuration

SCRIPT_DIR="$(dirname -- "$(realpath "${BASH_SOURCE[0]}")")"
cd -- "${SCRIPT_DIR}/.."

echo "Generating release packages..."

rm -rf -- packages/

./scripts/workflows.sh -p generate-source-package -c || exit 1

./scripts/workflows.sh -p linux-x86_64-clang-packages -c || exit 2

./scripts/workflows.sh -p windows-x86_64-clang-packages -c || exit 3

echo "Release packages generated."
