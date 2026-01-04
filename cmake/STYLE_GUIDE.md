# CMake Style Guide for Libmagicxx

## Overview

This document defines the coding conventions and best practices for CMake files in the Libmagicxx project.

## Table of Contents

+ [General Principles](#general-principles)

+ [File Organization](#file-organization)

+ [Naming Conventions](#naming-conventions)

+ [Formatting](#formatting)

+ [Commands and Functions](#commands-and-functions)

+ [Target Configuration](#target-configuration)

+ [Variable Usage](#variable-usage)

+ [Options](#options)

+ [Comments](#comments)

+ [Best Practices](#best-practices)

## General Principles

1. **Clarity over cleverness**: Write CMake code that is easy to understand and maintain.

2. **Modern CMake**: Use target-based approach (`target_*` commands) rather than directory-based commands.

3. **Consistency**: Follow established patterns in existing CMake files.

4. **Modularity**: Keep CMake modules focused on a single responsibility.

## File Organization

### Directory Structure

```
cmake/
├── STYLE_GUIDE.md                        # This file
├── MagicxxConfig.cmake.in                # Package config template
├── options.cmake                         # Build options
├── directories.cmake                     # Path definitions
├── sources.cmake                         # Source file lists
├── files.cmake                           # Additional file definitions
├── dependencies.cmake                    # External dependencies
├── targets.cmake                         # Library target definitions
├── package_config.cmake                  # CMake package configuration
├── install.cmake                         # Installation rules
├── uninstall.cmake                       # Uninstall target
├── cpack.cmake                           # Packaging configuration
├── format_source_code.cmake              # Code formatting target
├── generate_default_database_files.cmake # Database generation
└── toolchains/                           # Toolchain files
    ├── linux-x86_64-clang.cmake
    ├── linux-x86_64-gcc.cmake
    └── ...
```

### Include Order

In `CMakeLists.txt`, include modules in dependency order:

```cmake
include(cmake/options.cmake)              # Build options first
include(cmake/directories.cmake)          # Path setup
include(cmake/sources.cmake)              # File lists (depends on directories)
include(cmake/files.cmake)                # Additional files
include(cmake/dependencies.cmake)         # External deps
include(cmake/targets.cmake)              # Library targets (depends on above)
include(cmake/package_config.cmake)       # Package config
include(cmake/install.cmake)              # Installation
include(cmake/uninstall.cmake)            # Uninstall target
include(cmake/cpack.cmake)                # Packaging
include(cmake/format_source_code.cmake)   # Dev tools
include(cmake/generate_default_database_files.cmake)
```

## Naming Conventions

### Variables

| Category | Convention | Example |
|----------|------------|---------|
| Project internal | `project_SUFFIX` | `magicxx_SOURCE_DIR`, `magicxx_VERSION` |
| External dependency | `dependency_SUFFIX` | `magic_INCLUDE_DIR`, `gnurx_SOURCE_FILES` |
| Cache options | `SCREAMING_SNAKE_CASE` | `BUILD_MAGICXX_TESTS`, `INSTALL_MAGICXX` |
| Exported/public | `SCREAMING_SNAKE_CASE` | `MAGICXX_FOUND`, `MAGICXX_VERSION` |
| Local (in functions) | `snake_case` | `source_files`, `target_name` |

### Functions and Macros

Use `snake_case` for custom functions and macros:

```cmake
function(configure_magicxx_target target_name alias_name library_type)
    # ...
endfunction()

macro(add_magicxx_test test_name)
    # ...
endmacro()
```

### Targets

| Target Type | Internal Name | Exported Alias |
|-------------|---------------|----------------|
| Shared library | `magicxx` | `Recognition::Magicxx` |
| Static library | `magicxx_static` | `Recognition::MagicxxStatic` |
| Test executable | `magicxx_tests` | N/A |
| Example executable | `magicxx_examples` | N/A |

### Files

| File Type | Convention | Example |
|-----------|------------|---------|
| Module files | `snake_case.cmake` | `package_config.cmake` |
| Config templates | `PascalCaseConfig.cmake.in` | `MagicxxConfig.cmake.in` |
| Toolchain files | `platform-arch-compiler.cmake` | `linux-x86_64-clang.cmake` |

## Formatting

### Indentation

Use **4 spaces** for indentation (no tabs):

```cmake
if(BUILD_MAGICXX_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

### Line Length

Prefer lines under 100 characters. Break long commands across multiple lines:

```cmake
target_compile_definitions(${target_name}
    PRIVATE
        HAVE_CONFIG_H
        HAVE_STDINT_H
        MAGIC_DEFAULT_DATABASE_FILE="${magicxx_INSTALLED_DEFAULT_DATABASE_FILE}"
)
```

### Argument Alignment

Align arguments vertically when breaking across lines:

```cmake
set(magicxx_HEADER_FILES
    ${magicxx_INCLUDE_DIR}/magic.hpp
    ${magicxx_INCLUDE_DIR}/magic_exception.hpp
    ${magicxx_INCLUDE_DIR}/percentage.hpp
    ${magicxx_INCLUDE_DIR}/progress_tracker.hpp
    ${magicxx_INCLUDE_DIR}/utility.hpp
)
```

### Parentheses

No space before opening parenthesis, no space after:

```cmake
# Good
if(BUILD_MAGICXX_TESTS)
set(variable value)
function(my_function arg1 arg2)

# Bad
if ( BUILD_MAGICXX_TESTS )
set (variable value)
function( my_function arg1 arg2 )
```

### Control Flow Statements

Use empty parentheses for `endif()`, `endfunction()`, `endforeach()`, etc.:

```cmake
# Good
if(BUILD_MAGICXX_TESTS)
    enable_testing()
endif()

function(configure_target name)
    # ...
endfunction()

# Bad - Legacy style with repeated condition
if(BUILD_MAGICXX_TESTS)
    enable_testing()
endif(BUILD_MAGICXX_TESTS)
```

## Commands and Functions

### Command Case

Use **lowercase** for all CMake commands:

```cmake
# Good
cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)
project(magicxx VERSION 9.1.1)
add_library(magicxx SHARED)
target_link_libraries(target PRIVATE dependency)

# Bad
CMAKE_MINIMUM_REQUIRED(VERSION 3.31.6)
PROJECT(magicxx)
ADD_LIBRARY(magicxx SHARED)
```

### Keyword Case

Use **UPPERCASE** for CMake keywords:

```cmake
# Good
target_sources(${target_name}
    PUBLIC
        FILE_SET ${file_set_name}
        TYPE HEADERS
    PRIVATE
        ${source_files}
)

# Bad
target_sources(${target_name}
    public
        file_set ${file_set_name}
)
```

## Target Configuration

### Target Properties

Use `target_*` commands instead of global commands:

```cmake
# Good - Target-based (Modern CMake)
target_compile_features(magicxx PUBLIC cxx_std_23)
target_include_directories(magicxx PUBLIC ${include_dir})
target_compile_definitions(magicxx PRIVATE HAVE_CONFIG_H)

# Bad - Directory-based (Legacy CMake)
set(CMAKE_CXX_STANDARD 23)
include_directories(${include_dir})
add_definitions(-DHAVE_CONFIG_H)
```

### Generator Expressions

Use generator expressions for build/install interface differences:

```cmake
target_include_directories(magicxx
    PUBLIC
        $<BUILD_INTERFACE:${magicxx_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${magicxx_INSTALL_INCLUDE_DIR}>
)
```

## Variable Usage

### Setting Variables

Use descriptive names and group related variables:

```cmake
# Directory variables
set(magicxx_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/include/magicxx
)

set(magicxx_SOURCES_DIR
    ${magicxx_SOURCE_DIR}/sources
)
```

### Quoting

Quote variable references that may contain spaces:

```cmake
# Good
set(path "${CMAKE_CURRENT_SOURCE_DIR}/path with spaces")
message(STATUS "Path: ${path}")

# Required for paths, optional for simple values
target_compile_definitions(target PRIVATE
    DATABASE_PATH="${database_path}"
)
```

### Lists

Define lists with one item per line for readability:

```cmake
set(source_files
    ${src_dir}/file1.cpp
    ${src_dir}/file2.cpp
    ${src_dir}/file3.cpp
)
```

## Options

### Defining Options

Use `option()` with clear descriptions:

```cmake
option(BUILD_MAGICXX_TESTS
    "Build the unit tests"
    OFF
)

option(BUILD_MAGICXX_SHARED_LIB
    "Build Recognition::Magicxx shared library"
    ON
)
```

### Feature Summary

Use `FeatureSummary` for option reporting:

```cmake
include(FeatureSummary)

add_feature_info("Unit Tests" BUILD_MAGICXX_TESTS
    "Build unit tests using GoogleTest"
)

feature_summary(
    WHAT ENABLED_FEATURES DISABLED_FEATURES
    DESCRIPTION "Build configuration:"
)
```

## Comments

### Section Headers

Use section headers to organize CMake files:

```cmake
# -----------------------------------------------------------------------------
# Build shared library
# -----------------------------------------------------------------------------
if(BUILD_MAGICXX_SHARED_LIB)
    configure_magicxx_target(magicxx Magicxx SHARED)
endif()
```

### File Headers

Include SPDX license headers:

```cmake
# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only
```

### Inline Comments

Use inline comments sparingly for non-obvious logic:

```cmake
# Windows requires additional regex library
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_dependencies(configure_file configure_gnurx)
endif()
```

## Best Practices

### 1. Use Helper Functions

Create functions to reduce duplication:

```cmake
function(configure_magicxx_target target_name alias_name library_type)
    add_library(${target_name} ${library_type})
    add_library(Recognition::${alias_name} ALIAS ${target_name})
    # Common configuration...
endfunction()
```

### 2. Prefer `FATAL_ERROR`

Use `FATAL_ERROR` with `cmake_minimum_required`:

```cmake
cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)
```

### 3. Use Presets

Prefer CMake presets over command-line options for reproducible builds:

```bash
cmake --workflow --preset linux-x86_64-clang-tests
```

### 4. Validate Requirements

Check for required tools and fail early:

```cmake
find_program(CLANG_FORMAT clang-format)
if(NOT CLANG_FORMAT)
    message(FATAL_ERROR "clang-format not found!")
endif()
```

### 5. Component-Based Installation

Use components for granular installation:

```cmake
install(
    TARGETS magicxx
    EXPORT MagicxxTargets
    LIBRARY DESTINATION ${lib_dir}
    COMPONENT Library
)

install(
    FILES ${header_files}
    DESTINATION ${include_dir}
    COMPONENT Development
)
```

### 6. Export Targets Properly

Use namespaced aliases for exported targets:

```cmake
install(
    EXPORT MagicxxTargets
    NAMESPACE Recognition::
    DESTINATION ${cmake_dir}
)
```

## References

+ [CMake Documentation](https://cmake.org/documentation/)

+ [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)

+ [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1)

+ [CMake Best Practices](https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html)
