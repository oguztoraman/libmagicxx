# Contributing to Libmagicxx

Welcome to the Libmagicxx project! We appreciate your interest in contributing. Whether you are fixing bugs, adding features, improving documentation, or suggesting enhancements, your contributions are valuable and help make Libmagicxx better for everyone.

## Project Structure

```
libmagicxx/
├── .github/                   # Continuous Integration (CI)
├── cmake/                     # CMake configuration files, custom CMake targets.
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
├── CMakeLists.txt             # Top-level CMake file
├── CMakePresets.json          # CMake presets
├── CODE_OF_CONDUCT.md         # Code of Conduct
├── Containerfile              # Container image configuration
├── CONTRIBUTING.md            # Contribution guidelines
├── COPYING.LESSER             # License file
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

4. **Install Git**

    Download and install Git from the [official website](https://git-scm.com/).

5. **Fork & Clone the Repository**

    Fork the repository on GitHub and clone it to your local machine:

    ```bash
    git clone https://github.com/your-username/libmagicxx.git

    cd libmagicxx
    ```

6. **Run the Launch Container Script**

    Open a terminal in Visual Studio Code and run the following script:

    > On Linux/MacOS

    ```bash
    ./scripts/launch_container.sh
    ```

    > On Windows

    ```powershell
    ./scripts/launch_container.ps1
    ```

7. **Attach to the Container**

   Press `Ctrl+Shift+P` in Visual Studio Code, then select `Remote-Containers: Attach to Running Container...` and choose the container `libmagicxx_dev_env` you just launched.

8. **Install Extensions in the Container**

    Once you have attached to the container, you need to install the necessary Visual Studio Code extensions within the container environment as well. Follow these steps:

    + Open the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window or by pressing `Ctrl+Shift+X`.

    + Search for and install the following extensions:
        + [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
        + [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
        + [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

    These extensions will enhance your development experience within the container by providing features like code completion, debugging, and CMake integration.

9. **Initialize the Project**

    Use the CMake workflow preset `initialize` to initialize the project.

    ```bash
    cd libmagicxx && ./scripts/workflows.sh -p initialize
    ```

10. Now you are ready for your changes. You can commit your changes, build the project, run tests, and execute scripts within the container. However, you cannot push these changes directly from the container. Once you are done, close the container connection and push your work from your local computer.

## How to Use Libmagicxx in a CMake-based Project

1. Add the following lines to the top level `CMakeLists.txt` file of your project to include and link libmagicxx.

    ```cmake
    add_subdirectory(libmagicxx)

    target_include_directories(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> ${magicxx_INCLUDE_DIR}
    )

    target_link_libraries(<name of your project>
        <PUBLIC or PRIVATE or INTERFACE> magicxx
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

+ Use `snake_case` naming convention. The only exception is template parameters, which should use `PascalCase`.

+ Ensure that your code follows the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

+ To maintain high-quality documentation, ensure that all public APIs are 100% documented using `Doxygen`. For more information on how to write Doxygen comments, refer to the [Doxygen Manual](http://www.doxygen.nl/manual/docblocks.html).

+ If your changes introduce new features or significant modifications, add an example to demonstrate the usage.

3. **Build and Test**:

+ Use the `workflows.sh` script to configure, build and test the project via CMake workflow presets:

    ```bash
    ./scripts/workflows.sh -p linux-x86_64-gcc-shared-tests-release
    ```

+ For more options, use:

    ```bash
    ./scripts/workflows.sh -h
    Usage: ./scripts/workflows.sh [-l] [-p preset] [-h]
      -l              List available CMake workflow presets.
      -p preset       Specify the CMake workflow preset to use.
      -h              Display this message.
    ```

+ Ensure that your changes do not break any existing tests. If you are adding new features or fixing bugs, add the necessary unit tests to cover your changes.

4. **Format Code**:

+ Ensure your code is properly formatted using the CMake workflow preset `format-source-code`:

    ```bash
    ./scripts/workflows.sh -p format-source-code
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
