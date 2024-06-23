#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

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

./generate_documentation.sh -t ${GIT_TAG} || {
    exit 2
}

sed -i "s/    VERSION.*\..*\..*/    VERSION ${VERSION}/" "$CMAKE_FILE" || {
    exit 3
}

sed -i "s/## Next Release.*/## Next Release\n\n## [${GIT_TAG}] - ${DATE}\n/" "$CHANGELOG_FILE" || {
    exit 4
}

git add $CMAKE_FILE $CHANGELOG_FILE $DOXYFILE doc/* &&
git commit -m "Update the project version to ${VERSION}" &&
git tag -a $GIT_TAG -m "Version $VERSION" || {
    exit 5
}

echo "Committed '${GIT_TAG}'."
