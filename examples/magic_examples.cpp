/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <iostream>
#include <magic.hpp>
#include <print>

using namespace recognition;

void example_basic_identify()
{
    try {
        magic example_magic(magic::flags::mime, magic::default_database_file);
        if (!example_magic.is_valid()) {
            std::println(std::cerr, "Magic is not valid.");
            return;
        }
        auto type = example_magic.identify_file(magic::default_database_file);
        std::println(
            std::cout,
            "magic::default_database_file file type: {}",
            type
        );
    } catch (const magic_exception& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
}

void example_noexcept_identify()
{
    magic example_magic;
    if (!example_magic.open(magic::flags::mime, std::nothrow)) {
        std::println(std::cerr, "Failed to open magic.");
        return;
    }
    if (example_magic.load_database_file(std::nothrow)) {
        auto result = example_magic.identify_file(
            magic::default_database_file,
            std::nothrow
        );
        std::println(
            std::cout,
            "Noexcept magic::default_database_file file type: {}",
            to_string(result)
        );
    } else {
        std::println(std::cerr, "Failed to load database.");
    }
}

void example_identify_directory()
{
    try {
        magic example_magic(magic::flags::mime, magic::default_database_file);
        auto  results = example_magic.identify_files(
            std::filesystem::temp_directory_path()
        );
        std::println(
            std::cout,
            "Types in the temp directory (showing up to 10):"
        );
        for (const auto& result : results | std::views::take(10)) {
            std::println(std::cout, "{}", to_string(result));
        }
    } catch (const magic_exception& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
}

void example_custom_flags_parameters()
{
    magic example_magic;
    example_magic.open({magic::flags::mime, magic::flags::compress});
    example_magic.load_database_file();
    example_magic.set_parameter(magic::parameters::bytes_max, 2'048);
    auto value = example_magic.get_parameter(magic::parameters::bytes_max);
    std::println(std::cout, "Bytes_max: {}", value);
    auto flags = example_magic.get_flags();
    std::println(std::cout, "Flags: {}", to_string(flags));
}

void example_check_and_compile()
{
    if (magic::check()) {
        std::println(std::cout, "Database check passed.");
    } else {
        std::println(std::cerr, "Database check failed.");
    }
    if (magic::compile()) {
        std::println(std::cout, "Database compiled successfully.");
    } else {
        std::println(std::cerr, "Database compilation failed.");
    }
}

auto main() -> int
{
    std::println(std::cout, "--- Example 1: Basic Identify ---");
    example_basic_identify();
    std::println(std::cout, "\n--- Example 2: Noexcept Identify ---");
    example_noexcept_identify();
    std::println(std::cout, "\n--- Example 3: Directory Identify ---");
    example_identify_directory();
    std::println(std::cout, "\n--- Example 4: Custom Flags/Parameters ---");
    example_custom_flags_parameters();
    std::println(std::cout, "\n--- Example 5: Compile and Check ---");
    example_check_and_compile();
    return 0;
}
