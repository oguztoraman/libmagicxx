![Build & Test on Linux](https://github.com/oguztoraman/libmagicxx/actions/workflows/build_and_test_on_linux.yml/badge.svg)

# Libmagicxx

A C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library). It provides an easy-to-use interface through the magic class, enabling the identification of file types based on their content rather than file extensions.

## Key Features

+ **Modern C++**: Utilizes modern C++ standards and features, ensuring clean, efficient, and maintainable code.
+ **High-Level API**: Provides a simple and intuitive high-level API for easy integration and use.
+ **Simple Integration with CMake**: Easily integrate file type recognition into your CMake-based C++ projects.

## Requirements to Build Libmagicxx

+ Git
+ GCC with libstdc++ or Clang with libc++ (C++23)
+ CMake
+ GNU Make or Ninja
+ Libtool
+ Autoconf

## How to Build Libmagicxx

1. Clone the libmagicxx repo.

```bash
git clone https://github.com/oguztoraman/libmagicxx
```

2. Run the [install_dependencies.sh](https://github.com/oguztoraman/libmagicxx/blob/main/install_dependencies.sh) bash script to install the necessary dependencies.

```bash
cd libmagicxx && ./install_dependencies.sh
```

3. Run the [build.sh](https://github.com/oguztoraman/libmagicxx/blob/main/build.sh) bash script.

```bash
./build.sh -d build -b Release -c g++ -t
```

```bash
./build.sh -h
Usage: ./build.sh [-d build_dir] [-b build_type] [-c compiler] [-t] [-h]
  -d build_dir   Specify the build directory (default: release_build).
  -b build_type  Specify the CMake build type (default: Release).
  -c compiler    Specify the compiler (g++ or clang++, default: g++).
  -t             Build and run tests (default: OFF).
  -h             Display this message.
```

## How to Use Libmagicxx in a CMake-based Project

1. Clone the libmagicxx repo into your project.

```bash
git clone https://github.com/oguztoraman/libmagicxx
```

2. Run the [install_dependencies.sh](https://github.com/oguztoraman/libmagicxx/blob/main/install_dependencies.sh) bash script to install the necessary dependencies.

```bash
cd libmagicxx && ./install_dependencies.sh
```

3. Add the following lines to the top level CMakeLists.txt file of your project to include and link libmagicxx.

```cmake
add_subdirectory(libmagicxx)

add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>")

target_link_libraries(<name of your project>
    <PUBLIC or PRIVATE or INTERFACE> "magicxx;$<$<CXX_COMPILER_ID:Clang>:c++>"
)
```

4. Include the magic.hpp header file in your source files where you need to use Libmagicxx functionality.

```cpp
#include <print>
#include <iostream>
#include <magic.hpp>

auto main(int, char**)->int
{
    using namespace recognition;
    magic m{magic::flags::mime};
    std::println(std::cout, "{}", m.identify_file(magic::default_database_file));
    // possible output: text/x-file; charset=us-ascii
}
```

## Documentation

For comprehensive guides, API references, and detailed information, please visit the [documentation site](https://oguztoraman.github.io/libmagicxx/).

## Source Code

Explore the source code of Libmagicxx on [GitHub](https://github.com/oguztoraman/libmagicxx).

## License

+ Libmagicxx

GNU Lesser General Public License Version 3, see the [COPYING.LESSER](https://github.com/oguztoraman/libmagicxx/blob/main/COPYING.LESSER) file for details.

+ File and the Magic Number Recognition Library

See the [COPYING](https://github.com/file/file/blob/master/COPYING) file for details.

+ GoogleTest

See the [LICENSE](https://github.com/google/googletest/blob/main/LICENSE) file for details.
