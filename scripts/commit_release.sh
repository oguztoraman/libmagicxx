#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

SCRIPT_DIR="$(dirname -- "$(realpath "${BASH_SOURCE[0]}")")"
cd -- "${SCRIPT_DIR}/.."

usage(){
    echo "Usage: $0 -t git_tag [-y] [-h]"
    echo "  -t git_tag     Specify the git tag (i.e. v1.0.0)."
    echo "  -y             Skip interactive confirmation (use with care)."
    echo "  -h             Display this message."
    exit 1
}

GIT_TAG=""
DISPLAY_USAGE=true
SKIP_CONFIRM=false

while getopts 't:yh' OPTION; do
    case "${OPTION}" in
        t)
            GIT_TAG="${OPTARG}"
            DISPLAY_USAGE=false
            ;;
        y)
            SKIP_CONFIRM=true
            ;;
        *) usage ;;
    esac
done

if [ "$DISPLAY_USAGE" = true ] || [ $# -eq 0 ]; then
    usage
fi

echo "Committing with tag '${GIT_TAG}'..."

DATE="$(date +%d-%m-%Y)"
CMAKE_FILE="CMakeLists.txt"
CHANGELOG_FILE="CHANGELOG.md"
VERSION=$(sed 's/^v//' <<< "${GIT_TAG}")
RELEASE_BRANCH=$(sed 's/\(v[0-9]*\.[0-9]*\)\.[0-9]*/\1.x/' <<< "${GIT_TAG}")

if [[ ! "${GIT_TAG}" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: Tag '${GIT_TAG}' is not in the form vMAJOR.MINOR.PATCH."
    exit 2
fi

if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "Error: Working tree has uncommitted changes. Commit or stash first."
    exit 3
fi

if git rev-parse --verify --quiet "refs/tags/${GIT_TAG}" >/dev/null; then
    echo "Error: Tag '${GIT_TAG}' already exists. Choose a new version."
    exit 4
fi

if [[ "${SKIP_CONFIRM}" == false ]]; then
    echo "This will create a commit and tag '${GIT_TAG}' on branch '${RELEASE_BRANCH}'."
    read -r -p "Continue? [y/N] " reply
    case "${reply}" in
        y|Y|yes|YES) ;;
        *) echo "Aborted."; exit 5 ;;
    esac
fi

if git rev-parse --verify --quiet "${RELEASE_BRANCH}" >/dev/null; then
    git switch "${RELEASE_BRANCH}" || exit 6
else
    git switch -c "${RELEASE_BRANCH}" || exit 6
fi

./scripts/workflows.sh -p format-source-code || exit 7

sed -i "s/    VERSION.*\..*\..*/    VERSION ${VERSION}/" "${CMAKE_FILE}" || exit 8

sed -i "s/## Next Release.*/## Next Release\n\n## [${GIT_TAG}] - ${DATE}/" "${CHANGELOG_FILE}" || exit 9

./scripts/workflows.sh -p documentation || exit 10

git add -- "${CMAKE_FILE}" "${CHANGELOG_FILE}" documentation/html || exit 11

git commit -m "Update the project version to ${VERSION}" || exit 12

git tag -a "${GIT_TAG}" -m "Version ${VERSION}" || exit 13

echo "Committed '${GIT_TAG}'."
