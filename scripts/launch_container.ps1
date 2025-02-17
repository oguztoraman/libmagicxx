# SPDX-FileCopyrightText: Copyright (c) 2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

$SCRIPT_DIR = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
Set-Location -Path $SCRIPT_DIR\..

podman build -t libmagicxx_dev_env -f Containerfile
if ($LASTEXITCODE -ne 0) {
    Write-Error "Error: Failed to build the container image."
    exit 1
}

podman run -it --name libmagicxx_dev_env -v ${PWD}:/libmagicxx:Z libmagicxx_dev_env
if ($LASTEXITCODE -ne 0) {
    Write-Error "Error: Failed to run the container."
    exit 2
}
