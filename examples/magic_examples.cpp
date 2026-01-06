/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_examples.cpp
 * @brief Comprehensive examples demonstrating libmagicxx usage.
 *
 * This file contains practical examples showing how to use the libmagicxx
 * library for file type identification. Each example demonstrates a different
 * aspect of the library's functionality.
 *
 * @section examples_overview Examples Overview
 *
 * | Example | Description |
 * |---------|-------------|
 * | ExampleBasicIdentify() | Basic file identification with exception handling |
 * | ExampleNoexceptIdentify() | File identification using noexcept API |
 * | ExampleIdentifyDirectory() | Batch identification of files in a directory |
 * | ExampleCustomFlagsParameters() | Configuring flags and parameters |
 * | ExampleCheckAndCompile() | Database validation and compilation |
 * | ExampleProgressTracking() | Progress tracking for batch operations |
 * | ExampleContainerIdentify() | Identify specific files from a container |
 * | ExampleLifecycleManagement() | Manual lifecycle and state queries |
 * | ExampleVersionAndAllParameters() | Get version and all parameters |
 *
 * @section examples_building Building the Examples
 *
 * Build the examples using CMake with the examples preset:
 * @code{.sh}
 * ./scripts/workflows.sh -p linux-x86_64-clang-examples
 * @endcode
 *
 * @section examples_running Running the Examples
 *
 * After building, run the examples executable:
 * @code{.sh}
 * ./build/examples/magic_examples
 * @endcode
 *
 * @see Recognition::Magic
 * @see Recognition::MagicException
 */

#include <iostream>
#include <magic.hpp>
#include <print>

using namespace Recognition;

/**
 * @brief Example 1: Basic file identification with exception handling.
 *
 * This example demonstrates the most common usage pattern:
 * - Creating a Magic instance with MIME output format
 * - Validating the instance before use
 * - Identifying a file's type
 * - Handling exceptions properly
 *
 * @note The Magic constructor automatically opens and loads the database.
 *
 * @see Magic::Magic(FlagsMaskT, const std::filesystem::path&)
 * @see Magic::IdentifyFile()
 * @see MagicException
 */
void ExampleBasicIdentify()
{
    try {
        Magic example_magic{Magic::Flags::Mime, Magic::DEFAULT_DATABASE_FILE};
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

/**
 * @brief Example 2: File identification using the noexcept API.
 *
 * This example demonstrates the non-throwing API pattern:
 * - Manual Open() and LoadDatabaseFile() with std::nothrow
 * - Checking return values instead of catching exceptions
 * - Using IdentifyFile() with std::nothrow returning std::expected
 *
 * The noexcept API is useful when:
 * - Exceptions are disabled or expensive in your environment
 * - You prefer explicit error checking over exception handling
 * - Performance is critical and you want to avoid exception overhead
 *
 * @see Magic::Open(FlagsMaskT, const std::nothrow_t&)
 * @see Magic::LoadDatabaseFile(const std::nothrow_t&, const std::filesystem::path&)
 * @see Magic::IdentifyFile(const std::filesystem::path&, const std::nothrow_t&)
 */
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

/**
 * @brief Example 3: Batch identification of files in a directory.
 *
 * This example demonstrates directory scanning:
 * - Identifying all files in a directory recursively
 * - Processing results as a map of paths to types
 * - Using std::views for result filtering
 *
 * The IdentifyFiles() method recursively scans the directory and
 * returns a map from file paths to their identified types.
 *
 * @note For large directories, consider using the progress tracker
 *       overload to monitor identification progress.
 *
 * @see Magic::IdentifyFiles(const std::filesystem::path&, std::filesystem::directory_options)
 * @see Magic::FileTypeMapT
 */
void ExampleIdentifyDirectory()
{
    try {
        Magic example_magic{Magic::Flags::Mime, Magic::DEFAULT_DATABASE_FILE};
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

/**
 * @brief Example 4: Configuring flags and parameters.
 *
 * This example demonstrates advanced configuration:
 * - Opening with multiple flags using a container
 * - Setting parameters to tune identification behavior
 * - Retrieving current flag and parameter values
 *
 * ### Flags Used
 * - **Mime**: Return MIME type with encoding (e.g., "text/plain; charset=utf-8")
 * - **Compress**: Decompress files before identification
 *
 * ### Parameters Modified
 * - **BytesMax**: Maximum bytes to read from file (set to 2KB for demo)
 *
 * @see Magic::Flags
 * @see Magic::Parameters
 * @see Magic::SetParameter()
 * @see Magic::GetParameter()
 */
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

/**
 * @brief Example 5: Database validation and compilation.
 *
 * This example demonstrates database management:
 * - Checking database validity with Magic::Check()
 * - Compiling a magic database with Magic::Compile()
 *
 * These are static methods that don't require a Magic instance.
 * They operate on the default database file unless a path is specified.
 *
 * ### Use Cases
 * - **Check()**: Validate database integrity before deployment
 * - **Compile()**: Create .mgc compiled database from source magic file
 *
 * @note Compile() creates a new .mgc file alongside the source file.
 *
 * @see Magic::Check()
 * @see Magic::Compile()
 * @see Magic::DEFAULT_DATABASE_FILE
 */
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

/**
 * @brief Example 6: Progress tracking for batch operations.
 *
 * This example demonstrates progress monitoring:
 * - Creating a shared progress tracker
 * - Passing it to IdentifyFiles() for monitoring
 * - Checking completion status and percentage
 *
 * Progress tracking is useful for:
 * - Large directories with many files
 * - Providing user feedback during long operations
 * - Implementing progress bars or status updates
 *
 * @see Utility::MakeSharedProgressTracker()
 * @see Utility::ProgressTracker
 * @see Magic::IdentifyFiles(directory, progress_tracker)
 */
void ExampleProgressTracking()
{
    try {
        Magic example_magic{Magic::Flags::Mime};
        auto  tracker = Utility::MakeSharedProgressTracker();
        auto  results = example_magic.IdentifyFiles(
            std::filesystem::temp_directory_path(),
            tracker
        );
        std::println(
            std::cout,
            "Identified {} files. Completed: {}",
            results.size(),
            tracker->IsCompleted() ? "yes" : "no"
        );
        std::println(
            std::cout,
            "Final progress: {}",
            tracker->GetCompletionPercentage().ToString()
        );
    } catch (const MagicException& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
}

/**
 * @brief Example 7: Identify specific files from a container.
 *
 * This example demonstrates batch file identification:
 * - Creating a vector of specific file paths
 * - Identifying all files in the container at once
 * - Processing results as a map
 *
 * Use this pattern when you have a known list of files
 * rather than scanning a directory.
 *
 * @see Magic::IdentifyFiles(container)
 * @see Utility::FileContainer
 */
void ExampleContainerIdentify()
{
    try {
        Magic example_magic{Magic::Flags::MimeType};
        std::vector<std::filesystem::path> files = {
            Magic::DEFAULT_DATABASE_FILE
        };
        auto results = example_magic.IdentifyFiles(files);
        std::println(std::cout, "Container identification results:");
        for (const auto& result : results) {
            std::println(std::cout, "{}", ToString(result));
        }
    } catch (const MagicException& e) {
        std::println(std::cerr, "Error: {}", e.what());
    }
}

/**
 * @brief Example 8: Manual lifecycle and state queries.
 *
 * This example demonstrates lifecycle management:
 * - Default construction (Closed state)
 * - State queries: IsOpen(), IsDatabaseLoaded(), IsValid()
 * - Manual Open() and LoadDatabaseFile()
 * - SetFlags() to change flags after construction
 * - Close() to release resources
 *
 * ### State Transitions
 * ```
 * Closed -> Open() -> Opened -> LoadDatabaseFile() -> Valid -> Close() -> Closed
 * ```
 *
 * @see Magic::IsOpen()
 * @see Magic::IsDatabaseLoaded()
 * @see Magic::IsValid()
 * @see Magic::Close()
 * @see Magic::SetFlags()
 */
void ExampleLifecycleManagement()
{
    Magic example_magic;
    std::println(
        std::cout,
        "After default construction: IsOpen={}, IsDatabaseLoaded={}, IsValid={}",
        example_magic.IsOpen(),
        example_magic.IsDatabaseLoaded(),
        example_magic.IsValid()
    );

    example_magic.Open(Magic::Flags::None);
    std::println(
        std::cout,
        "After Open(): IsOpen={}, IsDatabaseLoaded={}, IsValid={}",
        example_magic.IsOpen(),
        example_magic.IsDatabaseLoaded(),
        example_magic.IsValid()
    );

    example_magic.LoadDatabaseFile();
    std::println(
        std::cout,
        "After LoadDatabaseFile(): IsOpen={}, IsDatabaseLoaded={}, IsValid={}",
        example_magic.IsOpen(),
        example_magic.IsDatabaseLoaded(),
        example_magic.IsValid()
    );

    example_magic.SetFlags(Magic::Flags::Mime);
    auto new_flags = example_magic.GetFlags();
    std::println(std::cout, "After SetFlags(): {}", ToString(new_flags));

    example_magic.Close();
    std::println(
        std::cout,
        "After Close(): IsOpen={}, IsDatabaseLoaded={}, IsValid={}",
        example_magic.IsOpen(),
        example_magic.IsDatabaseLoaded(),
        example_magic.IsValid()
    );
}

/**
 * @brief Example 9: Get version and all parameters.
 *
 * This example demonstrates:
 * - Getting the libmagic version with GetVersion()
 * - Getting all parameters at once with GetParameters()
 * - Setting multiple parameters with SetParameters()
 *
 * @see Magic::GetVersion()
 * @see Magic::GetParameters()
 * @see Magic::SetParameters()
 */
void ExampleVersionAndAllParameters()
{
    std::println(std::cout, "Libmagic version: {}", Magic::GetVersion());

    Magic example_magic{Magic::Flags::Mime};
    auto  all_params = example_magic.GetParameters();
    std::println(std::cout, "All parameters: {}", ToString(all_params));

    example_magic.SetParameters({
        {Magic::Parameters::BytesMax,  1'024'000},
        {Magic::Parameters::RegexMax,  4'096    }
    });
    auto updated_params = example_magic.GetParameters();
    std::println(
        std::cout,
        "After SetParameters(): {}",
        ToString(updated_params)
    );
}

/**
 * @brief Main entry point for the examples program.
 *
 * Runs all example functions sequentially, demonstrating
 * various features of the libmagicxx library.
 *
 * @return 0 on successful completion.
 */
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
    std::println(std::cout, "\n--- Example 6: Progress Tracking ---");
    ExampleProgressTracking();
    std::println(std::cout, "\n--- Example 7: Container Identify ---");
    ExampleContainerIdentify();
    std::println(std::cout, "\n--- Example 8: Lifecycle Management ---");
    ExampleLifecycleManagement();
    std::println(std::cout, "\n--- Example 9: Version and All Parameters ---");
    ExampleVersionAndAllParameters();
    return 0;
}
