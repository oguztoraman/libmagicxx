#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

DOXYFILE=Doxyfile
LATEST_GIT_TAG=$(git describe --tags --abbrev=0)

echo "Generating Doxygen documentation..."

sed -i "s/^PROJECT_NUMBER.*/PROJECT_NUMBER = ${LATEST_GIT_TAG}/" "$DOXYFILE"

doxygen ${DOXYFILE}

echo "Done."
