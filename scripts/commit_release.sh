#!/bin/bash
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
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
VERSION=${GIT_TAG//v/}
RELEASE_BRANCH=${GIT_TAG%.*}.x

if [[ ! "${GIT_TAG}" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: Tag '${GIT_TAG}' is not in the form vMAJOR.MINOR.PATCH."
    exit 2
fi

CURRENT_TAG="$(git tag --list 'v[0-9]*.[0-9]*.[0-9]*' --sort=v:refname | tail -n 1)"
if [[ -z "${CURRENT_TAG}" ]]; then
    echo "Error: No existing semantic version tags found (vMAJOR.MINOR.PATCH)."
    echo "Please create the initial tag manually first."
    exit 3
fi

CURRENT_VERSION=${CURRENT_TAG//v/}
CURRENT_MAJOR=${CURRENT_VERSION%%.*}
CURRENT_MINOR_PART=${CURRENT_VERSION#*.}
CURRENT_MINOR=${CURRENT_MINOR_PART%%.*}
CURRENT_PATCH=${CURRENT_VERSION##*.}

EXPECTED_PATCH_TAG="v${CURRENT_MAJOR}.${CURRENT_MINOR}.$((CURRENT_PATCH + 1))"
EXPECTED_MINOR_TAG="v${CURRENT_MAJOR}.$((CURRENT_MINOR + 1)).0"
EXPECTED_MAJOR_TAG="v$((CURRENT_MAJOR + 1)).0.0"

if [[ "${GIT_TAG}" != "${EXPECTED_PATCH_TAG}" && \
      "${GIT_TAG}" != "${EXPECTED_MINOR_TAG}" && \
      "${GIT_TAG}" != "${EXPECTED_MAJOR_TAG}" ]]; then
    echo "Error: Tag '${GIT_TAG}' is not the next patch, minor, or major version."
    echo "Latest existing tag: ${CURRENT_TAG}"
    echo "Expected one of: ${EXPECTED_PATCH_TAG}, ${EXPECTED_MINOR_TAG}, ${EXPECTED_MAJOR_TAG}"
    exit 4
fi

if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "Error: Working tree has uncommitted changes. Commit or stash first."
    exit 5
fi

if git rev-parse --verify --quiet "refs/tags/${GIT_TAG}" >/dev/null; then
    echo "Error: Tag '${GIT_TAG}' already exists. Choose a new version."
    exit 6
fi

if [[ "${SKIP_CONFIRM}" == false ]]; then
    echo "This will create a commit and tag '${GIT_TAG}' on branch '${RELEASE_BRANCH}'."
    read -r -p "Continue? [y/N] " reply
    case "${reply}" in
        y|Y|yes|YES) ;;
        *) echo "Aborted."; exit 7 ;;
    esac
fi

MAJOR=${VERSION%%.*}
MINOR_PART=${VERSION#*.}
MINOR=${MINOR_PART%%.*}
SECURITY_FILE="SECURITY.md"
TMP_SECURITY_FILE="${SECURITY_FILE}.tmp"

awk -v major="${MAJOR}" -v minor="${MINOR}" '
BEGIN {
    in_block = 0;
}
{
    if ($0 ~ /^## Supported Versions[[:space:]]*$/) {
        in_block = 1;
        print $0;
        next;
    }
    if (in_block == 1) {
        if ($0 ~ /^##[[:space:]]+/) {
            print "";
            print "| Version | Supported |";
            print "| ------- | ----------|";
            if (minor == 0) {
                printf("| %d.0.x  |     +     |\n", major);
                printf("|  < %d  |     -     |\n", major);
            } else {
                printf("| %d.%d.x  |     +     |\n", major, minor);
                printf("| %d.0.x  |     -     |\n", major);
                printf("|  < %d   |     -     |\n", major);
            }
            print "";
            print $0;
            in_block = 2;
            next;
        } else {
            next;
        }
    }
    print $0;
}
' "${SECURITY_FILE}" > "${TMP_SECURITY_FILE}" && mv -- "${TMP_SECURITY_FILE}" "${SECURITY_FILE}" || {
    echo "Error: Failed to update ${SECURITY_FILE}.";
    exit 8;
}

if git rev-parse --verify --quiet "${RELEASE_BRANCH}" >/dev/null; then
    git switch "${RELEASE_BRANCH}" || exit 9
else
    git switch -c "${RELEASE_BRANCH}" || exit 9
fi

./scripts/workflows.sh -p format-source-code || exit 10

sed -i "s/    VERSION.*\..*\..*/    VERSION ${VERSION}/" "${CMAKE_FILE}" || exit 11

sed -i "s/## Next Release.*/## Next Release\n\n## [${GIT_TAG}] - ${DATE}/" "${CHANGELOG_FILE}" || exit 12

git add -- "${CMAKE_FILE}" "${CHANGELOG_FILE}" "${SECURITY_FILE}" || exit 13

git commit -m "Update the project version to ${VERSION}" || exit 14

git tag -a "${GIT_TAG}" -m "Version ${VERSION}" || exit 15

echo "Committed '${GIT_TAG}'."
