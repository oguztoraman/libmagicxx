# Libmagicxx

A C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library).

## Requirements

+ Git
+ GCC or Clang (C++23)
+ CMake
+ GNU Make
+ Libtool
+ Autoconf

## Usage

1. Clone the libmagicxx repo into your project.

```
git clone https://github.com/oguztoraman/libmagicxx
```

2. Run the [setup.sh](https://github.com/oguztoraman/libmagicxx/blob/main/setup.sh) bash script to install the requirements and build the Magic Number Recognition Library.

```
cd libmagicxx && ./setup.sh
```

3. Add the following lines to the top level CMakeLists.txt file of your project.

```
add_subdirectory(libmagicxx)

target_link_libraries(<name of your project>
    <PUBLIC or PRIVATE or INTERFACE> magicxx
)
```

## License

### Libmagicxx

GNU Lesser General Public License Version 3, see the [COPYING.LESSER](https://github.com/oguztoraman/libmagicxx/blob/main/COPYING.LESSER) file for details.

### File and the Magic Number Recognition Library

See the [COPYING](https://github.com/file/file/blob/master/COPYING) file for details.
