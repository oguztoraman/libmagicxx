# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

FROM fedora:latest

RUN dnf install -y cmake git gcc g++ clang-tools-extra

WORKDIR /libmagicxx
