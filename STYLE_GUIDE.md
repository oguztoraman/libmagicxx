# C++ Style Guide for Libmagicxx

## Overview

This document defines the coding style and naming conventions for the libmagicxx project. All contributors must follow these guidelines to ensure consistency and maintainability across the codebase.

## Table of Contents

+ [Naming Conventions](#naming-conventions)

+ [Code Formatting](#code-formatting)

+ [Documentation](#documentation)

## Naming Conventions

### Classes

Use **PascalCase** for all class names.

```cpp
class Magic { };
class ProgressTracker { };
class MagicException : public std::runtime_error { };
```

### Enumerations

Use **PascalCase** for both enum types and their values.

```cpp
enum class Parameters : std::size_t {
    IndirMax     = 0uz,
    NameMax      = 1uz,
    ElfPhnumMax  = 2uz,
    ElfShnumMax  = 3uz
};

enum Flags : unsigned long long {
    None            = 0ULL,
    Debug           = 1ULL << 0,
    Symlink         = 1ULL << 1,
    Compress        = 1ULL << 2,
    MimeType        = 1ULL << 4,
    ContinueSearch  = 1ULL << 5
};
```

### Variables

Use **snake_case** for all variables.

```cpp
int file_count = 0;
std::string database_path;
const auto& current_file = files[index];
```

### Member Variables

Use **snake_case** with an **`m_` prefix** for all non-static member variables.

```cpp
class FileHandler {
private:
    std::string m_filename;
    int m_file_descriptor;
    bool m_is_open;
    std::unique_ptr<Magic> m_magic_instance;
};
```

### Functions and Methods

Use **PascalCase** for all functions and methods, including member functions.

```cpp
void LoadDatabaseFile(const std::filesystem::path& database_file);
bool IsValid() const noexcept;
std::string IdentifyFile(const std::filesystem::path& file_path);
auto GetFileType() -> expected_file_type_t;
```

### Static Member Functions

Static member functions follow the same **PascalCase** convention and must be called with the class name qualifier for clarity.

```cpp
class Utility {
public:
    static void ValidatePath(const std::filesystem::path& path);
    static constexpr bool IsValidFlag(unsigned long long flag);
};

/* Usage */
Utility::ValidatePath(file_path);
Utility::IsValidFlag(Flags::Debug);
```

### Test Names

Use **PascalCase** ending with **`Test`** for test fixture/group names and **snake_case** for individual test case names (GoogleTest `TEST_F` and `TEST` macros).

Test files should use **snake_case** and end with **`_test.cpp`**.

```cpp
/* Test fixture - PascalCase ending with Test */
struct MagicOpenCloseTest : testing::Test {
    Magic m_magic;
};

/* Test cases - snake_case */
TEST_F(MagicOpenCloseTest, closed_magic_open_with_flags_mask)
{
    EXPECT_TRUE(m_magic.Open(Magic::Flags::Mime, std::nothrow));
}

TEST_F(MagicOpenCloseTest, close_after_open)
{
    m_magic.Open(Magic::Flags::Mime);
    m_magic.Close();
    EXPECT_FALSE(m_magic.IsOpen());
}

TEST(MagicVersionTest, get_version_returns_non_empty_string)
{
    auto version = Magic::GetVersion();
    EXPECT_FALSE(version.empty());
}
```

**Test file naming:**
```
tests/magic_open_close_test.cpp
tests/magic_flags_test.cpp
tests/magic_version_test.cpp
```

### Namespaces

Use **PascalCase** for namespace names.

```cpp
namespace Recognition {
namespace Utility {
    class ProgressTracker { };
}  /* namespace Utility */
}  /* namespace Recognition */
```

### Template Parameters

Template parameters use **PascalCase** with specific suffixes:

+ **Type parameters**: End with **`T`**

+ **Non-type parameters**: End with **`V`**

```cpp
/* Type template parameters */
template <typename ValueT, typename ContainerT, typename AllocatorT>
class CustomVector { };

template <typename PathT>
void ProcessFile(const PathT& path);

/* Non-type template parameters */
template <std::size_t SizeV, std::size_t AlignmentV>
class FixedBuffer { };

template <int MaxDepthV>
constexpr auto ComputeDepth();

/* Mixed */
template <typename ElementT, std::size_t CapacityV>
class StaticArray { };
```

### Type Aliases (Typedefs)

Use **PascalCase** ending with **`T`** for all type aliases and typedefs.

```cpp
using FlagsMaskT = unsigned long long;
using ProgressTrackerT = std::shared_ptr<ProgressTracker>;
using FileTypeT = std::string;
using ExpectedFileTypeT = std::expected<FileTypeT, std::string>;
using FileTypeMapT = std::map<std::filesystem::path, FileTypeT>;
using FileTypeEntryT = FileTypeMapT::value_type;
using ParameterValueMapT = std::map<Parameters, std::size_t>;
```

### Constants and Macros

Use **SCREAMING_SNAKE_CASE** for both preprocessor macros and compile-time constants.

```cpp
/* Macros */
#define MAGIC_VERSION_MAJOR 9
#define MAGIC_API_EXPORT __attribute__((visibility("default")))
#define MAGIC_DEPRECATED [[deprecated]]

/* Constants */
static constexpr std::size_t MAX_FILE_SIZE = 1024uz * 1024uz;
static constexpr int DEFAULT_BUFFER_SIZE = 4096;
inline constexpr std::string_view DEFAULT_DATABASE_PATH = "/usr/share/misc/magic.mgc";
```

### Files and Directories

Use **snake_case** for all source code files and directory names.

```
include/magicxx/magic.hpp
include/magicxx/magic_exception.hpp
include/magicxx/progress_tracker.hpp
sources/magic.cpp
tests/magic_open_close_test.cpp
scripts/workflows.sh
```

### Documentation Files

Use **SCREAMING_SNAKE_CASE** for all documentation markdown files at the root level.

```
README.md
CONTRIBUTING.md
CHANGELOG.md
STYLE_GUIDE.md
AGENTS.md
CODE_OF_CONDUCT.md
SECURITY.md
RELEASE_PROCEDURE.md
PULL_REQUEST_TEMPLATE.md
```

## Code Formatting

### Automatic Formatting

All code must be formatted using the project's `.clang-format` configuration. Run the formatter before committing:

```bash
./scripts/workflows.sh -p format-source-code
```

## Documentation

### Doxygen Comments

All public APIs **must** be documented with Doxygen comments. Use `/** ... */` style for documentation blocks.

```cpp
/**
 * @class Magic
 *
 * @brief Provides a C++ wrapper over the Magic Number Recognition Library.
 *
 * @note To use the Magic class for file type identification, ensure:
 *       1. The Magic instance must be opened.
 *       2. A magic database file must be successfully loaded.
 */
class Magic {
public:
    /**
     * @brief Load a magic database file.
     *
     * @param[in] database_file The path to the magic database file.
     *
     * @throws MagicLoadDatabaseFileError if loading the database file fails.
     * @throws EmptyPath if the database file path is empty.
     * @throws PathDoesNotExist if the database file does not exist.
     *
     * @note This function adds ".mgc" to the filename as appropriate.
     */
    void LoadDatabaseFile(const std::filesystem::path& database_file);
};
```

### Comment Style

+ Use `/* ... */` for comments.

+ Use `/** ... */` for documentation comments.
