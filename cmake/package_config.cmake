# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

include(CMakePackageConfigHelpers)

set(magicxx_CMAKE_PACKAGE_CONFIG_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/magicxxConfig.cmake
)

set(magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/magicxxConfigVersion.cmake
)

configure_package_config_file(
    cmake/magicxxConfig.cmake.in
        ${magicxx_CMAKE_PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION
        ${magicxx_CMAKE_INSTALL_LIBDIR}
    PATH_VARS
        magicxx_INSTALL_INCLUDEDIR
        magicxx_INSTALL_LIBDIR
        magicxx_INSTALL_DOCDIR
        magicxx_CMAKE_INSTALL_LIBDIR
        magicxx_DEFAULT_DATABASES_INSTALL_DIR
)

write_basic_package_version_file(
    ${magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE}
    VERSION
        ${magicxx_VERSION}
    COMPATIBILITY
        SameMajorVersion
)
