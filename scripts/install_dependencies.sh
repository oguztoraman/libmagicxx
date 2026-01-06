#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only
#
# @file install_dependencies.sh
# @brief Install build dependencies on Fedora-based systems.
#
# This script installs all required development tools and libraries needed
# to build, test, and package the libmagicxx project. It uses DNF package
# manager and requires sudo privileges.
#
# @section id_features Features
# - Automated installation of complete toolchain
# - Support for both GCC and Clang compilers
# - Cross-compilation toolchain for Windows (MinGW)
# - Documentation generation tools (Doxygen)
#
# @section id_usage Usage
#     ./scripts/install_dependencies.sh
#
# @section id_packages Packages Installed
#
# @subsection id_vcs Version Control
# - git, git-lfs
#
# @subsection id_build Build Tools
# - cmake, ninja-build, make
# - autoconf, libtool (for libmagic)
# - awk, python
#
# @subsection id_cpp C++ Compilers and Debuggers
# - g++, gdb (GNU toolchain)
# - clang, libcxx-devel, lld, lldb (LLVM toolchain)
# - clang-tools-extra (clang-format, clang-tidy)
#
# @subsection id_cross Cross-Compilation (Windows)
# - mingw64-gcc-c++ (MinGW-w64 cross-compiler)
# - mingw32-nsis (NSIS installer creator)
#
# @subsection id_pkg Packaging
# - rpm-build (for RPM packages)
#
# @subsection id_docs Documentation
# - doxygen
#
# @section id_exit Exit Codes
# - 0: All packages installed successfully
# - 1: DNF installation failed
#
# @section id_prereq Prerequisites
# - Fedora-based Linux distribution (Fedora, RHEL, CentOS)
# - sudo privileges
# - Network access to package repositories
#
# @warning This script is designed for Fedora. For other distributions,
#          package names and package manager may differ.
#
# @see Containerfile for container-based alternative
# @see CONTRIBUTING.md for development environment setup

echo "Installing the dependencies..."
sudo dnf install -y              \
     git git-lfs python          \
     awk make autoconf libtool   \
     cmake ninja-build           \
     rpm-build mingw32-nsis      \
     g++ gdb mingw64-gcc-c++     \
     clang libcxx-devel lld lldb \
     clang-tools-extra doxygen || {
    exit 1
}
echo "Done"
