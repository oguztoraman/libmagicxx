# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

FROM fedora:42

COPY ./scripts/install_dependencies.sh /scripts/

RUN /scripts/install_dependencies.sh

WORKDIR /libmagicxx
