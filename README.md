# Libmagicxx

[![Latest Release](https://img.shields.io/github/v/release/oguztoraman/libmagicxx?label=release)](https://github.com/oguztoraman/libmagicxx/releases/latest)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

**Libmagicxx** is a C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library). It provides an easy-to-use interface through the `magic` class, enabling the *identification of file types* based on their content rather than file extensions.

## Table of Contents
+ [Key Features](#key-features)
+ [Supported Platforms](#supported-platforms)
+ [Project Status](#project-status)
+ [Code Quality](#code-quality)
+ [Quick Start](#quick-start)
+ [Downloading Libmagicxx](#downloading-libmagicxx)
+ [Installing Libmagicxx](#installing-libmagicxx)
+ [Using Libmagicxx with CMake](#using-libmagicxx-with-cmake)
+ [Examples](#examples)
+ [Documentation](#documentation)
+ [Uninstalling Libmagicxx](#uninstalling-libmagicxx)
+ [Contributing](#contributing)
+ [Security](#security)
+ [Source Code](#source-code)
+ [License](#license)
+ [Disclaimer](#disclaimer)

## Key Features

+ **Modern C++ (C++23)**: Utilizes modern C++ standards and features, ensuring clean, efficient, and maintainable code.
+ **High-Level API**: Provides a simple and intuitive high-level API for easy integration and use.
+ **Simple Integration with CMake**: Easily integrate file type recognition into your CMake-based C++ projects.
+ **Cross Platform**: Compatible with both Windows and Linux, ensuring broad usability and flexibility in various development environments.
+ **Comprehensive Documentation**: Detailed documentation and examples to help you get started quickly and effectively.

## Supported Platforms

+ Linux (x86_64)
+ Windows (x86_64)

## Project Status

![Linux GCC](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_linux_gcc.yml/badge.svg) ![Linux Clang](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_linux_clang.yml/badge.svg) ![Windows MinGW64](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_windows_mingw64.yml/badge.svg)

## Code Quality

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/e7067c7f0a87475c8f0ceda254b9a240)](https://app.codacy.com/gh/oguztoraman/libmagicxx/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=oguztoraman_libmagicxx&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=oguztoraman_libmagicxx)

## Quick Start

1. Download the latest release from the [GitHub Releases page](https://github.com/oguztoraman/libmagicxx/releases/latest).

2. Install the package for your platform (see instructions below).

3. Link Libmagicxx in your CMake project (see usage instructions below).

## Downloading Libmagicxx

You can download the latest release packages and source code directly from the [GitHub Releases page](https://github.com/oguztoraman/libmagicxx/releases).

+ **[Latest Release](https://github.com/oguztoraman/libmagicxx/releases/latest)**: Download the most recent version of Libmagicxx, including pre-built packages for Linux and Windows, as well as the source code archive.
+ **[All Releases](https://github.com/oguztoraman/libmagicxx/releases)**: Browse and download previous versions.

After downloading, follow the installation instructions below for your platform.

## Installing Libmagicxx

**For Debian-based Linux distributions**: Use the `apt` command to install the `.deb` package.

```bash
sudo apt install libmagicxx-<version>-linux-x86_64.deb
```

**For Red Hat-based Linux distributions**: Use the `dnf` command to install the `.rpm` package.

```bash
sudo dnf install libmagicxx-<version>-linux-x86_64.rpm
```

**For Windows**: Install the `libmagicxx-<version>-windows-x86_64.exe` package provided by the NSIS installer. Run the installer and follow the on-screen instructions to complete the installation.

## Using Libmagicxx with CMake

Libmagicxx is designed to be easily integrated into CMake-based projects. To use Libmagicxx in your project after installation, follow these steps:

1. Add the following lines to the top level `CMakeLists.txt` file of your project to include and link Libmagicxx.

    ```cmake
    find_package(magicxx REQUIRED)

    target_link_libraries(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> <recognition::magicxx or recognition::magicxx_static>
    )
    ```

2. Include the `magic.hpp` header file in your source files. Below is an example code snippet that demonstrates how to print the MIME type of the default database file using the `magic` class.

    ```cpp
    #include <print>
    #include <iostream>
    #include <magic.hpp>

    auto main(int, char**) -> int
    {
        using namespace recognition;
        magic m{magic::flags::mime};
        std::println(std::cout, "{}", m.identify_file(magic::default_database_file));
        // possible output: text/x-file; charset=us-ascii
    }
    ```

## Examples

For more examples, refer to the [examples directory](https://github.com/oguztoraman/libmagicxx/tree/main/examples).

## Documentation

For comprehensive guides, API references, and detailed information, visit the [Libmagicxx Documentation Site](https://oguztoraman.github.io/libmagicxx/). The source code for the documentation site is available in the `documentation/html` directory of the repository.

## Uninstalling Libmagicxx

**For Debian-based Linux distributions**: Use the `apt` command to remove the installed `.deb` package.

```bash
sudo apt remove libmagicxx
```

**For Red Hat-based Linux distributions**: Use the `dnf` command to remove the installed `.rpm` package.

```bash
sudo dnf remove libmagicxx
```

**For Windows**: Use the uninstaller provided by the NSIS installer. Navigate to the "Add or Remove Programs" section in the Control Panel, find `libmagicxx`, and click "Uninstall."

## Contributing

We welcome contributions of all kinds, including bug fixes, feature requests, documentation improvements, and code enhancements. Please see the [CONTRIBUTING.md](https://github.com/oguztoraman/libmagicxx/blob/main/CONTRIBUTING.md) file for detailed guidelines on how to contribute or build Libmagicxx from source.

## Security

For information on reporting security vulnerabilities, please see the [SECURITY.md](https://github.com/oguztoraman/libmagicxx/blob/main/SECURITY.md) file.

## Source Code

Explore the source code of Libmagicxx on [GitHub](https://github.com/oguztoraman/libmagicxx).

## License

Libmagicxx is licensed under the GNU Lesser General Public License Version 3. For more details, see the [COPYING.LESSER](https://github.com/oguztoraman/libmagicxx/blob/main/COPYING.LESSER) file.

### Other Licenses

+ File and the Magic Number Recognition Library: See the [COPYING](https://github.com/file/file/blob/master/COPYING) file.

+ GoogleTest: See the [LICENSE](https://github.com/google/googletest/blob/main/LICENSE) file.

+ Libgnurx: See the [LICENSE](https://github.com/TimothyGu/libgnurx/blob/master/COPYING.LIB) file.

## Disclaimer

This project is a personal hobby project developed in my own time and with my own resources.
It is not affiliated with, endorsed by, or created on behalf of any employer, company, or organization.
No proprietary or confidential information belonging to any employer or third party was used in its development.
This project is not intended for any commercial purpose, income generation, or professional service.
