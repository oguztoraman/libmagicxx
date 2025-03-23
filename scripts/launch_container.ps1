# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

$SCRIPT_DIR = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
Set-Location -Path "$SCRIPT_DIR\.."

$UPDATE_IMAGE = $false

function usage {
    Write-Output "Usage: $($MyInvocation.MyCommand) [-u] [-h]"
    Write-Output "Launch the development container."
    Write-Output "Options:"
    Write-Output "  -u              Update the existing container image."
    Write-Output "  -h              Display this message."
    exit 1
}

param (
    [switch]$u,
    [switch]$h
)

if ($h) {
    usage
}

if ($u) {
    $UPDATE_IMAGE = $true
}

if (podman image exists libmagicxx_dev_env) {
    if ($UPDATE_IMAGE) {
        podman build --pull -t libmagicxx_dev_env -f Containerfile
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Error: Failed to update the container image."
            exit 2
        }
    }
} else {
    podman build -t libmagicxx_dev_env -f Containerfile
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Error: Failed to build the container image."
        exit 3
    }
}

podman run -it -v ${PWD}:/libmagicxx:Z libmagicxx_dev_env
if ($LASTEXITCODE -ne 0) {
    Write-Error "Error: Failed to run the container."
    exit 4
}
