# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# License file installation
# -----------------------------------------------------------------------------
install(
    FILES
        ${magicxx_LICENSE_FILE}
    DESTINATION
        ${magicxx_INSTALL_LICENSE_DIR}
    COMPONENT
        License
)

install(
    FILES
        ${magic_LICENSE_FILE}
    DESTINATION
        ${magic_INSTALL_LICENSE_DIR}
    COMPONENT
        License
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(
        FILES
            ${gnurx_LICENSE_FILE}
        DESTINATION
            ${gnurx_INSTALL_LICENSE_DIR}
        COMPONENT
            License
    )
endif()

# -----------------------------------------------------------------------------
# Main installation
# -----------------------------------------------------------------------------
if(INSTALL_MAGICXX)
    install(
        FILES
            ${magicxx_DEFAULT_DATABASE_FILES}
        DESTINATION
            ${magicxx_INSTALL_DEFAULT_DATABASES_DIR}
        COMPONENT
            Data
    )

    install(
        FILES
            ${magicxx_HEADER_FILES}
        DESTINATION
            ${magicxx_INSTALL_INCLUDE_DIR}
        COMPONENT
            Development
    )

    install(
        EXPORT
            MagicxxTargets
        NAMESPACE
            Recognition::
        DESTINATION
            ${magicxx_CMAKE_INSTALL_LIB_DIR}
        COMPONENT
            Development
    )

    install(
        FILES
            ${magicxx_CMAKE_PACKAGE_CONFIG_FILE}
            ${magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE}
        DESTINATION
            ${magicxx_CMAKE_INSTALL_LIB_DIR}
        COMPONENT
            Development
    )

    if(BUILD_MAGICXX_SHARED_LIB)
        install(
            TARGETS
                magicxx
            EXPORT
                MagicxxTargets
            FILE_SET
                magicxxHeaders
            LIBRARY DESTINATION
                ${magicxx_INSTALL_LIB_DIR}
            COMPONENT
                Library
        )
    endif()

    if(BUILD_MAGICXX_STATIC_LIB)
        install(
            TARGETS
                magicxx_static
            EXPORT
                MagicxxTargets
            FILE_SET
                magicxx_staticHeaders
            ARCHIVE DESTINATION
                ${magicxx_INSTALL_LIB_DIR}
            COMPONENT
                Archive
        )
    endif()

    install(
        FILES
            ${PROJECT_SOURCE_DIR}/documentation/README.txt
        DESTINATION
            ${magicxx_INSTALL_DOC_DIR}
        COMPONENT
            Documentation
    )
endif()
