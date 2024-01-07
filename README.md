# Magic++

A C++ wrapper library over the [Magic Number Recognition Library](https://github.com/file/file#readme-for-file1-command-and-the-libmagic3-library).

## Requirements

+ Git
+ GCC or Clang (C++23)
+ CMake
+ Magic Number Recognition Library

## Installing Requirements

> For Fedora

```
sudo dnf install cmake g++ clang git file-dev
```

## Usage

1. Clone the magic-cpp repo into your project.

```
git clone https://github.com/oguztoraman/magic-cpp
```

2. Add the following lines to the top level CMakeLists.txt file of your project.

```
add_subdirectory(magic-cpp)

target_link_libraries(<name of your project>   
    <PUBLIC or PRIVATE or INTERFACE> magic++
)
```

## License

GNU General Public License Version 3, see the see [COPYING](https://github.com/oguztoraman/magic-cpp/blob/main/COPYING) file for details.

## To Do
 
+ [ ] Add tests.
+ [ ] Add the documentation.
+ [ ] Use modules.
+ [ ] Build as a CMake package.
+ [ ] Build as an RPM package.
