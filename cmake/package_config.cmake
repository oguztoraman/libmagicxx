# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

include(CMakePackageConfigHelpers)

# -----------------------------------------------------------------------------
# Package configuration files
# -----------------------------------------------------------------------------
set(magicxx_CMAKE_PACKAGE_CONFIG_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/MagicxxConfig.cmake
)

set(magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/MagicxxConfigVersion.cmake
)

# -----------------------------------------------------------------------------
# Generate package configuration
# -----------------------------------------------------------------------------
configure_package_config_file(
    cmake/MagicxxConfig.cmake.in
        ${magicxx_CMAKE_PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION
        ${magicxx_CMAKE_INSTALL_LIB_DIR}
    PATH_VARS
        magicxx_INSTALL_INCLUDE_DIR
        magicxx_INSTALL_LIB_DIR
        magicxx_INSTALL_LICENSE_DIR
        magicxx_INSTALL_DOC_DIR
        magicxx_CMAKE_INSTALL_LIB_DIR
        magicxx_INSTALL_DEFAULT_DATABASES_DIR
)

# -----------------------------------------------------------------------------
# Generate version file
# -----------------------------------------------------------------------------
write_basic_package_version_file(
    ${magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE}
    VERSION
        ${magicxx_VERSION}
    COMPATIBILITY
        SameMajorVersion
)
