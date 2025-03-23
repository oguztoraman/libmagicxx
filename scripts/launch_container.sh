#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

UPDATE_IMAGE=false

usage(){
    echo "Usage: $0 [-u] [-h]"
    echo "Launch the development container."
    echo "Options:"
    echo "  -u              Update the existing container image."
    echo "  -h              Display this message."
    exit 1
}

while getopts 'uh' OPTION; do
    case ${OPTION} in
        u) UPDATE_IMAGE=true;;
        *) usage;;
    esac
done

if podman image exists libmagicxx_dev_env; then
    if [ "$UPDATE_IMAGE" = true ]; then
        podman build --pull -t libmagicxx_dev_env -f Containerfile || {
            echo "Error: Failed to update the container image."
            exit 2
        }
    fi
else
    podman build -t libmagicxx_dev_env -f Containerfile || {
        echo "Error: Failed to build the container image."
        exit 3
    }
fi

podman run -it -v $(pwd):/libmagicxx:Z libmagicxx_dev_env || {
    echo "Error: Failed to run the container."
    exit 4
}
