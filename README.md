![Build & Test on Linux using GCC](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_on_linux_using_gcc.yml/badge.svg) ![Build & Test on Linux using CLANG](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_on_linux_using_clang.yml/badge.svg) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=oguztoraman_libmagicxx&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=oguztoraman_libmagicxx)

# Libmagicxx

**Libmagicxx** is a C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library). It provides an easy-to-use interface through the `magic` class, enabling the *identification of file types* based on their content rather than file extensions.

## Key Features

+ **Modern C++**: Utilizes modern C++ standards and features, ensuring clean, efficient, and maintainable code.
+ **High-Level API**: Provides a simple and intuitive high-level API for easy integration and use.
+ **Simple Integration with CMake**: Easily integrate file type recognition into your CMake-based C++ projects.

## Requirements to Build Libmagicxx

+ Git
+ GCC with libstdc++ or Clang with libc++ (C++23)
+ CMake
+ GNU Make
+ Ninja
+ Libtool
+ Autoconf
+ Mold

## How to Build Libmagicxx

1. Clone the libmagicxx repo.

    ```bash
    git clone https://github.com/oguztoraman/libmagicxx
    ```

2. Install the necessary dependencies using the [install_dependencies.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/install_dependencies.sh) bash script.

    ```bash
    cd libmagicxx && ./scripts/install_dependencies.sh
    ```

3. Build the project using the [workflows.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/workflows.sh) bash script.

    ```bash
    ./scripts/workflows.sh -p linux-x86_64-gcc-shared-release
    ```

    For more options, use:

    ```bash
    ./scripts/workflows.sh -h
    Usage: ./scripts/workflows.sh [-l] [-p preset] [-h]
      -l              List available CMake workflow presets.
      -p preset       Specify the CMake workflow preset to use.
      -h              Display this message.
    ```

## How to Use Libmagicxx in a CMake-based Project

1. Clone the libmagicxx repo into your project.

    ```bash
    git clone https://github.com/oguztoraman/libmagicxx
    ```

2. Install the necessary dependencies using the [install_dependencies.sh](https://github.com/oguztoraman/libmagicxx/blob/main/scripts/install_dependencies.sh) bash script.

    ```bash
    cd libmagicxx && ./scripts/install_dependencies.sh
    ```

3. Add the following lines to the top level `CMakeLists.txt` file of your project to include and link libmagicxx.

    ```cmake
    add_subdirectory(libmagicxx)

    target_include_directories(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> ${magicxx_INCLUDE_DIR}
    )

    target_link_libraries(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> magicxx
    )
    ```

4. Include the `magic.hpp` header file in your source files. Below is an example code snippet that demonstrates how to print the MIME type of the default database file using the `magic` class.

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

For comprehensive guides, API references, and detailed information, visit the [documentation site](https://oguztoraman.github.io/libmagicxx/).

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
