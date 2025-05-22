# Changelog

## Next Release

+ [**ENHANCEMENT**] .github/ISSUE_TEMPLATE/*: Add version information to the issue templates.

+ [**ENHANCEMENT**] .github/workflows/deploy_release_packages.yml: Add project name to the release title when deploying release packages.

+ [**DOCUMENTATION**] CONTRIBUTING.md, README.md: Enhance documentation in README.md and CONTRIBUTING.md with detailed sections.

+ [**ENHANCEMENT**] README.md: Add downloading instructions for Libmagicxx in README.md.

+ [**DOCUMENTATION**] CONTRIBUTING.md: Clarify git operations restrictions within the container environment in CONTRIBUTING.md.

+ [**ENHANCEMENT**] scripts/publish_release.sh, RELEASE_PROCEDURE.md: Rename commit_release.sh to publish_release.sh and automate pushing new release tag and branch.

+ [**ENHANCEMENT**] CONTRIBUTING.md, RELEASE_PROCEDURE.md: Add release procedure documentation.

+ [**QUALITY**] .github/workflows/deploy_doxygen_documentation.yml: Remove unnecessary steps in Doxygen documentation deployment workflow.

+ [**QUALITY**] .github/workflows/*: Refactor GitHub Actions workflows to standardize formatting.

+ [**ENHANCEMENT**] .github/ISSUE_TEMPLATE/code-quality.md, CONTRIBUTING.md, PULL_REQUEST_TEMPLATE.md: Add code quality issue template and update contributing guidelines.

## [v8.1.1] - 21-05-2025

+ [**BUGFIX**] .github/workflows/deploy_release_packages.yml: Update permissions in deployment workflow to allow write access.

## [v8.1.0] - 21-05-2025

+ [**ENHANCEMENT**] .github/workflows/deploy_release_packages.yml, scripts/generate_release_packages.sh: Add GitHub Actions workflow for deploying release packages.

## [v8.0.1] - 21-05-2025

+ [**BUGFIX**] cmake/cpack_source.cmake, cmake/directories.cmake: Clean existing build files before generating source package.

+ [**BUGFIX**] SECURITY.md: Update supported versions in SECURITY.md to reflect version 8.0.x.

## [v8.0.0] - 21-05-2025

+ [**BUGFIX**] cmake/directories.cmake, cmake/files.cmake, cmake/format_source_code.cmake, cmake/generate_default_database_files.cmake: Fix CMake organization.

+ [**ENHANCEMENT**] .github/workflows/build*, cmake/toolchains/*, cmake/cpack*, cmake/directories.cmake, cmake/files.cmake, scripts/install_dependencies.sh, .gitignore, CMakeLists.txt, CMakePresets.json, CONTRIBUTING.md, README.md: Add support for generating DEB, RPM, NSIS, ZIP and source packages for easier installation and distribution.

+ [**ENHANCEMENT**] cmake/install.cmake: Add support for component-based installation in CMake.

+ [**ENHANCEMENT**] cmake/files.cmake, cmake/install.cmake: Install the license file.

+ [**ENHANCEMENT**] .github/workflows/*, CMakePresets.json, CONTRIBUTING.md, README.md: Erase the release tag from the CMake presets.

+ [**ENHANCEMENT**] cmake/toolchains/*, cmake/magicxxConfig.cmake.in, cmake/options.cmake, documentation/CMakeLists.txt: Format CMake files.

+ [**ENHANCEMENT**] cmake/directories.cmake, cmake/files.cmake, cmake/generate_default_database_files.cmake, CMakeLists.txt: Use the dir naming convention instead of path in the CMake files.

## [v7.0.0] - 24-04-2025

+ [**ENHANCEMENT**] .github/workflows/*, cmake/install.cmake, cmake/options.cmake, cmake/targets.cmake, examples/CMakeLists.txt, scripts/workflows.sh, tests/CMakeLists.txt, CMakeLists.txt, CMakePresets.json, CONTRIBUTING.md, README.md: Update build system to support simultaneous compilation of shared and static libraries.

+ [**ENHANCEMENT**] cmake/*, CMakeLists.txt: Refactor CMake configuration.

+ [**BUGFIX**] .github/workflows/*: Clear cache files in the workflows.

+ [**ENHANCEMENT**] scripts/workflows.sh: Enhance workflow script to include cache clearing option and update usage instructions.

+ [**DOCUMENTATION**] CONTRIBUTING.md, README.md: Improve documentation clarity and add installation/uninstallation instructions for Libmagicxx.

+ [**ENHANCEMENT**] .github/workflows/*, cmake/magicxxConfig.cmake.in, cmake/uninstall_magicxx.cmake.in, examples/*, tests/CMakeLists.txt, CMakeLists.txt, CONTRIBUTING.md, README.md: Add support for installing, uninstalling, and exporting magicxx.

+ [**ENHANCEMENT**] .github/workflows/deploy_doxygen_documentation.yml, docdocumentation/*, include/*, sources/*, CMakeLists.txt, CONTRIBUTING.md: Improve project structure.

+ [**BUGFIX**] .github/workflows/*, Containerfile: Update container images to use Fedora 41 to fix the compilation error.

+ [**BUGFIX**] scripts/launch_container.py: Always pull the latest image when updating the development container.

+ [**DOCUMENTATION**] doc/CMakeLists.txt: Update Doxygen configuration to enable index and adjust sidebar settings.

+ [**DOCUMENTATION**] CONTRIBUTING.md: Update contributing guidelines to fix rendering issues.

+ [**DOCUMENTATION**] doc/man/*, doc/CMakeLists.txt: Delete man documentation.

## [v6.0.2] - 15-04-2025

+ [**DOCUMENTATION**] inc/magic.hpp: Fix documentation formatting in magic class description.

## [v6.0.1] - 14-04-2025

+ [**BUGFIX**] doc/CMakeLists.txt, CMakePresets.json: Update documentation generation steps and add cleanup target to delete old documentation.

## [v6.0.0] - 13-04-2025

+ [**ENHANCEMENT**] .github/workflows/*, cmake/dependencies.cmake, cmake/generate_default_database_files.cmake, databases/*, scripts/install_dependencies.sh, .gitattributes, .gitignore, CMakeLists.txt, CMakePresets.json, CONTRIBUTING.md: Add default database files to speed up build & Add CMake workflow preset to generate default database files.

+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp, tests/magic_parameters_test.cpp: Add missing parameter mag_warn_max & Fix parameters' default values in the documentation.

+ [**BUGFIX**] cmake/dependencies.cmake: Fix file configure options to disable lrziplib.

+ [**ENHANCEMENT**] CMakePresets.json: Remove 'cleanFirst' option from base-config build preset.

+ [**DOCUMENTATION**] CONTRIBUTING.md: Update CONTRIBUTING.md to correct directory structure and remove TODO.md reference

+ [**ENHANCEMENT**] .scripts/launch_container.py, CONTRIBUTING.md: Merge launch_container scripts into one Python script.

+ [**BUGFIX**] .github/workflows/*, cmake/dependencies.cmake, CMakeLists.txt: Refactor CMake dependencies and update database generation commands for the Windows build.

+ [**BUGFIX**] cmake/dependencies.cmake, inc/magic.hpp, tests/*, .gitignore, CMakeLists.txt: Generate default database files & Refactor default database file handling to pass the tests.

+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp, CMakeLists.txt: Adapt default database file path based on the target operating system.

+ [**BUGFIX**] doc/CMakeLists.txt, CMakeLists.txt: Add missing magic header files to Doxygen documentation generation.

+ [**BUGFIX**] Containerfile: Fix script execution path in Containerfile.

+ [**BUGFIX**] scripts/workflows.sh, CMakeLists.txt, CMakePresets.json: Export compile commands to a location easily accessible by Clangd.

+ [**BUGFIX**] CONTRIBUTING.md: Fix path syntax for launching the container in Windows.

+ [**ENHANCEMENT**] external/file, external/googletest, tests/magic_version_test.cpp: Update file version to v5.46, googletest version to v1.16.0.

+ [**ENHANCEMENT**] .github/workflows/*, cmake/toolchains/windows-x86_64-mingw64.cmake, cmake/dependencies.cmake, scripts/install_dependencies.sh, src/magic.cpp, CMakeLists.txt, CMakePresets.json, README.md: Add support for Windows build using MinGW64.

+ [**ENHANCEMENT**] external/libgnurx, .gitsubmodules, README.md: Add libgnurx submodule for the x86_64-w64-mingw32 build.

+ [**ENHANCEMENT**] external/file, external/googletest, .gitsubmodules, CMakeLists.txt, CONTRIBUTING.md: Refactor submodule paths to use 'external/' directory for better organization.

## [v5.7.1] - 23-03-2025

+ [**BUGFIX**]  cmake/dependencies.cmake, .scripts/initialize.sh, CMakePresets.json, CONTRIBUTING.md, README.md: Move the project initialization process from CMake workflow to a bash script to fix CMake initialization workflow error.

## [v5.7.0] - 23-03-2025

+ [**BUGFIX**] .scripts/commit_release.sh: Remove unused DOXYFILE variable from commit_release script to fix the runtime error.

+ [**ENHANCEMENT**] scripts/launch_container.ps1, scripts/launch_container.sh: Enhance container launch scripts to support image updates and improve usage instructions.

+ [**DOCUMENTATION**] CONTRIBUTING.md, README.md: Update contributing guidelines and README to include project initialization step.

+ [**BUGFIX**] .scripts/install_dependencies.sh, README.md: Add missing build dependency awk.

+ [**BUGFIX**] .github/workflows/build_and_test_on_linux_using_clang.yml, .github/workflows/build_and_test_on_linux_using_gcc.yml: Fix CI workflows.

+ [**BUGFIX**] cmake/dependencies.cmake, scripts/install_dependencies.sh, CMakePresets.json, Containerfile, CONTRIBUTING.md, README.md: Bring back the install_dependencies.sh script.

+ [**BUGFIX**] Containerfile: Add missing development tools installation to Containerfile.

+ [**ENHANCEMENT**] cmake/dependencies.cmake, CMakePresets.json, Containerfile, CONTRIBUTING.md, README.md: Embed the install_dependencies.sh script to the CMake workflows && Add new CMake workflow presets install-dependencies, initialize-and-update-git-submodules and initialize.

+ [**ENHANCEMENT**] cmake/dependencies.cmake,CMakeLists.txt: Embed the magic_header target into the configure_file target..

+ [**DOCUMENTATION**]: CONTRIBUTING.md: Update CONTRIBUTING.md to clarify project structure.

+ [**ENHANCEMENT**] cmake/format_source_code.cmake, scripts/commit_release.sh, CMakeLists.txt, CMakePresets.json, CONTRIBUTING.md: Replace format script with CMake workflow for source code formatting.

+ [**ENHANCEMENT**] cmake/dependencies.cmake, CMakeLists.txt: Refactor the CMake configuration by adding custom targets for the magicxx dependencies to speed up the configuration process.

+ [**ENHANCEMENT**] CMakeLists.txt, CMakePresets.json: Rename BUILD_MAGICXX_DOCUMENTATION_ONLY build option to BUILD_MAGICXX_DOCUMENTATION.

+ [**ENHANCEMENT**] CMakeLists.txt, CMakePresets.json: Rename BUILD_AS_STATIC build option to BUILD_MAGICXX_AS_STATIC.

+ [**ENHANCEMENT**] CMakeLists.txt, CMakePresets.json, CONTRIBUTING.md, doc/CMakeLists.txt, scripts/commit_release.sh, scripts/generate_documentation.sh: Add support for building documentation from CMake and remove generate_documentation.sh script.

## [v5.6.3] - 16-03-2025

+ [**BUGFIX**] CONTRIBUTING.md: Update CONTRIBUTING.md to include CMakePresets.json and clarify CMake configuration.

## [v5.6.2] - 16-03-2025

+ [**BUGFIX**]: Update copyright year in source files to 2022-2025.

## [v5.6.1] - 16-03-2025

+ [**BUGFIX**] SECURITY.md: Update SECURITY.md to reflect support status for version 5.6.x.

## [v5.6.0] - 16-03-2025

+ [**ENHANCEMENT**] CMakeLists.txt: Remove unused source files.

+ [**ENHANCEMENT**] cmake/configure_libmagic.cmake: Enable year 2038 support in libmagic.

+ [**BUGFIX**] .github/workflows/build_and_test_on_linux_using_clang.yml, .github/workflows/build_and_test_on_linux_using_gcc.yml, .gitignore, CMakeLists.txt, CMakePresets.json, CONTRIBUTING.md, README.md, cmake/configure_libmagic.cmake, cmake/toolchains/linux-x86_64-clang.cmake, cmake/toolchains/linux-x86_64-gcc.cmake, examples/CMakeLists.txt, scripts/install_dependencies.sh, scripts/workflows.sh, tests/CMakeLists.txt: Refactor build scripts and CMake configurations & Add new toolchain files for GCC and Clang.

## [v5.5.1] - 06-03-2025

+ [**BUGFIX**] PULL_REQUEST_TEMPLATE.md: Update PULL_REQUEST_TEMPLATE.md to include title format guidelines.
+ [**BUGFIX**] .github/ISSUE_TEMPLATE/*: Add issue templates for bug reports, documentation, and feature requests.

## [v5.5.0] - 23-02-2025

+ [**ENHANCEMENT**] CONTRIBUTING.md: Update CONTRIBUTING.md to specify naming conventions.
+ [**ENHANCEMENT**] inc/utility.hpp: Add concepts for range containers and string converters to improve maintainability.
+ [**ENHANCEMENT**] src/magic.cpp: Explicitly capture the required scope variables & Remove the redundant return type.
+ [**ENHANCEMENT**] inc/magic_exception.hpp: Refactor magic_exception constructor to inherit from std::runtime_error.
+ [**ENHANCEMENT**] inc/magic_exception.hpp, src/magic.cpp: Use std::format instead of concatenating pieces manually.
+ [**ENHANCEMENT**] .clang-format: Update .clang-format to adjust line breaking penalties and compound requirement settings.
+ [**ENHANCEMENT**] .clang-format: Add SPDX license information and clang-format version to .clang-format.

## [v5.4.1] - 17-02-2025

+ [**BUGFIX**] scripts/launch_container.ps1, scripts/launch_container.sh: Remove container name specification in launch scripts to fix the container name is already used error.

## [v5.4.0] - 17-02-2025

+ [**ENHANCEMENT**] CONTRIBUTING.md, Containerfile, scripts/launch_container.ps1, scripts/launch_container.sh: Add container development environment setup and scripts.
+ [**ENHANCEMENT**] scripts/*: Improve error handling in scripts.

## [v5.3.2] - 16-02-2025

+ [**BUGFIX**] CMakeLists.txt: Fix compilation errors.

## [v5.3.1] - 15-02-2025

+ [**BUGFIX**] file: Set the version of File to 5.45 to fix compilation errors.

## [v5.3.0] - 15-02-2025

+ [**ENHANCEMENT**] CMakeLists.txt, scripts/build.sh: Add build as static option.
+ [**ENHANCEMENT**] CMakeLists.txt, scripts/build.sh: Remove rebuild the libmagic option.

## [v5.2.0] - 13-02-2025

+ [**DOCUMENTATION**] CONTRIBUTING.md: Add CONTRIBUTING.md
+ [**DOCUMENTATION**] examples/*: Add examples.
+ [**DOCUMENTATION**] PULL_REQUEST_TEMPLATE.md: Add PULL_REQUEST_TEMPLATE.md
+ [**DOCUMENTATION**] ISSUE_TEMPLATE.md: Add ISSUE_TEMPLATE.md
+ [**ENHANCEMENT**] CMakeLists.txt: Add rebuild the libmagic option.
+ [**ENHANCEMENT**] CMakeLists.txt: Build libmagic as a static library.
+ [**ENHANCEMENT**] .clang-format: Add formatting rules via clang-format.
+ [**ENHANCEMENT**] scripts/*.sh: Move scripts into the scripts directory.
+ [**ENHANCEMENT**] file: Update the version of File to 5.46
+ [**ENHANCEMENT**] googletest: Update the version of GoogleTest to 1.15.2
+ [**DOCUMENTATION**] README.md: Update documentation.
+ [**ENHANCEMENT**] inc/magic.hpp, inc/utility.hpp: Use std::views instead of std::ranges::fold_left.
+ [**DOCUMENTATION**] Doxyfile: Update documentation.
+ [**ENHANCEMENT**] SECURITY.md: Add SECURITY.md

## [v5.1.1] - 25-06-2024

+ [**DOCUMENTATION**] inc/magic.hpp: Add missing documentation for flags and parameters.
+ [**DOCUMENTATION**] CHANGELOG.md: Fix typo in CHANGELOG.md.
+ [**DOCUMENTATION**] README.md: Update README.md presentation for better readability.

## [v5.1.0] - 23-06-2024

+ [**ENHANCEMENT**] commit_release.sh: Add commit_release.sh.
+ [**ENHANCEMENT**] .github/workflows/build_and_test_on_linux.yml, deploy_doxygen_documentation.yml: Add Github Actions.
+ [**DOCUMENTATION**] doc/*, Doxyfile, generate_documentation.sh, README.md: Add Doxygen-generated documentation.
+ [**ENHANCEMENT**] build.sh: Add build script.
+ [**ENHANCEMENT**] CMakeLists.txt, install_dependencies.sh, setup.sh, README.md: Automate initialization and setup steps.

## [v5.0.0] - 06-06-2024

+ [**ENHANCEMENT**] src/magic.cpp: Rename flag_converter to flags_converter.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Make setting parameters of magic using parameter_value_map_t possible.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Make setting flags of magic using flags_container_t possible.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Make construction of magic using the flags_container_t possible.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Make opening magic using the flags_container_t possible.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Rename the flags using the snake case convention.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Rename flags_t to flags_mask_t, Flag to flags and Flags to flags_container_t.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Rename the parameters using the snake case convention.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Rename Parameter to parameters and Parameters to parameter_value_map_t.
+ [**ENHANCEMENT**] inc/file_concepts.hpp, src/magic.cpp: Use the utility::to_string function wherever possible.
+ [**ENHANCEMENT**] CMakeLists.txt, inc/utility.hpp: Add a customizable to_string function template.
+ [**ENHANCEMENT**] inc/magic_exception.hpp, inc/magic.hpp: Report the parameter value with the parameter name if the set_parameter function fails.
+ [**ENHANCEMENT**] inc/file_concepts.hpp, inc/magic.hpp: Declare to_string functions with the nodiscard attribute.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the expected_types_of_files_t with the to_string function.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the expected_file_type_t with the to_string function.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the types_of_files_t with the to_string function.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Parameters with the to_string function.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Parameter with the to_string function.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Flags with the to_string function.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Flag with the to_string function.
+ [**ENHANCEMENT**] inc/file_concepts.hpp: Replace the operator<< function for the file containers with the to_string function.
+ [**ENHANCEMENT**] inc/file_concepts.hpp, inc/magic.hpp: Add the file_concepts namespace.

## [v4.1.2] - 12-05-2024

+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the compilation error when trying to print the Flag and the Parameter using operator <<.
+ [**DOCUMENTATION**] README.md: Update the formatting of the license section.
+ [**DOCUMENTATION**] README.md, TODO.md: Separate the to do list from the README.
+ [**BUGFIX**] src/magic.cpp: Check if the magic is open before checking whether the path is empty or not.
+ [**BUGFIX**] src/magic.cpp: Fix the incorrect string conversion of Flag::None when using the operator<<.
+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the compilation error when trying to print Parameters using operator <<.
+ [**BUGFIX**] inc/magic_exception.hpp: Add the missing string header.
+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the compilation error when trying to print Flags using operator <<.

## [v4.1.1] - 07-05-2024

+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the segmentation fault error when calling a member function of a moved-from magic object.
+ [**ENHANCEMENT**] src/magic.cpp: Erase the operator bool function of the magic_private class.

## [v4.1.0] - 05-05-2024

+ [**BUGFIX**] src/magic.cpp: Fix the segmentation fault error when calling a member function of a default constructed magic.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Make the functions "check" and "compile" noexcept.
+ [**ENHANCEMENT**] inc/magic.hpp: Erase the brackets.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Add default_database_file.
+ [**ENHANCEMENT**] src/magic.cpp: Use std::format for the version string.

## [v4.0.0] - 04-05-2024

+ [**ENHANCEMENT**] CMakeLists.txt, README.md: Change the project name to Libmagicxx.
+ [**BUGFIX**] CMakeLists.txt: Make magic_INCLUDE_DIR private.

## [v3.1.1] - 28-04-2024

+ [**BUGFIX**] CMakeLists.txt: Use libc++ when the compiler is clang.
+ [**BUGFIX**] setup.sh: Install the missing libcxx-devel package for clang.

## [v3.1.0] - 23-04-2024

+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Add the missing parameters and flags.
+ [**ENHANCEMENT**] CMakeLists.txt, README.md, setup.sh: Add setup.sh.
+ [**BUGFIX**] CMakeLists.txt: Fix the SPDX-License-Identifier.
+ [**ENHANCEMENT**] .gitmodules, file: Add the Magic Number Recognition Library v5.45 as a submodule.

## [v3.0.0] - 14-04-2024

+ [**ENHANCEMENT**] COPYING, COPYING.LESSER, README.md, inc/file_concepts.hpp, inc/magic.hpp, inc/magic_exception.hpp, src/magic.cpp: Change the license to LGPL-3.0-only.

## [v2.1.0] - 07-04-2024

+ [**ENHANCEMENT**] inc/magic.hpp: Add operator<< overload for expected_file_type_t.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Add operator<< overloads for Flag, Flags, Parameter and Parameters.
+ [**ENHANCEMENT**] inc/magic.hpp, src/magic.cpp: Add get_parameters function.

## [v2.0.0] - 06-04-2024

+ [**ENHANCEMENT**] inc/file_concepts.hpp, inc/magic.hpp, src/magic.cpp: Use std::expected instead of std::optional to report the error.

## [v1.0.2] - 03-04-2024

+ [**BUGFIX**] src/magic.cpp: Fix throwing an empty_path exception when the path is not empty.

## [v1.0.1] - 11-03-2024

+ [**BUGFIX**] CMakeLists.txt: Fix typo.
+ [**DOCUMENTATION**] inc/magic.hpp: Update Doxygen comments.
+ [**DOCUMENTATION**] README.md: Fix typo.
+ [**ENHANCEMENT**] inc/magic.hpp: Use abbreviated function templates.
+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Remove the namespace alias std_fs for the Doxygen.

## [v1.0.0] - 03-01-2024

+ Initial release.
