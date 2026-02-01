# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only
#
# Containerfile
# =============
# Development container image for libmagicxx.
#
# This Containerfile defines a Fedora-based development environment with all
# required build tools, compilers, and dependencies pre-installed. It provides
# a consistent, reproducible build environment across different host systems.
#
# Features
# --------
# - Fedora 43 base image (stable, up-to-date packages)
# - GCC and Clang C++ toolchains
# - CMake + Ninja build system
# - Cross-compilation support for Windows (MinGW-w64)
# - Documentation generation (Doxygen)
# - Package creation tools (RPM, DEB, NSIS)
#
# Usage
# -----
# The pre-built image is available on GitHub Container Registry:
#     podman pull ghcr.io/oguztoraman/libmagicxx-dev:latest
#
# Or build the image locally:
#     podman build -t libmagicxx_dev_env -f Containerfile .
#
# Run interactively with project mounted:
#     podman run -it --rm -v $(pwd):/libmagicxx:Z ghcr.io/oguztoraman/libmagicxx-dev:latest
#
# Or use the launch script (recommended):
#     python ./scripts/launch_container.py           # Pull from GHCR (fastest)
#     python ./scripts/launch_container.py --local   # Build locally
#
# Post-Launch Setup
# -----------------
# After entering the container, initialize the development environment:
#     ./scripts/initialize.sh
#
# Then build and test using workflow presets:
#     ./scripts/workflows.sh -p linux-x86_64-clang-tests
#
# Image Layers
# ------------
# 1. Base: Fedora 43 minimal container image
# 2. Scripts: Copy install_dependencies.sh into container
# 3. Dependencies: Run installation script (adds ~1.5GB)
# 4. Workdir: Set /libmagicxx as default working directory
#
# Mount Points
# ------------
# /libmagicxx - Project source code (bind-mounted from host)
#
# Security Notes
# --------------
# - Uses :Z suffix for SELinux relabeling on bind mounts
# - Runs as root inside container (standard for development containers)
# - No network operations during build except package downloads
#
# See Also
# --------
# - scripts/launch_container.py: Automated container management
# - scripts/install_dependencies.sh: Package installation details
# - CONTRIBUTING.md: Development environment setup guide
#

# -----------------------------------------------------------------------------
# Base Image
# -----------------------------------------------------------------------------
# Fedora 43 provides recent versions of GCC, Clang, CMake, and other tools.
# Using a specific version ensures reproducible builds.
FROM fedora:43

# -----------------------------------------------------------------------------
# Install Build Dependencies
# -----------------------------------------------------------------------------
# Copy the dependency installation script and execute it.
# This installs: git, cmake, ninja, gcc, clang, mingw, doxygen, rpm-build, etc.
COPY ./scripts/install_dependencies.sh /scripts/

RUN /scripts/install_dependencies.sh

# -----------------------------------------------------------------------------
# Configure Working Directory
# -----------------------------------------------------------------------------
# Set the default working directory where the project will be mounted.
# The host project directory is bind-mounted here at runtime.
WORKDIR /libmagicxx
