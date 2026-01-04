# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Generate uninstall script
# -----------------------------------------------------------------------------
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    IMMEDIATE
        @ONLY
)

# -----------------------------------------------------------------------------
# Uninstall target
# -----------------------------------------------------------------------------
add_custom_target(uninstall
    COMMAND
        ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
