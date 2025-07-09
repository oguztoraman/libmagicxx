#!/bin/python
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

import os
import sys
import argparse
import subprocess

def main():
    parser = argparse.ArgumentParser(description="Launch the development container.")
    parser.add_argument("-u", "--update", action="store_true", help="update the existing container image.")
    args = parser.parse_args()
    script_dir = os.path.dirname(os.path.realpath(__file__))
    os.chdir(os.path.join(script_dir, ".."))
    update_image = args.update
    try:
        result = subprocess.run(["podman", "image", "exists", "libmagicxx_dev_env"], check=False)
        image_exists = (result.returncode == 0)
        if image_exists and update_image:
            container_ids = subprocess.run([
                "podman", "ps", "-a", "-q", "--filter", "ancestor=libmagicxx_dev_env"
            ], capture_output=True, text=True, check=True).stdout.strip().splitlines()
            if container_ids:
                subprocess.run(["podman", "stop"] + container_ids, check=False)
                subprocess.run(["podman", "rm"] + container_ids, check=False)
            subprocess.run(["podman", "rmi", "libmagicxx_dev_env"], check=False)
            subprocess.run(["podman", "build", "--pull=always", "-t", "libmagicxx_dev_env", "-f", "Containerfile"], check=True)
        elif not image_exists:
            subprocess.run(["podman", "build", "--pull=always", "-t", "libmagicxx_dev_env", "-f", "Containerfile"], check=True)
        subprocess.run(["podman", "run", "-it", "--rm", "-v", f"{os.getcwd()}:/libmagicxx:Z", "libmagicxx_dev_env"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: Command {e.cmd} failed with return code {e.returncode}")
        sys.exit(e.returncode)

if __name__ == "__main__":
    main()
