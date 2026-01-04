# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Helper function to configure a magicxx library target
# -----------------------------------------------------------------------------
function(configure_magicxx_target target_name alias_name library_type)
    add_library(${target_name} ${library_type})
    add_library(Recognition::${alias_name} ALIAS ${target_name})
    add_dependencies(${target_name} configure_file)

    target_compile_features(${target_name}
        PUBLIC
            cxx_std_23
        PRIVATE
            c_std_99
    )

    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
        C_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
    )

    if(library_type STREQUAL "SHARED")
        set_target_properties(${target_name} PROPERTIES
            VERSION ${magicxx_VERSION}
            SOVERSION ${magicxx_VERSION_MAJOR}
        )
        set(file_set_name ${target_name}Headers)
    else()
        set(file_set_name ${target_name}Headers)
    endif()

    target_sources(${target_name}
        PUBLIC
            FILE_SET ${file_set_name}
            TYPE HEADERS
            BASE_DIRS ${magicxx_SOURCE_DIR}/include
            FILES ${magicxx_HEADER_FILES}
        PRIVATE
            ${gnurx_SOURCE_FILES}
            ${magic_SOURCE_FILES}
            ${magicxx_SOURCE_FILES}
    )

    target_compile_definitions(${target_name}
        PRIVATE
            HAVE_CONFIG_H
            HAVE_STDINT_H
            MAGIC_DEFAULT_DATABASE_FILE="${magicxx_INSTALLED_DEFAULT_DATABASE_FILE}"
    )

    target_include_directories(${target_name}
        PRIVATE
            ${gnurx_INCLUDE_DIR}
            ${magic_INCLUDE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${magicxx_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${magicxx_INSTALL_INCLUDE_DIR}>
    )
endfunction()

# -----------------------------------------------------------------------------
# Build shared library
# -----------------------------------------------------------------------------
if(BUILD_MAGICXX_SHARED_LIB)
    configure_magicxx_target(magicxx Magicxx SHARED)
endif()

# -----------------------------------------------------------------------------
# Build static library
# -----------------------------------------------------------------------------
if(BUILD_MAGICXX_STATIC_LIB)
    configure_magicxx_target(magicxx_static MagicxxStatic STATIC)
endif()
