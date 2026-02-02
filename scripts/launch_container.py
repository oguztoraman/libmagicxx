#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

"""
Launch and manage the libmagicxx development container.

This script provides functionality to pull, build, and run the development
container using Podman. By default, it uses the pre-built image from GitHub
Container Registry for faster startup. Optionally, you can build locally
from the Containerfile.

Features
--------
- Pull pre-built image from GHCR (default, fastest)
- Build locally from Containerfile with --local flag
- Update/rebuild images with --update flag
- Automatic cleanup of stopped containers before rebuild
- Project directory mounted with SELinux context (:Z)
- Verbose logging mode for troubleshooting

Usage
-----
From the host system (not inside a container)::

    python ./scripts/launch_container.py                # Pull from GHCR, launch
    python ./scripts/launch_container.py --update       # Re-pull latest, launch
    python ./scripts/launch_container.py --tag v10.0.x  # Use versioned container
    python ./scripts/launch_container.py --local        # Build locally, launch
    python ./scripts/launch_container.py -l -u          # Rebuild locally, launch
    python ./scripts/launch_container.py -v             # Verbose output

Container Lifecycle
-------------------
1. Check if image exists locally
2. If missing, pull from GHCR (or build if --local)
3. If --update, stop containers and re-pull/rebuild image
4. Run container interactively with project mounted
5. Container removed automatically on exit (--rm)

Mount Configuration
-------------------
- Host: Project root directory (auto-detected)
- Container: /libmagicxx
- SELinux: Relabeling enabled (:Z suffix)

Exit Codes
----------
- 0: Container exited normally
- 1-125: Command execution failures (see ContainerError.return_code)
- 130: Operation cancelled by user (Ctrl+C)

Prerequisites
-------------
- Podman installed and configured (rootless mode supported)
- Network access to ghcr.io (or Containerfile for --local)
- Sufficient disk space for container image (~2GB)

Post-Launch Setup
-----------------
After launching the container, run::

    ./scripts/initialize.sh

This initializes git submodules and pulls LFS files.

Warning
-------
Do not run git push/pull commands inside the container.
Use the host terminal for git operations requiring network access.

See Also
--------
- Containerfile: Container image definition
- CONTRIBUTING.md: Development environment setup guide
- AGENTS.md: Agent workflow guidelines
"""

from __future__ import annotations

import argparse
import logging
import os
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import NoReturn

# Container configuration constants
GHCR_IMAGE_BASE = "ghcr.io/oguztoraman/libmagicxx-dev"
DEFAULT_TAG = "latest"
LOCAL_IMAGE_NAME = "libmagicxx-dev-local"
CONTAINER_FILE = "Containerfile"
MOUNT_TARGET = "/libmagicxx"

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format="%(levelname)s: %(message)s",
)
logger = logging.getLogger(__name__)


@dataclass(frozen=True, slots=True)
class ContainerConfig:
    """Configuration for container operations."""

    image_name: str
    container_file: str
    mount_target: str
    project_root: Path
    use_local: bool = False


class ContainerError(Exception):
    """Base exception for container-related errors."""

    def __init__(self, message: str, return_code: int = 1) -> None:
        super().__init__(message)
        self.return_code = return_code


def run_command(
    args: list[str],
    *,
    check: bool = True,
    capture_output: bool = False,
) -> subprocess.CompletedProcess[str]:
    """
    Execute a shell command with consistent error handling.

    Args:
        args: Command and arguments to execute.
        check: Whether to raise an exception on non-zero exit code.
        capture_output: Whether to capture stdout and stderr.

    Returns:
        CompletedProcess instance with command results.

    Raises:
        ContainerError: If the command fails and check is True.
    """
    try:
        return subprocess.run(
            args,
            check=check,
            capture_output=capture_output,
            text=True,
        )
    except subprocess.CalledProcessError as e:
        command_str = " ".join(args)
        raise ContainerError(
            f"Command '{command_str}' failed with return code {e.returncode}",
            return_code=e.returncode,
        ) from e


def image_exists(image_name: str) -> bool:
    """
    Check if a container image exists locally.

    Args:
        image_name: Name of the image to check.

    Returns:
        True if the image exists, False otherwise.
    """
    result = run_command(
        ["podman", "image", "exists", image_name],
        check=False,
    )
    return result.returncode == 0


def get_container_ids(image_name: str) -> list[str]:
    """
    Get IDs of all containers derived from a specific image.

    Args:
        image_name: Name of the ancestor image.

    Returns:
        List of container IDs.
    """
    result = run_command(
        ["podman", "ps", "-a", "-q", "--filter", f"ancestor={image_name}"],
        capture_output=True,
    )
    return result.stdout.strip().splitlines()


def cleanup_containers(container_ids: list[str]) -> None:
    """
    Stop and remove specified containers.

    Args:
        container_ids: List of container IDs to clean up.
    """
    if not container_ids:
        return

    logger.info("Stopping %d container(s)...", len(container_ids))
    run_command(["podman", "stop", *container_ids], check=False)

    logger.info("Removing container(s)...")
    run_command(["podman", "rm", *container_ids], check=False)


def remove_image(image_name: str) -> None:
    """
    Remove a container image.

    Args:
        image_name: Name of the image to remove.
    """
    logger.info("Removing image '%s'...", image_name)
    run_command(["podman", "rmi", image_name], check=False)


def pull_image(image_name: str) -> None:
    """
    Pull a container image from a registry.

    Args:
        image_name: Name of the image to pull.
    """
    logger.info("Pulling image '%s'...", image_name)
    run_command(["podman", "pull", image_name])


def build_image(config: ContainerConfig) -> None:
    """
    Build the container image from the Containerfile.

    Args:
        config: Container configuration.
    """
    logger.info("Building image '%s'...", config.image_name)
    run_command([
        "podman", "build",
        "--pull=always",
        "-t", config.image_name,
        "-f", config.container_file,
    ])


def run_container(config: ContainerConfig) -> None:
    """
    Run the development container interactively.

    Args:
        config: Container configuration.
    """
    mount_spec = f"{config.project_root}:{config.mount_target}:Z"
    logger.info("Starting container '%s'...", config.image_name)
    run_command([
        "podman", "run",
        "-it", "--rm",
        "-v", mount_spec,
        config.image_name,
    ])


def update_image(config: ContainerConfig) -> None:
    """
    Update an existing container image.

    For local builds, this stops and removes any running containers,
    removes the old image, and builds a fresh one.
    For GHCR images, this pulls the latest version.

    Args:
        config: Container configuration.
    """
    logger.info("Updating image '%s'...", config.image_name)

    container_ids = get_container_ids(config.image_name)
    cleanup_containers(container_ids)
    remove_image(config.image_name)

    if config.use_local:
        build_image(config)
    else:
        pull_image(config.image_name)


def ensure_image_exists(config: ContainerConfig, *, force_update: bool) -> None:
    """
    Ensure the container image exists, pulling/building or updating as needed.

    Args:
        config: Container configuration.
        force_update: If True, re-pull/rebuild the image even if it exists.
    """
    exists = image_exists(config.image_name)

    if exists and force_update:
        update_image(config)
    elif not exists:
        if config.use_local:
            build_image(config)
        else:
            pull_image(config.image_name)
    else:
        logger.info("Image '%s' already exists.", config.image_name)


def get_project_root() -> Path:
    """
    Determine the project root directory.

    Returns:
        Path to the project root (parent of the scripts directory).
    """
    script_path = Path(__file__).resolve()
    return script_path.parent.parent


def parse_arguments() -> argparse.Namespace:
    """
    Parse command-line arguments.

    Returns:
        Parsed arguments namespace.
    """
    parser = argparse.ArgumentParser(
        description="Launch the libmagicxx development container.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                Pull from GHCR and launch (fastest)
  %(prog)s --update       Re-pull latest image before launching
  %(prog)s --tag v10.0.x  Use versioned container for release branch
  %(prog)s --local        Build from local Containerfile
  %(prog)s -l -u          Rebuild local image before launching
  %(prog)s -v             Enable verbose output
        """,
    )
    parser.add_argument(
        "-t", "--tag",
        default=DEFAULT_TAG,
        help="container image tag to use (default: latest, e.g., v10.0.x for release branches)",
    )
    parser.add_argument(
        "-l", "--local",
        action="store_true",
        help="build from local Containerfile instead of pulling from GHCR",
    )
    parser.add_argument(
        "-u", "--update",
        action="store_true",
        help="re-pull or rebuild the container image before launching",
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="enable verbose output",
    )
    return parser.parse_args()


def main() -> NoReturn:
    """
    Main entry point for the container launch script.

    Parses arguments, ensures the container image exists, and runs
    the development container.
    """
    args = parse_arguments()

    if args.verbose:
        logger.setLevel(logging.DEBUG)

    project_root = get_project_root()

    # Use local image name when building locally, GHCR image with tag otherwise
    image_name = LOCAL_IMAGE_NAME if args.local else f"{GHCR_IMAGE_BASE}:{args.tag}"

    config = ContainerConfig(
        image_name=image_name,
        container_file=CONTAINER_FILE,
        mount_target=MOUNT_TARGET,
        project_root=project_root,
        use_local=args.local,
    )

    # Change to project root for Containerfile context
    os.chdir(project_root)

    try:
        ensure_image_exists(config, force_update=args.update)
        run_container(config)
        sys.exit(0)
    except ContainerError as e:
        logger.error("%s", e)
        sys.exit(e.return_code)
    except KeyboardInterrupt:
        logger.info("Operation cancelled by user.")
        sys.exit(130)


if __name__ == "__main__":
    main()
