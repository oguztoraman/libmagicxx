/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <iostream>
#include <magic.hpp>
#include <print>

using namespace Recognition;

void ExampleBasicIdentify()
{
    try {
        Magic example_magic(Magic::Flags::Mime, Magic::DEFAULT_DATABASE_FILE);
        if (!example_magic.IsValid()) {
            std::println(std::cerr, "Magic is not valid.");
            return;
        }
        auto type = example_magic.IdentifyFile(Magic::DEFAULT_DATABASE_FILE);
        std::println(
            std::cout,
            "Magic::DEFAULT_DATABASE_FILE file type: {}",
            type
        );
    } catch (const MagicException& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
}

void ExampleNoexceptIdentify()
{
    Magic example_magic;
    if (!example_magic.Open(Magic::Flags::Mime, std::nothrow)) {
        std::println(std::cerr, "Failed to open magic.");
        return;
    }
    if (example_magic.LoadDatabaseFile(std::nothrow)) {
        auto result = example_magic.IdentifyFile(
            Magic::DEFAULT_DATABASE_FILE,
            std::nothrow
        );
        std::println(
            std::cout,
            "Noexcept Magic::DEFAULT_DATABASE_FILE file type: {}",
            ToString(result)
        );
    } else {
        std::println(std::cerr, "Failed to load database.");
    }
}

void ExampleIdentifyDirectory()
{
    try {
        Magic example_magic(Magic::Flags::Mime, Magic::DEFAULT_DATABASE_FILE);
        auto  results = example_magic.IdentifyFiles(
            std::filesystem::temp_directory_path()
        );
        std::println(
            std::cout,
            "Types in the temp directory (showing up to 10):"
        );
        for (const auto& result : results | std::views::take(10)) {
            std::println(std::cout, "{}", ToString(result));
        }
    } catch (const MagicException& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
}

void ExampleCustomFlagsParameters()
{
    Magic example_magic;
    example_magic.Open({Magic::Flags::Mime, Magic::Flags::Compress});
    example_magic.LoadDatabaseFile();
    example_magic.SetParameter(Magic::Parameters::BytesMax, 2'048);
    auto value = example_magic.GetParameter(Magic::Parameters::BytesMax);
    std::println(std::cout, "BytesMax: {}", value);
    auto flags = example_magic.GetFlags();
    std::println(std::cout, "Flags: {}", ToString(flags));
}

void ExampleCheckAndCompile()
{
    if (Magic::Check()) {
        std::println(std::cout, "Database check passed.");
    } else {
        std::println(std::cerr, "Database check failed.");
    }
    if (Magic::Compile()) {
        std::println(std::cout, "Database compiled successfully.");
    } else {
        std::println(std::cerr, "Database compilation failed.");
    }
}

auto main() -> int
{
    std::println(std::cout, "--- Example 1: Basic Identify ---");
    ExampleBasicIdentify();
    std::println(std::cout, "\n--- Example 2: Noexcept Identify ---");
    ExampleNoexceptIdentify();
    std::println(std::cout, "\n--- Example 3: Directory Identify ---");
    ExampleIdentifyDirectory();
    std::println(std::cout, "\n--- Example 4: Custom Flags/Parameters ---");
    ExampleCustomFlagsParameters();
    std::println(std::cout, "\n--- Example 5: Compile and Check ---");
    ExampleCheckAndCompile();
    return 0;
}
