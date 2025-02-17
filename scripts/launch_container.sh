#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

podman build -t libmagicxx_dev_env -f Containerfile || {
    echo "Error: Failed to build the container image."
    exit 1
}

podman run -it -v $(pwd):/libmagicxx:Z libmagicxx_dev_env || {
    echo "Error: Failed to run the container."
    exit 2
}
