# Magic++

A C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library).

## Requirements

+ Git
+ GCC or Clang (C++23)
+ CMake
+ GNU Make
+ Libtool
+ Autoconf

## Usage

1. Clone the magic-cpp repo into your project.

```
git clone https://github.com/oguztoraman/magic-cpp
```

2. Run the [setup.sh](https://github.com/oguztoraman/magic-cpp/blob/main/setup.sh) bash script to install the requirements and build the Magic Number Recognition Library.

```
cd magic-cpp && ./setup.sh
```

3. Add the following lines to the top level CMakeLists.txt file of your project.

```
add_subdirectory(magic-cpp)

target_link_libraries(<name of your project>
    <PUBLIC or PRIVATE or INTERFACE> magic++
)
```

## License

### Magic++

GNU Lesser General Public License Version 3, see the [COPYING.LESSER](https://github.com/oguztoraman/magic-cpp/blob/main/COPYING.LESSER) file for details.

### File and the Magic Number Recognition Library

See the [COPYING](https://github.com/file/file/blob/master/COPYING) file for details.

## To Do

+ [ ] Add tests.
+ [ ] Add the documentation.
+ [ ] Use modules.
+ [ ] Build as a CMake package.
+ [ ] Build as an RPM package.
