# Contributing to Libmagicxx

Welcome to the Libmagicxx project! We appreciate your interest in contributing. Whether you are fixing bugs, adding features, improving documentation, or suggesting enhancements, your contributions are valuable and help make Libmagicxx better for everyone.

## Project Structure

```
libmagicxx/
├── .github/                   # Continuous Integration (CI)
├── doc/                       # Documentation files
├── examples/                  # Examples
├── file/                      # File and the Magic Number Recognition Library
├── googletest                 # GoogleTest
├── inc/                       # Header files
├── scripts/                   # Utility scripts
├── src/                       # Source files
├── tests/                     # Test files
├── .clang-format              # Clang format configuration
├── .gitignore                 # Git ignore file
├── .gitmodules                # Git submodules configuration
├── CHANGELOG.md               # Changelog
├── CMakeLists.txt             # CMake build configuration
├── CODE_OF_CONDUCT.md         # Code of Conduct
├── CONTRIBUTING.md            # Contribution guidelines
├── COPYING.LESSER             # License file
├── Doxyfile                   # Doxygen configuration
├── ISSUE_TEMPLATE.md          # Issue template
├── PULL_REQUEST_TEMPLATE.md   # Pull request template
├── README.md                  # Project details
├── SECURITY.md                # Security policy
├── TODO.md                    # To do list
```

## Development

+ The `main` branch is the development branch for the next feature release. The `main` branch contains the tags of the feature releases (e.g., `v5.2.0`).

+ Each release is supported until the next feature release. The maintenance of the release is done in its own branch, (e.g., `v5.2.x`). These branches contain the tags of the bugfix releases, (e.g., `v5.2.1`).

+ Each change for the bugfix release is merged to the `main` branch.

## Documentation

For comprehensive guides, API references, and detailed information, visit the [documentation site](https://oguztoraman.github.io/libmagicxx/).

## Code of Conduct

We expect all contributors to adhere to the [Code of Conduct](https://github.com/oguztoraman/libmagicxx/blob/main/CODE_OF_CONDUCT.md). Please read it to understand the standards of behavior we expect from our community.

## Getting Started

1. **Fork & Clone the Repository**:

    ```bash
    git clone https://github.com/your-username/libmagicxx.git
    cd libmagicxx
    ```

2. **Set up Development Environment**:

+ The current development environment is the latest stable release of Fedora. You can install the necessary dependencies using the `install_dependencies.sh` script:

    ```bash
    ./scripts/install_dependencies.sh
    ```

## How to Use Libmagicxx in a CMake-based Project

1. Add the following lines to the top level `CMakeLists.txt` file of your project to include and link libmagicxx.

    ```cmake
    add_subdirectory(libmagicxx)

    add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>")

    target_include_directories(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> ${magicxx_INCLUDE_DIR}
    )

    target_link_libraries(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> "magicxx;$<$<CXX_COMPILER_ID:Clang>:c++>"
    )
    ```

2. Include the `magic.hpp` header file in your source files.

## Making Changes

1. **Create a New Branch**:

+ Create the branch from `main` if your change is scheduled for the next feature release. If you are fixing an issue for a supported release, create your branch from the supported release branch (e.g., `v5.2.x`).

+ Use the following naming conventions for your branches:
    + For bug fixes: `bugfix/brief_description`
    + For documentation changes: `document/brief_description`
    + For enhancements: `enhancement/brief_description`

2. **Make Changes**:

+ Make your changes to the codebase. Ensure that your code follows the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

+ To maintain high-quality documentation, ensure that all public APIs are 100% documented using `Doxygen`. For more information on how to write Doxygen comments, refer to the [Doxygen Manual](http://www.doxygen.nl/manual/docblocks.html).

+ If your changes introduce new features or significant modifications, add an example to demonstrate the usage.

3. **Build and Test**:

+ Use the `build.sh` script to build and test the project:

    ```bash
    ./scripts/build.sh -t
    ```

+ For more options, use:

    ```bash
    ./scripts/build.sh -h
    Usage: ./scripts/build.sh [-d build_dir] [-b build_type] [-c compiler] [-t] [-e] [-h]
      -d build_dir   Specify the build directory (default: release_build).
      -b build_type  Specify the CMake build type (default: Release).
      -c compiler    Specify the compiler (g++ or clang++, default: g++).
      -t             Build and run tests (default: OFF).
      -e             Build and run examples (default: OFF).
      -r             Rebuild libmagic (default: OFF).
      -h             Display this message.
    ```

+ Ensure that your changes do not break any existing tests. If you are adding new features or fixing bugs, add the necessary unit tests to cover your changes.

4. **Format Code**:

+ Ensure your code is properly formatted using the `format.sh` script:

    ```bash
    ./scripts/format.sh
    ```

## Creating a Pull Request

+ Each pull request must fix an existing issue. Please ensure that there is a linked issue or create a new issue before submitting your pull request.

+ Create a pull request to the `main` branch if your change is scheduled for the next feature release. If you are fixing an issue for a supported release, create a pull request to the supported release branch (e.g., `v5.2.x`) and also the `main` branch.

+ Use the following naming conventions for your pull request title:
    + For bug fixes: `BUGFIX: Brief Description, Fixes issue #????.`
    + For documentation changes: `DOCUMENTATION: Brief Description, Fixes issue #????.`
    + For enhancements: `ENHANCEMENT: Brief Description, Fixes issue #????.`

+ Fill the [PULL_REQUEST_TEMPLATE.md](https://github.com/oguztoraman/libmagicxx/blob/main/PULL_REQUEST_TEMPLATE.md).

## Reporting Issues

If you find a bug or have a feature request, please create an issue on the [GitHub Issues](https://github.com/oguztoraman/libmagicxx/issues) page. Provide as much detail as possible to help us understand and address the issue.

## How to Create an Issue

1. Add a title summarizing the issue.

2. Fill the [ISSUE_TEMPLATE.md](https://github.com/oguztoraman/libmagicxx/blob/main/ISSUE_TEMPLATE.md).

## Continuous Integration (CI)

Our CI pipeline runs automated tests and checks on each pull request to ensure code quality and correctness.

## Review Process

1. **Initial Review**:
+ Once you submit a pull request, it will be reviewed by one or more project maintainers.

+ The maintainers will check if the pull request follows the contribution guidelines, including coding standards, documentation, and testing.

2. **Feedback and Revisions**:
+ If the maintainers find any issues or have suggestions, they will provide feedback on the pull request.

+ You are expected to address the feedback by making the necessary changes and updating the pull request.

3. **Approval and Merging**:
+ Once the maintainers are satisfied with the changes, they will approve the pull request.

+ The pull request will then be merged into the appropriate branch (`main` or a supported release branch).

4. **Post-Merge**:
+ After merging, the CI pipeline will run automated tests to ensure that the changes do not introduce any new issues.

+ If any issues are found, they will be addressed promptly.

## Thank You

Thank you for contributing to Libmagicxx! Your efforts help improve the project for everyone.
