#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
cd ${SCRIPT_DIR}/..

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

echo "Committing with tag '${GIT_TAG}'..."

DATE=$(date +%d-%m-%Y)
DOXYFILE=Doxyfile
CMAKE_FILE=CMakeLists.txt
CHANGELOG_FILE=CHANGELOG.md
VERSION=$(sed 's/^v//' <<< "$GIT_TAG")
RELEASE_BRANCH=$(sed 's/\(v[0-9]*\.[0-9]*\)\.[0-9]*/\1.x/' <<< "$GIT_TAG")

./scripts/format.sh || {
    exit 2
}

./scripts/generate_documentation.sh -t ${GIT_TAG} || {
    exit 3
}

sed -i "s/    VERSION.*\..*\..*/    VERSION ${VERSION}/" "$CMAKE_FILE" &&
sed -i "s/## Next Release.*/## Next Release\n\n## [${GIT_TAG}] - ${DATE}/" "$CHANGELOG_FILE" || {
    exit 4
}

git add $CMAKE_FILE $CHANGELOG_FILE $DOXYFILE doc/* &&
git commit -m "Update the project version to ${VERSION}" &&
git tag -a $GIT_TAG -m "Version $VERSION" || {
    exit 5
}

git branch $RELEASE_BRANCH

echo "Committed '${GIT_TAG}'."
