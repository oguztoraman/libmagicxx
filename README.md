# Libmagicxx

**Libmagicxx** is a C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library). It provides an easy-to-use interface through the `magic` class, enabling the *identification of file types* based on their content rather than file extensions.

## Key Features

+ **Modern C++ (C++23)**: Utilizes modern C++ standards and features, ensuring clean, efficient, and maintainable code.
+ **High-Level API**: Provides a simple and intuitive high-level API for easy integration and use.
+ **Simple Integration with CMake**: Easily integrate file type recognition into your CMake-based C++ projects.
+ **Cross Platform**: Compatible with both Windows and Linux, ensuring broad usability and flexibility in various development environments.

## Project Status

![Linux GCC](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_linux_gcc.yml/badge.svg) ![Linux Clang](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_linux_clang.yml/badge.svg) ![Windows MinGW64](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_windows_mingw64.yml/badge.svg)

## Code Quality

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/e7067c7f0a87475c8f0ceda254b9a240)](https://app.codacy.com/gh/oguztoraman/libmagicxx/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=oguztoraman_libmagicxx&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=oguztoraman_libmagicxx)

## Build Requirements for Libmagicxx

To build Libmagicxx, ensure you have the following tools and dependencies installed:

+ **Git**: Version control system.
+ **Git LFS**: Git extension for versioning large files.
+ **Awk**: Text processing tool.
+ **Autoconf**: Tool for generating configuration scripts.
+ **GNU Make**: Build automation tool.
+ **Libtool**: Generic library support script.
+ **CMake**: Cross-platform build system.
+ **Ninja**: Small build system with a focus on speed.
+ **Toolchains**: One of the following toolchains is required:
    + **GCC**: GNU Compiler Collection (libstdc++, Mold Linker)
    + **Clang**: C language family frontend for LLVM (libc++, Mold Linker)
    + **MinGW64**: Minimalist GNU for Windows (libstdc++exp, LD Linker)

## Building Libmagicxx

1. Clone the libmagicxx repo.

    ```bash
    git clone https://github.com/oguztoraman/libmagicxx
    ```

2. Install the necessary dependencies using the [install_dependencies.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/install_dependencies.sh) bash script.

    ```bash
    cd libmagicxx && ./scripts/install_dependencies.sh
    ```

3. Initialize the project using the [initialize.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/initialize.sh) bash script.

    ```bash
    ./scripts/initialize.sh
    ```

4. Build the project using the [workflows.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/workflows.sh) bash script.

    ```bash
    ./scripts/workflows.sh -p linux-x86_64-gcc -c
    ```

    For more options, use:

    ```bash
    ./scripts/workflows.sh -h
    Usage: ./scripts/workflows.sh [-l] [-p preset] [-c] [-h]
      -l              List available CMake workflow presets.
      -p preset       Specify the CMake workflow preset to use.
      -c              Clear the CMake cache before running the selected preset.
      -h              Display this message.
    ```

## Installing Libmagicxx

+ Install Libmagicxx using CMake.

    ```bash
    cmake --install build/ --strip
    ```

+ Install Libmagicxx packages.

    **For Debian-based Linux distributions**: Use the `apt` command to install the `.deb` package.

    ```bash
    sudo apt install libmagicxx-<version>-linux-x86_64.deb
    ```

    **For Red Hat-based Linux distributions**: Use the `dnf` command to install the `.rpm` package.

    ```bash
    sudo dnf install --setopt=tsflags=nodocs libmagicxx-<version>-linux-x86_64.rpm
    ```

    **For Windows**: Install the `libmagicxx-<version>-windows-x86_64.exe` package provided by the NSIS installer. Run the installer and follow the on-screen instructions to complete the installation.

## Uninstalling Libmagicxx

+ Uninstall Libmagicxx using CMake.

    ```bash
    cmake --build build/ --target uninstall
    ```

+ Uninstall Libmagicxx packages.

    **For Debian-based Linux distributions**: Use the `apt` command to remove the installed `.deb` package.

    ```bash
    sudo apt remove libmagicxx
    ```

    **For Red Hat-based Linux distributions**: Use the `dnf` command to remove the installed `.rpm` package.

    ```bash
    sudo dnf remove libmagicxx
    ```

    **For Windows**: Use the uninstaller provided by the NSIS installer. Navigate to the "Add or Remove Programs" section in the Control Panel, find `libmagicxx`, and click "Uninstall."

## Using Libmagicxx in a CMake Project Without Installation

1. Clone the libmagicxx repo into your project.

    ```bash
    git clone https://github.com/oguztoraman/libmagicxx
    ```

2. Install the necessary dependencies using the [install_dependencies.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/install_dependencies.sh) bash script.

    ```bash
    cd libmagicxx && ./scripts/install_dependencies.sh
    ```

3. Initialize the project using the [initialize.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/initialize.sh) bash script.

    ```bash
    ./scripts/initialize.sh
    ```

4. Add the following lines to the top level `CMakeLists.txt` file of your project to include and link Libmagicxx.

    ```cmake
    add_subdirectory(libmagicxx)

    target_link_libraries(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> <recognition::magicxx or recognition::magicxx_static>
    )
    ```

5. Include the `magic.hpp` header file in your source files.

## Using Installed Libmagicxx with CMake

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

## Contributing

We welcome contributions! Please see the [CONTRIBUTING.md](https://github.com/oguztoraman/libmagicxx/blob/main/CONTRIBUTING.md) file for guidelines on how to contribute.

## Code of Conduct

We expect all contributors to adhere to the [Code of Conduct](https://github.com/oguztoraman/libmagicxx/blob/main/CODE_OF_CONDUCT.md). Please read it to understand the standards of behavior we expect from our community.

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
