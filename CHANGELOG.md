# Changelog

## Next Release

+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the types_of_files_t with the to_string function.
+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Parameters with the to_string function.
+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Parameter with the to_string function.
+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Flags with the to_string function.
+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Replace the operator<< function for the Flag with the to_string function.
+ [**FEAUTURE**] inc/file_concepts.hpp: Replace the operator<< function for the file containers with the to_string function.
+ [**IMPROVEMENT**] inc/file_concepts.hpp, inc/magic.hpp: Add the file_concepts namespace.

## [v4.1.2] - 12-05-2024

+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the compilation error when trying to print the Flag and the Parameter using operator <<.
+ [**IMPROVEMENT**] README.md: Update the formatting of the license section.
+ [**IMPROVEMENT**] README.md, TODO.md: Separate the to do list from the README.
+ [**BUGFIX**] src/magic.cpp: Check if the magic is open before checking whether the path is empty or not.
+ [**BUGFIX**] src/magic.cpp: Fix the incorrect string conversion of Flag::None when using the operator<<.
+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the compilation error when trying to print Parameters using operator <<.
+ [**BUGFIX**] inc/magic_exception.hpp: Add the missing string header.
+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the compilation error when trying to print Flags using operator <<.

## [v4.1.1] - 07-05-2024

+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Fix the segmentation fault error when calling a member function of a moved-from magic object.
+ [**IMPROVEMENT**] src/magic.cpp: Erase the operator bool function of the magic_private class.

## [v4.1.0] - 05-05-2024

+ [**BUGFIX**] src/magic.cpp: Fix the segmentation fault error when calling a member function of a default constructed magic.
+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Make the functions "check" and "compile" noexcept.
+ [**IMPROVEMENT**] inc/magic.hpp: Erase the brackets.
+ [**FEAUTURE**] inc/magic.hpp, src/magic.cpp: Add default_database_file.
+ [**IMPROVEMENT**] src/magic.cpp: Use std::format for the version string.

## [v4.0.0] - 04-05-2024

+ [**IMPROVEMENT**] CMakeLists.txt, README.md: Change the project name to Libmagicxx.
+ [**BUGFIX**] CMakeLists.txt: Make magic_INCLUDE_DIR private.

## [v3.1.1] - 28-04-2024

+ [**BUGFIX**] CMakeLists.txt: Use libc++ when the compiler is clang.
+ [**BUGFIX**] setup.sh: Install the missing libcxx-devel package for clang.

## [v3.1.0] - 23-04-2024

+ [**FEATURE**] inc/magic.hpp, src/magic.cpp: Add the missing parameters and flags.
* [**FEATURE**] CMakeLists.txt, README.md, setup.sh: Add setup.sh.
+ [**BUGFIX**] CMakeLists.txt: Fix the SPDX-License-Identifier.
+ [**FEATURE**] .gitmodules, file: Add the Magic Number Recognition Library v5.45 as a submodule.

## [v3.0.0] - 14-04-2024

+ [**IMPROVEMENT**] COPYING, COPYING.LESSER, README.md, inc/file_concepts.hpp, inc/magic.hpp, inc/magic_exception.hpp, src/magic.cpp: Change the license to LGPL-3.0-only.

## [v2.1.0] - 07-04-2024

+ [**FEATURE**] inc/magic.hpp: Add operator<< overload for expected_file_type_t.
+ [**FEATURE**] inc/magic.hpp, src/magic.cpp: Add operator<< overloads for Flag, Flags, Parameter and Parameters.
+ [**FEATURE**] inc/magic.hpp, src/magic.cpp: Add get_parameters function.

## [v2.0.0] - 06-04-2024

+ [**FEATURE**] inc/file_concepts.hpp, inc/magic.hpp, src/magic.cpp: Use std::expected instead of std::optional to report the error.

## [v1.0.2] - 03-04-2024

+ [**BUGFIX**] src/magic.cpp: Fix throwing an empty_path exception when the path is not empty.

## [v1.0.1] - 11-03-2024

+ [**BUGFIX**] CMakeLists.txt: Fix typo.
+ [**BUGFIX**] inc/magic.hpp: Update Doxygen comments.
+ [**BUGFIX**] README.md: Fix typo.
+ [**IMPROVEMENT**] inc/magic.hpp: Use abbreviated function templates.
+ [**BUGFIX**] inc/magic.hpp, src/magic.cpp: Remove the namespace alias std_fs for the Doxygen.

## [v1.0.0] - 03-01-2024

+ Initial release.
