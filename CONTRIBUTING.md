# Contributing to Libmagicxx

Welcome to the Libmagicxx project! We appreciate your interest in contributing. Whether you are fixing bugs, adding features, improving documentation, or suggesting enhancements, your contributions are valuable and help make Libmagicxx better for everyone.

## Table of Contents
+ [Code of Conduct](#code-of-conduct)
+ [Development](#development)
+ [Project Structure](#project-structure)
+ [Build Requirements for Libmagicxx](#build-requirements-for-libmagicxx)
+ [CMake Options of Libmagicxx](#cmake-options-of-libmagicxx)
+ [Building Libmagicxx](#building-libmagicxx)
+ [Getting Started](#getting-started)
+ [Making Changes](#making-changes)
+ [Creating a Pull Request](#creating-a-pull-request)
+ [Reporting Issues](#reporting-issues)
+ [Continuous Integration (CI)](#continuous-integration-ci)
+ [Review Process](#review-process)
+ [Thank You](#thank-you)

## Code of Conduct

We expect all contributors to adhere to the [Code of Conduct](https://github.com/oguztoraman/libmagicxx/blob/main/CODE_OF_CONDUCT.md). Please read it to understand the standards of behavior we expect from our community.

## Development

+ The `main` branch is the development branch for the next feature release. The `main` branch contains the tags of the feature releases (e.g., `v5.2.0`).

+ Each release is supported until the next feature release. The maintenance of the release is done in its own branch, (e.g., `v5.2.x`). These branches contain the tags of the bugfix releases, (e.g., `v5.2.1`).

+ Each change for the bugfix release is merged to the `main` branch.

## Project Structure

```
libmagicxx/
├── .github/                   # Continuous Integration (CI)
├── build/                     # Build directory (Git ignored)
├── cmake/                     # CMake configuration files, custom CMake targets
├── databases/                 # Default database files
├── documentation/             # Documentation files
├── examples/                  # Examples
├── external/                  # External projects
    ├── file/                  # File and the Magic Number Recognition Library
    ├── googletest/            # GoogleTest
    ├── libgnurx/              # Regex library
├── include/                   # Header files
├── packages                   # DEB/RPM/NSIS/ZIP/SOURCE Packages (Git ignored)
├── scripts/                   # Utility scripts
├── sources/                   # Source files
├── tests/                     # Test files
├── .clang-format              # Clang format configuration
├── .gitattributes             # Git attributes file
├── .gitignore                 # Git ignore file
├── .gitmodules                # Git submodules configuration
├── CHANGELOG.md               # Changelog
├── CMakeLists.txt             # Top-level CMake file
├── CMakePresets.json          # CMake presets
├── CODE_OF_CONDUCT.md         # Code of Conduct
├── Containerfile              # Container image configuration
├── CONTRIBUTING.md            # Contribution guidelines
├── COPYING.LESSER             # License file
├── PULL_REQUEST_TEMPLATE.md   # Pull request template
├── README.md                  # Project details
├── RELEASE_PROCEDURE.md       # Release procedure
├── SECURITY.md                # Security policy
```

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

## CMake Options of Libmagicxx

Below is a table describing the CMake configuration options of Libmagicxx.

| Option                        | Default | Description                                |
|-------------------------------|---------|--------------------------------------------|
| `INSTALL_MAGICXX`             | ON      | Enable installation of Libmagicxx.         |
| `BUILD_MAGICXX_SHARED_LIB`    | ON      | Build recognition::magicxx shared library. |
| `BUILD_MAGICXX_STATIC_LIB`    | ON      | Build recognition::magicxx_static library. |
| `BUILD_MAGICXX_TESTS`         | OFF     | Build the tests.                           |
| `BUILD_MAGICXX_EXAMPLES`      | OFF     | Build the examples.                        |
| `BUILD_MAGICXX_DOCUMENTATION` | OFF     | Build the documentation.                   |

## Building Libmagicxx

The recommended way to build Libmagicxx is to use the provided development container, which ensures a consistent and fully supported build environment. To do this, follow the steps in the **Getting Started** section above. This will set up the container with all required dependencies and tools, allowing you to build, test, and develop Libmagicxx seamlessly.

Alternatively, you may attempt to build Libmagicxx by manually installing all the required dependencies listed in the **Build Requirements for Libmagicxx** section. However, this method is **not tested or officially supported**, and you may encounter issues related to missing or incompatible dependencies.

For best results, always use the development container workflow described in this guide.

## Getting Started

The current development environment is a container image built on top of the latest stable release of [Fedora](https://fedoraproject.org/).

1. **Install Visual Studio Code**

    Download and install Visual Studio Code from the [official website](https://code.visualstudio.com/).

2. **Install the Dev Containers Extension**

    + Open Visual Studio Code.
    + Go to the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window or by pressing `Ctrl+Shift+X`.
    + Search for `Dev Containers`.
    + Click `Install` on the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension.

3. **Install Podman**

    Follow the [installation guide](https://podman.io/getting-started/installation) to install Podman.

4. **Install Git & Git Large File Storage**

    Download and install Git from the [official website](https://git-scm.com/).
    Download and install Git Large File Storage from the [official website](https://git-lfs.com/).

5. **Install Python**

    Download and install Python from the [official website](https://www.python.org/).

6. **Fork & Clone the Repository**

    Fork the repository on GitHub and clone it to your local machine:

    ```bash
    git clone https://github.com/your-username/libmagicxx.git

    cd libmagicxx
    ```

7. **Run the Launch Container Script**

    Open the project in Visual Studio Code and run the following script in a terminal to launch the development container:

    On Linux/MacOS;

    ```bash
    python ./scripts/launch_container.py
    ```

    On Windows;

    ```powershell
    python .\scripts\launch_container.py
    ```

8. **Attach to the Container**

   Press `Ctrl+Shift+P` in Visual Studio Code, then select `Remote-Containers: Attach to Running Container...` and choose the container `libmagicxx_dev_env` you just launched.

9. **Install Extensions in the Container**

    Once you have attached to the container, you need to install the necessary Visual Studio Code extensions within the container environment as well. Follow these steps:

    + Open the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window or by pressing `Ctrl+Shift+X`.

    + Search for and install the following extensions:
        + [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
        + [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
        + [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
        + [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python)

    These extensions will enhance your development experience within the container by providing features like code completion, debugging, and CMake integration.

10. **Initialize the Project**

    Open a new terminal initialize the project using the `initialize.sh` bash script;
    
    ```
    ./scripts/initialize.sh 
    ```

11. **Now You Are Ready for Your Changes**

    You can commit your changes, build the project, run tests, and execute scripts within the container. **However, you cannot perform `git push`, `git pull`, or `git fetch` operations directly from the container.** Once you are done, close the container connection and perform these `git` operations from your local machine (the host). This is necessary because authentication restrictions in the container environment prevent these operations from working correctly.

## Making Changes

1. **Create a New Branch**:

    + Create the branch from `main` if your change is scheduled for the next feature release. If you are fixing an issue for a supported release, create your branch from the supported release branch (e.g., `v5.2.x`).

    + Use the following naming conventions for your branches:
        + For bug fixes: `bugfix/brief_description`
        + For documentation changes: `document/brief_description`
        + For enhancements: `enhancement/brief_description`
        + For code quality improvements: `quality/brief_description`

2. **Make Changes**:

    + Use `snake_case` naming convention. The only exception is template parameters, which should use `PascalCase`.

    + Ensure that your code follows the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

    + To maintain high-quality documentation, ensure that all public APIs are 100% documented using `Doxygen`. For more information on how to write Doxygen comments, refer to the [Doxygen Manual](http://www.doxygen.nl/manual/docblocks.html).

    + If your changes introduce new features or significant modifications, add an example to demonstrate the usage.

3. **Build and Test**:

    Use the `workflows.sh` script to configure, build and test the project via CMake workflow presets:

    ```bash
    ./scripts/workflows.sh -p linux-x86_64-gcc-tests -c
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

    Ensure that your changes do not break any existing tests. If you are adding new features or fixing bugs, add the necessary unit tests to cover your changes.

4. **Format Code**:

    Ensure your code is properly formatted using the CMake workflow preset `format-source-code`:

    ```bash
    ./scripts/workflows.sh -p format-source-code
    ```

5. **Update the Changelog**:

    Add a summary of your changes to the `CHANGELOG.md` file under the appropriate section (e.g., Next Release). This helps keep track of all improvements, bug fixes, and new features for each release.

## Creating a Pull Request

+ Each pull request must fix an existing issue. Please ensure that there is a linked issue or create a new issue before submitting your pull request.

+ Create a pull request to the `main` branch if your change is scheduled for the next feature release. If you are fixing an issue for a supported release, create a pull request to the supported release branch (e.g., `v5.2.x`) and also the `main` branch.

+ Use the following naming conventions for your pull request title:
    + For bug fixes: `[BUGFIX] Brief Description, Fixes issue #????.`
    + For documentation changes: `[DOCUMENTATION] Brief Description, Fixes issue #????.`
    + For enhancements: `[ENHANCEMENT] Brief Description, Fixes issue #????.`
    + For code quality improvements: `[QUALITY] Brief Description, Fixes issue #????.`

+ Fill the [PULL_REQUEST_TEMPLATE.md](https://github.com/oguztoraman/libmagicxx/blob/main/PULL_REQUEST_TEMPLATE.md).

## Reporting Issues

If you find a bug or have a feature request, please create an issue on the [GitHub Issues](https://github.com/oguztoraman/libmagicxx/issues) page. Provide as much detail as possible to help us understand and address the issue.

## How to Create an Issue

1. Add a title summarizing the issue.

2. Fill the issue template.

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
