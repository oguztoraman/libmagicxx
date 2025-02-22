# Changelog

## Next Release

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
* [**ENHANCEMENT**] CMakeLists.txt, README.md, setup.sh: Add setup.sh.
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
