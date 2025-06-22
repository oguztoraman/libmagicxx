# Changelog

## Next Release

+ [**BUGFIX**]**;** Magic: Fix bugs identified during testing.

+ [**QUALITY**]**;** Tests & Examples: Refactor tests & examples.

+ [**DOCUMENTATION**]**;** Docs: Add section on Magic States to README.md for better understanding of magic class usability.

+ [**ENHANCEMENT**]**;** Magic: Add type_of_a_file_t, expected_type_of_a_file_t, parameter_value_t typedefs with to_string functions.

+ [**BUGFIX**]**;** Utility: Replace std::views with std::ranges::fold_left pending Clang support for std::views::join_with.

+ [**ENHANCEMENT**]**;** Magicxx: Add is_database_loaded member function.

+ [**ENHANCEMENT**]**;** Magicxx: Throw filesystem_error from identify_files(directory) if the underlying std::filesystem OS API fails.

+ [**ENHANCEMENT**]**;** Magicxx: Add filesystem_error.

+ [**QUALITY**]**;** Magicxx: Refactor identify_file(s).

+ [**ENHANCEMENT**]**;** Magicxx: API change - Throw path_is_not_directory from identify_files(directory) on the failure.

+ [**QUALITY**]**;** Magicxx: Add [[nodiscard]] attribute to several methods.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Make magic::compile static.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Make magic::check static.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Rename magic_load_error to magic_load_database_file_error for consistency and clarity in exception handling.

+ [**ENHANCEMENT**]**;** Magicxx: Enhance magic class with noexcept versions of constructors and methods for improved error handling.

+ [**DOCUMENTATION**]**;** Docs: Update pull request title template.

+ [**DOCUMENTATION**]**;** Docs: Add downloads badge to README for better visibility of library usage statistics.

+ [**DOCUMENTATION**]**;** Docs: Add missing documentation for magic_exception classes.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Rename magic_file_error to magic_identify_file_error for consistency and clarity in exception handling.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Rename magic_set_param_error to magic_set_parameter_error for consistency and clarity in exception handling.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Replace invalid_path exception with path_is_not_regular_file for better clarity in error handling.

+ [**ENHANCEMENT**]**;** Magicxx: API change - Add path_does_not_exist exception and enhance error handling for file paths.

+ [**ENHANCEMENT**]**;** Magicxx: Enhance magic class with noexcept versions of constructors and methods for improved error handling.

+ [**QUALITY**]**;** Magicxx: Move implementation of identify_files functions from header to source file.

+ [**ENHANCEMENT**]**;** Magicxx: API change – Add is_valid() to check if instance is valid for identification (open and database loaded); Make operator bool() a shortcut for is_valid().

+ [**ENHANCEMENT**]**;** Tests: Update googletest version to v1.17.0.

+ [**BUGFIX**]**;** Magicxx: Add magic_database_not_loaded exception and update identify_file functions to handle database loading state.

+ [**DOCUMENTATION**]**;** Docs: Add C++ standard badge to README.md.

+ [**QUALITY**]**;** Tests: Refactor magic_load_database_file_test with using test fixture.

+ [**ENHANCEMENT**]**;** CMake: Do not build libraries when building examples.

+ [**BUGFIX**]**;** Magicxx: Do not throw exceptions while constructing std::filesystem::recursive_directory_iterator in a noexcept function.

+ [**DOCUMENTATION**]**;** Docs: Update CONTRIBUTING.md and README.md to include CMake options and package variables.

+ [**ENHANCEMENT**]**;** CMake: Add options to build shared and static libraries independently.

+ [**ENHANCEMENT**]**;** CMake: Add option to disable installation of Libmagicxx.

## [v8.2.1] - 22-05-2025

+ [**DOCUMENTATION**]**;** Update supported versions in SECURITY.md to reflect version 8.2.x.

+ [**BUGFIX**]**;** Fix release title in workflow to use correct reference name.

## [v8.2.0] - 22-05-2025

+ [**BUGFIX**]**;** Add license directory into the CMake package.

+ [**QUALITY**]**;** Refactor installation directory variables for consistency in CMake files.

+ [**DOCUMENTATION**]**;** Add disclaimer section to README.md clarifying project status and affiliations.

+ [**BUGFIX**]**;** Include all required license files in binary distributions.

+ [**ENHANCEMENT**]**;** Remove documentation dependency from CMake package.

+ [**ENHANCEMENT**]**;** Add version information to the issue templates.

+ [**ENHANCEMENT**]**;** Add project name to the release title when deploying release packages.

+ [**DOCUMENTATION**]**;** Enhance documentation in README.md and CONTRIBUTING.md with detailed sections.

+ [**ENHANCEMENT**]**;** Add downloading instructions for Libmagicxx in README.md.

+ [**DOCUMENTATION**]**;** Clarify git operations restrictions within the container environment in CONTRIBUTING.md.

+ [**ENHANCEMENT**]**;** Add release procedure documentation.

+ [**QUALITY**]**;** Remove unnecessary steps in Doxygen documentation deployment workflow.

+ [**QUALITY**]**;** Refactor GitHub Actions workflows to standardize formatting.

+ [**ENHANCEMENT**]**;** Add code quality issue template and update contributing guidelines.

## [v8.1.1] - 21-05-2025

+ [**BUGFIX**]**;** Update permissions in deployment workflow to allow write access.

## [v8.1.0] - 21-05-2025

+ [**ENHANCEMENT**]**;** Add GitHub Actions workflow for deploying release packages.

## [v8.0.1] - 21-05-2025

+ [**BUGFIX**]**;** Clean existing build files before generating source package.

+ [**BUGFIX**]**;** Update supported versions in SECURITY.md to reflect version 8.0.x.

## [v8.0.0] - 21-05-2025

+ [**BUGFIX**]**;** Fix CMake organization.

+ [**ENHANCEMENT**]**;** Add support for generating DEB, RPM, NSIS, ZIP and source packages for easier installation and distribution.

+ [**ENHANCEMENT**]**;** Add support for component-based installation in CMake.

+ [**ENHANCEMENT**]**;** Install the license file.

+ [**ENHANCEMENT**]**;** Erase the release tag from the CMake presets.

+ [**ENHANCEMENT**]**;** Format CMake files.

+ [**ENHANCEMENT**]**;** Use the dir naming convention instead of path in the CMake files.

## [v7.0.0] - 24-04-2025

+ [**ENHANCEMENT**]**;** Update build system to support simultaneous compilation of shared and static libraries.

+ [**ENHANCEMENT**]**;** Refactor CMake configuration.

+ [**BUGFIX**]**;** Clear cache files in the workflows.

+ [**ENHANCEMENT**]**;** Enhance workflow script to include cache clearing option and update usage instructions.

+ [**DOCUMENTATION**]**;** Improve documentation clarity and add installation/uninstallation instructions for Libmagicxx.

+ [**ENHANCEMENT**]**;** Add support for installing, uninstalling, and exporting magicxx.

+ [**ENHANCEMENT**]**;** Improve project structure.

+ [**BUGFIX**]**;** Update container images to use Fedora 41 to fix the compilation error.

+ [**BUGFIX**]**;** Always pull the latest image when updating the development container.

+ [**DOCUMENTATION**]**;** Update Doxygen configuration to enable index and adjust sidebar settings.

+ [**DOCUMENTATION**]**;** Update contributing guidelines to fix rendering issues.

+ [**DOCUMENTATION**]**;** Delete man documentation.

## [v6.0.2] - 15-04-2025

+ [**DOCUMENTATION**]**;** Fix documentation formatting in magic class description.

## [v6.0.1] - 14-04-2025

+ [**BUGFIX**]**;** Update documentation generation steps and add cleanup target to delete old documentation.

## [v6.0.0] - 13-04-2025

+ [**ENHANCEMENT**]**;** Add default database files to speed up build & Add CMake workflow preset to generate default database files.

+ [**BUGFIX**]**;** Add missing parameter mag_warn_max & Fix parameters' default values in the documentation.

+ [**BUGFIX**]**;** Fix file configure options to disable lrziplib.

+ [**ENHANCEMENT**]**;** Remove 'cleanFirst' option from base-config build preset.

+ [**DOCUMENTATION**]**;** Update CONTRIBUTING.md to correct directory structure and remove TODO.md reference

+ [**ENHANCEMENT**]**;** Merge launch_container scripts into one Python script.

+ [**BUGFIX**]**;** Refactor CMake dependencies and update database generation commands for the Windows build.

+ [**BUGFIX**]**;** Generate default database files & Refactor default database file handling to pass the tests.

+ [**ENHANCEMENT**]**;** Adapt default database file path based on the target operating system.

+ [**BUGFIX**]**;** Add missing magic header files to Doxygen documentation generation.

+ [**BUGFIX**]**;** Fix script execution path in Containerfile.

+ [**BUGFIX**]**;** Export compile commands to a location easily accessible by Clangd.

+ [**BUGFIX**]**;** Fix path syntax for launching the container in Windows.

+ [**ENHANCEMENT**]**;** Update file version to v5.46, googletest version to v1.16.0.

+ [**ENHANCEMENT**]**;** Add support for Windows build using MinGW64.

+ [**ENHANCEMENT**]**;** Add libgnurx submodule for the x86_64-w64-mingw32 build.

+ [**ENHANCEMENT**]**;** Refactor submodule paths to use 'external/' directory for better organization.

## [v5.7.1] - 23-03-2025

+ [**BUGFIX**]**;** Move the project initialization process from CMake workflow to a bash script to fix CMake initialization workflow error.

## [v5.7.0] - 23-03-2025

+ [**BUGFIX**]**;** Remove unused DOXYFILE variable from commit_release script to fix the runtime error.

+ [**ENHANCEMENT**]**;** Enhance container launch scripts to support image updates and improve usage instructions.

+ [**DOCUMENTATION**]**;** Update contributing guidelines and README to include project initialization step.

+ [**BUGFIX**]**;** Add missing build dependency awk.

+ [**BUGFIX**]**;** Fix CI workflows.

+ [**BUGFIX**]**;** Bring back the install_dependencies.sh script.

+ [**BUGFIX**]**;** Add missing development tools installation to Containerfile.

+ [**ENHANCEMENT**]**;** Embed the install_dependencies.sh script to the CMake workflows && Add new CMake workflow presets install-dependencies, initialize-and-update-git-submodules and initialize.

+ [**ENHANCEMENT**]**;** Embed the magic_header target into the configure_file target..

+ [**DOCUMENTATION**]**;** Update CONTRIBUTING.md to clarify project structure.

+ [**ENHANCEMENT**]**;** Replace format script with CMake workflow for source code formatting.

+ [**ENHANCEMENT**]**;** Refactor the CMake configuration by adding custom targets for the magicxx dependencies to speed up the configuration process.

+ [**ENHANCEMENT**]**;** Rename BUILD_MAGICXX_DOCUMENTATION_ONLY build option to BUILD_MAGICXX_DOCUMENTATION.

+ [**ENHANCEMENT**]**;** Rename BUILD_AS_STATIC build option to BUILD_MAGICXX_AS_STATIC.

+ [**ENHANCEMENT**]**;** Add support for building documentation from CMake and remove generate_documentation.sh script.

## [v5.6.3] - 16-03-2025

+ [**BUGFIX**]**;** Update CONTRIBUTING.md to include CMakePresets.json and clarify CMake configuration.

## [v5.6.2] - 16-03-2025

+ [**BUGFIX**]**;**: Update copyright year in source files to 2022-2025.

## [v5.6.1] - 16-03-2025

+ [**BUGFIX**]**;** Update SECURITY.md to reflect support status for version 5.6.x.

## [v5.6.0] - 16-03-2025

+ [**ENHANCEMENT**]**;** Remove unused source files.

+ [**ENHANCEMENT**]**;** Enable year 2038 support in libmagic.

+ [**BUGFIX**]**;** Refactor build scripts and CMake configurations & Add new toolchain files for GCC and Clang.

## [v5.5.1] - 06-03-2025

+ [**BUGFIX**]**;** Update PULL_REQUEST_TEMPLATE.md to include title format guidelines.

+ [**BUGFIX**]**;** Add issue templates for bug reports, documentation, and feature requests.

## [v5.5.0] - 23-02-2025

+ [**ENHANCEMENT**]**;** Update CONTRIBUTING.md to specify naming conventions.

+ [**ENHANCEMENT**]**;** Add concepts for range containers and string converters to improve maintainability.

+ [**ENHANCEMENT**]**;** Explicitly capture the required scope variables & Remove the redundant return type.

+ [**ENHANCEMENT**]**;** Refactor magic_exception constructor to inherit from std::runtime_error.

+ [**ENHANCEMENT**]**;** Use std::format instead of concatenating pieces manually.

+ [**ENHANCEMENT**]**;** Update .clang-format to adjust line breaking penalties and compound requirement settings.

+ [**ENHANCEMENT**]**;** Add SPDX license information and clang-format version to .clang-format.

## [v5.4.1] - 17-02-2025

+ [**BUGFIX**]**;** Remove container name specification in launch scripts to fix the container name is already used error.

## [v5.4.0] - 17-02-2025

+ [**ENHANCEMENT**]**;** Add container development environment setup and scripts.

+ [**ENHANCEMENT**]**;** Improve error handling in scripts.

## [v5.3.2] - 16-02-2025

+ [**BUGFIX**]**;** Fix compilation errors.

## [v5.3.1] - 15-02-2025

+ [**BUGFIX**]**;** Set the version of File to 5.45 to fix compilation errors.

## [v5.3.0] - 15-02-2025

+ [**ENHANCEMENT**]**;** Add build as static option.

+ [**ENHANCEMENT**]**;** Remove rebuild the libmagic option.

## [v5.2.0] - 13-02-2025

+ [**DOCUMENTATION**]**;** Add CONTRIBUTING.md

+ [**DOCUMENTATION**]**;** Add examples.

+ [**DOCUMENTATION**]**;** Add PULL_REQUEST_TEMPLATE.md

+ [**DOCUMENTATION**]**;** Add ISSUE_TEMPLATE.md

+ [**ENHANCEMENT**]**;** Add rebuild the libmagic option.

+ [**ENHANCEMENT**]**;** Build libmagic as a static library.

+ [**ENHANCEMENT**]**;** Add formatting rules via clang-format.

+ [**ENHANCEMENT**]**;** Move scripts into the scripts directory.

+ [**ENHANCEMENT**]**;** Update the version of File to 5.46

+ [**ENHANCEMENT**]**;** Update the version of GoogleTest to 1.15.2

+ [**DOCUMENTATION**]**;** Update documentation.

+ [**ENHANCEMENT**]**;** Use std::views instead of std::ranges::fold_left.

+ [**DOCUMENTATION**]**;** Update documentation.

+ [**ENHANCEMENT**]**;** Add SECURITY.md

## [v5.1.1] - 25-06-2024

+ [**DOCUMENTATION**]**;** Add missing documentation for flags and parameters.

+ [**DOCUMENTATION**]**;** Fix typo in CHANGELOG.md.

+ [**DOCUMENTATION**]**;** Update README.md presentation for better readability.

## [v5.1.0] - 23-06-2024

+ [**ENHANCEMENT**]**;** Add commit_release.sh.

+ [**ENHANCEMENT**]**;** Add Github Actions.

+ [**DOCUMENTATION**]**;** Add Doxygen-generated documentation.

+ [**ENHANCEMENT**]**;** Add build script.

+ [**ENHANCEMENT**]**;** Automate initialization and setup steps.

## [v5.0.0] - 06-06-2024

+ [**ENHANCEMENT**]**;** Rename flag_converter to flags_converter.

+ [**ENHANCEMENT**]**;** Make setting parameters of magic using parameter_value_map_t possible.

+ [**ENHANCEMENT**]**;** Make setting flags of magic using flags_container_t possible.

+ [**ENHANCEMENT**]**;** Make construction of magic using the flags_container_t possible.

+ [**ENHANCEMENT**]**;** Make opening magic using the flags_container_t possible.

+ [**ENHANCEMENT**]**;** Rename the flags using the snake case convention.

+ [**ENHANCEMENT**]**;** Rename flags_t to flags_mask_t, Flag to flags and Flags to flags_container_t.

+ [**ENHANCEMENT**]**;** Rename the parameters using the snake case convention.

+ [**ENHANCEMENT**]**;** Rename Parameter to parameters and Parameters to parameter_value_map_t.

+ [**ENHANCEMENT**]**;** Use the utility::to_string function wherever possible.

+ [**ENHANCEMENT**]**;** Add a customizable to_string function template.

+ [**ENHANCEMENT**]**;** Report the parameter value with the parameter name if the set_parameter function fails.

+ [**ENHANCEMENT**]**;** Declare to_string functions with the nodiscard attribute.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the expected_types_of_files_t with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the expected_file_type_t with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the types_of_files_t with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the Parameters with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the Parameter with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the Flags with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the Flag with the to_string function.

+ [**ENHANCEMENT**]**;** Replace the operator<< function for the file containers with the to_string function.

+ [**ENHANCEMENT**]**;** Add the file_concepts namespace.

## [v4.1.2] - 12-05-2024

+ [**BUGFIX**]**;** Fix the compilation error when trying to print the Flag and the Parameter using operator <<.

+ [**DOCUMENTATION**]**;** Update the formatting of the license section.

+ [**DOCUMENTATION**]**;** Separate the to do list from the README.

+ [**BUGFIX**]**;** Check if the magic is open before checking whether the path is empty or not.

+ [**BUGFIX**]**;** Fix the incorrect string conversion of Flag::None when using the operator<<.

+ [**BUGFIX**]**;** Fix the compilation error when trying to print Parameters using operator <<.

+ [**BUGFIX**]**;** Add the missing string header.

+ [**BUGFIX**]**;** Fix the compilation error when trying to print Flags using operator <<.

## [v4.1.1] - 07-05-2024

+ [**BUGFIX**]**;** Fix the segmentation fault error when calling a member function of a moved-from magic object.

+ [**ENHANCEMENT**]**;** Erase the operator bool function of the magic_private class.

## [v4.1.0] - 05-05-2024

+ [**BUGFIX**]**;** Fix the segmentation fault error when calling a member function of a default constructed magic.

+ [**ENHANCEMENT**]**;** Make the functions "check" and "compile" noexcept.

+ [**ENHANCEMENT**]**;** Erase the brackets.

+ [**ENHANCEMENT**]**;** Add default_database_file.

+ [**ENHANCEMENT**]**;** Use std::format for the version string.

## [v4.0.0] - 04-05-2024

+ [**ENHANCEMENT**]**;** Change the project name to Libmagicxx.

+ [**BUGFIX**]**;** Make magic_INCLUDE_DIR private.

## [v3.1.1] - 28-04-2024

+ [**BUGFIX**]**;** Use libc++ when the compiler is clang.

+ [**BUGFIX**]**;** Install the missing libcxx-devel package for clang.

## [v3.1.0] - 23-04-2024

+ [**ENHANCEMENT**]**;** Add the missing parameters and flags.

+ [**ENHANCEMENT**]**;** Add setup.sh.

+ [**BUGFIX**]**;** Fix the SPDX-License-Identifier.

+ [**ENHANCEMENT**]**;** Add the Magic Number Recognition Library v5.45 as a submodule.

## [v3.0.0] - 14-04-2024

+ [**ENHANCEMENT**]**;** Change the license to LGPL-3.0-only.

## [v2.1.0] - 07-04-2024

+ [**ENHANCEMENT**]**;** Add operator<< overload for expected_file_type_t.

+ [**ENHANCEMENT**]**;** Add operator<< overloads for Flag, Flags, Parameter and Parameters.

+ [**ENHANCEMENT**]**;** Add get_parameters function.

## [v2.0.0] - 06-04-2024

+ [**ENHANCEMENT**]**;** Use std::expected instead of std::optional to report the error.

## [v1.0.2] - 03-04-2024

+ [**BUGFIX**]**;** Fix throwing an empty_path exception when the path is not empty.

## [v1.0.1] - 11-03-2024

+ [**BUGFIX**]**;** Fix typo.

+ [**DOCUMENTATION**]**;** Update Doxygen comments.

+ [**DOCUMENTATION**]**;** Fix typo.

+ [**ENHANCEMENT**]**;** Use abbreviated function templates.

+ [**BUGFIX**]**;** Remove the namespace alias std_fs for the Doxygen.

## [v1.0.0] - 03-01-2024

+ Initial release.
