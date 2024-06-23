#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

DOXYFILE=Doxyfile
GIT_TAG=""

usage(){
    echo "Usage: $0 -t git_tag [-h]"
    echo "  -t git_tag     Specify the git tag (i.e. v1.0.0)."
    echo "  -h             Display this message."
    exit 1
}

DISPLAY_USAGE=true

while getopts 't:h' OPTION; do
    case ${OPTION} in
        t) GIT_TAG=$OPTARG     DISPLAY_USAGE=false;;
        *) usage;;
    esac
done

if [ "$DISPLAY_USAGE" = true ] || [ $# -eq 0 ]; then
    usage
fi

echo "Generating Doxygen documentation with tag '${GIT_TAG}'..."

sed -i "s/^PROJECT_NUMBER.*/PROJECT_NUMBER = ${GIT_TAG}/" "$DOXYFILE"

rm -rf doc/* && doxygen ${DOXYFILE} || {
    exit 2
}

echo "Doxygen documentation generated with tag '${GIT_TAG}'."
