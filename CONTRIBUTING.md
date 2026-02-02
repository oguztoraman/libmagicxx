# Contributing to Libmagicxx

Welcome to the Libmagicxx project! We appreciate your interest in contributing. Whether you are fixing bugs, adding features, improving documentation, or suggesting enhancements, your contributions are valuable and help make Libmagicxx better for everyone.

This guide provides everything you need to contribute successfully.

---

## Table of Contents

+ [Quick Reference](#quick-reference)

+ [Code of Conduct](#code-of-conduct)

+ [Types of Contributions](#types-of-contributions)

+ [Development Workflow](#development-workflow)

+ [Project Structure](#project-structure)

+ [Getting Started](#getting-started)

+ [Making Changes](#making-changes)

+ [Writing Tests](#writing-tests)

+ [Writing Documentation](#writing-documentation)

+ [Build and Test Commands](#build-and-test-commands)

+ [Debugging](#debugging)

+ [Creating a Pull Request](#creating-a-pull-request)

+ [Reporting Issues](#reporting-issues)

+ [Review Process](#review-process)

+ [Troubleshooting](#troubleshooting)

+ [FAQ](#faq)

+ [Thank You](#thank-you)


---

## Quick Reference

| Task | Command |
|------|---------|
| Build &amp; test (Clang) | `./scripts/workflows.sh -p linux-x86_64-clang-tests -c` |
| Build &amp; test (GCC) | `./scripts/workflows.sh -p linux-x86_64-gcc-tests -c` |
| Format code | `./scripts/workflows.sh -p format-source-code` |
| Build examples | `./scripts/workflows.sh -p linux-x86_64-clang-examples` |
| Build documentation | `./scripts/workflows.sh -p documentation` |
| List all presets | `./scripts/workflows.sh -l` |
| Run specific test | `./build/tests/magicxx_tests --gtest_filter="TestName.*"` |
| Initialize project | `./scripts/initialize.sh` |

---

## Code of Conduct

We expect all contributors to adhere to the <a href="CODE_OF_CONDUCT.md">Code of Conduct</a>. Please read it to understand the standards of behavior we expect from our community.

---

## Types of Contributions

| Type | Branch Prefix | PR Title Prefix | Description |
|------|---------------|-----------------|-------------|
| 🐛 **Bug Fix** | `bugfix/` | `[BUGFIX]` | Fix a bug in existing functionality |
| ✨ **Enhancement** | `enhancement/` | `[ENHANCEMENT]` | Add new feature or improve existing one |
| 📚 **Documentation** | `documentation/` | `[DOCUMENTATION]` | Improve docs, comments, or examples |
| 🧹 **Code Quality** | `quality/` | `[QUALITY]` | Refactoring, formatting, style improvements |

---

## Development Workflow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        Contribution Workflow                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  1. SETUP                     2. DEVELOP                                │
│  ┌──────────────────┐         ┌──────────────────┐                      │
│  │ Fork repository  │────────>│ Create branch    │                      │
│  │ Clone locally    │         │ (from main)      │                      │
│  │ Start container  │         │                  │                      │
│  │ Run initialize.sh│         │ bugfix/my-fix    │                      │
│  └──────────────────┘         └────────┬─────────┘                      │
│                                        │                                │
│                                        ▼                                │
│  4. VALIDATE                   3. CODE                                  │
│  ┌──────────────────┐         ┌──────────────────┐                      │
│  │ Build & test     │<────────│ Make changes     │                      │
│  │ Format code      │         │ Add/update tests │                      │
│  │ Update CHANGELOG │         │ Update docs      │                      │
│  │                  │         │ Commit often     │                      │
│  └────────┬─────────┘         └──────────────────┘                      │
│           │                                                             │
│           ▼                                                             │
│  5. SUBMIT                    6. REVIEW                                 │
│  ┌──────────────────┐         ┌──────────────────┐                      │
│  │ Push to fork     │────────>│ Maintainer review│                      │
│  │ (from host, not  │         │ Address feedback │                      │
│  │  container)      │         │ CI must pass     │                      │
│  │ Create PR        │         │ Merge!           │                      │
│  └──────────────────┘         └──────────────────┘                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Branch Strategy

+ **`main`**: Development branch for next feature release.
+ **`vX.Y.x`**: Stable release branches. All release tags (`vX.Y.0`, `vX.Y.1`, etc.) are created on these branches.
+ Major/minor releases (`vX.0.0`, `vX.Y.0`) create a new `vX.Y.x` branch from `main`.
+ Patch releases (`vX.Y.Z`) are tagged on existing `vX.Y.x` branches.
+ All bugfix changes are also merged to `main`.

---

## Project Structure

```
libmagicxx/
├── include/magicxx/           # 📦 PUBLIC API - Header files
│   └── magic.hpp              #    The main header (Recognition::Magic class)
│
├── sources/                   # 🔧 IMPLEMENTATION
│   └── magic.cpp              #    Implementation of magic.hpp
│
├── tests/                     # 🧪 UNIT TESTS (GoogleTest)
│   ├── CMakeLists.txt         #    Test build configuration
│   ├── main.cpp               #    Test runner entry point
│   └── magic_*_test.cpp       #    Test files (one per feature area)
│
├── examples/                  # 💡 EXAMPLES
│   ├── CMakeLists.txt         #    Examples build configuration
│   └── magic_examples.cpp     #    Runnable usage examples
│
├── cmake/                     # ⚙️ CMAKE CONFIGURATION
│   ├── STYLE_GUIDE.md         #    CMake coding conventions
│   ├── options.cmake          #    CMake option definitions
│   ├── targets.cmake          #    Library target definitions
│   └── ...                    #    Other CMake modules
│
├── scripts/                   # 🛠️ UTILITY SCRIPTS
│   ├── workflows.sh           #    Main build/test script
│   ├── initialize.sh          #    Project initialization
│   ├── launch_container.py    #    Development container launcher
│   └── ...                    #    Other scripts
│
├── databases/                 # 🗃️ MAGIC DATABASE FILES
│   ├── magic                  #    Source magic definitions
│   └── magic.mgc              #    Compiled magic database
│
├── external/                  # 📚 EXTERNAL DEPENDENCIES (Git submodules)
│   ├── file/                  #    libmagic (file command library)
│   ├── googletest/            #    GoogleTest framework
│   └── libgnurx/              #    Regex library for Windows
│
├── documentation/             # 📖 DOXYGEN DOCUMENTATION
├── .github/                   # 🔄 CI/CD WORKFLOWS
├── CMakeLists.txt             # 📋 Top-level CMake configuration
├── CMakePresets.json          # 🎯 CMake workflow presets
├── STYLE_GUIDE.md             # 📝 C++ coding conventions
├── CHANGELOG.md               # 📜 Version history
└── Containerfile              # 🐳 Development container definition
```

### Key Files for Contributors

| What You're Changing | Files to Modify |
|---------------------|-----------------|
| Public API | `include/magicxx/magic.hpp` |
| Implementation | `sources/magic.cpp` |
| Add a test | `tests/magic_*_test.cpp` (existing) or create new `tests/magic_yourfeature_test.cpp` |
| Add an example | `examples/magic_examples.cpp` |
| Fix a build issue | `cmake/*.cmake` or `CMakeLists.txt` |
| Update docs | `README.md`, `documentation/`, or Doxygen comments in `.hpp` |

---

## Getting Started

### Prerequisites (Host Machine)

Install these on your **host machine** (not the container):

| Tool | Purpose | Installation |
|------|---------|--------------|
| **Git** | Version control | [git-scm.com](https://git-scm.com/) |
| **Git LFS** | Large file storage | [git-lfs.com](https://git-lfs.com/) |
| **Python 3** | Container launcher script | [python.org](https://www.python.org/) |
| **Podman** | Container runtime | [podman.io](https://podman.io/getting-started/installation) |
| **VS Code** | Code Editor  | [code.visualstudio.com](https://code.visualstudio.com/) |
| **Dev Containers** | VS Code extension | Search "Dev Containers" in VS Code extensions |

### Step-by-Step Setup

#### Step 1: Fork and Clone

```bash
# Fork on GitHub, then clone your fork
git clone https://github.com/YOUR-USERNAME/libmagicxx.git
cd libmagicxx
```

#### Step 2: Launch Development Container

The development container is pre-built and hosted on GitHub Container Registry for fast startup:

```bash
# Pull from GHCR and launch (recommended, fastest)
python ./scripts/launch_container.py

# To update to the latest container image
python ./scripts/launch_container.py -u

# For release branches (e.g., v10.0.x), use the versioned container
python ./scripts/launch_container.py --tag v10.0.x

# To build locally from Containerfile (for container development)
python ./scripts/launch_container.py --local

# To rebuild local image
python ./scripts/launch_container.py -l -u
```

**Container Versioning:**

+ `latest` — used for `main` branch development
+ `vX.Y.x` — used for `vX.Y.x` stable branches (e.g., `v10.0.x` for branch `v10.0.x`)

CI workflows automatically select the appropriate container tag based on the target branch. When a new minor release (`vX.Y.0`) is tagged, the container is also tagged with the `vX.Y.x` format.

> ⚠️ **Important: Development Container Changes**
>
> If your contribution requires changes to the development container (`Containerfile` or `scripts/install_dependencies.sh`):
>
> 1. **Submit container changes as a separate PR first** — do not bundle them with other changes
> 2. **Wait for the container PR to be merged** before submitting dependent changes
> 3. All CI workflows run on the dev container, so container updates must be merged and published before other PRs can use them

#### Step 3: Attach VS Code to Container

1. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on macOS)

2. Select **"Dev Containers: Attach to Running Container..."**

3. Choose the running container that was started by `launch_container.py`:
   - If you used the GHCR image (default or with `--tag`), select the container whose **Image** field matches `ghcr.io/oguztoraman/libmagicxx-dev:<tag>` (e.g., `latest` or `v10.0.x`).
   - If you built the image locally, select the container named `libmagicxx-dev-local`.

#### Step 4: Install VS Code Extensions (Inside Container)

Install these extensions in the container for best experience:

+ [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)

+ [clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)

+ [LLDB DAP](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.lldb-dap)

+ [Git Graph](https://marketplace.visualstudio.com/items?itemName=mhutchie.git-graph)

#### Step 5: Initialize the Project

```bash
# Inside the container terminal
./scripts/initialize.sh
```

This builds external dependencies (libmagic, GoogleTest) and prepares the build environment.

#### Step 6: Verify Setup

```bash
# Build and run all tests to verify everything works
./scripts/workflows.sh -p linux-x86_64-clang-tests -c
```

Expected output: All tests should pass.

---

## Making Changes

### Step 1: Create a Branch

```bash
# For a bug fix
git checkout -b bugfix/fix-null-pointer-crash

# For a new feature
git checkout -b enhancement/add-buffer-identification

# For documentation
git checkout -b documentation/improve-readme

# For code quality
git checkout -b quality/refactor-error-handling
```

### Step 2: Understand the Code Style

All code must follow <a href="STYLE_GUIDE.md">STYLE_GUIDE.md</a>. Key rules:

#### Naming Conventions

```cpp
namespace Recognition {              /* PascalCase for namespaces */

class Magic {                        /* PascalCase for classes */
public:
    using FileTypeT = std::string;   /* PascalCase + T suffix for type aliases */
    
    template <typename ContainerT>   /* PascalCase + T suffix for type params */
    void SetFlags(ContainerT flags); /* PascalCase for functions */
    
    static constexpr int MAX_SIZE = 100;  /* SCREAMING_SNAKE_CASE for constants */
    
private:
    magic_t m_handle;                /* snake_case with m_ prefix for members */
    int m_flag_count;                /* snake_case for variables */
};

}  /* namespace Recognition */

/* Test fixture - PascalCase ending with Test */
struct MagicFlagsTest : testing::Test { };

/* Test cases - snake_case */
TEST_F(MagicFlagsTest, set_flags_returns_true_for_valid_magic) { }
```

#### File Naming

```
magic_feature.hpp          # snake_case, .hpp for headers
magic_feature.cpp          # snake_case, .cpp for source files
magic_feature_test.cpp     # _test.cpp suffix for test files
CHANGELOG.md               # SCREAMING_SNAKE_CASE for documentation files
```

### Step 3: Make Your Changes

1. **Edit the appropriate files** (see [Key Files for Contributors](#key-files-for-contributors))

2. **Add/update tests** for any behavior changes

3. **Update documentation** (Doxygen comments for API changes)

4. **Commit frequently** with descriptive messages using component prefixes:

#### Commit Message Format

```
<Component>: <Brief description>
```

**Component prefixes:**

| Prefix | Use For |
|--------|---------|
| `CI/CD:` | GitHub Actions workflows, CI/CD pipelines, automation |
| `CMake:` | Build system, CMakeLists.txt, presets |
| `Deps:` | External dependencies updates |
| `DevEnv:` | Container, development environment setup |
| `Docs:` | Documentation, README, Doxygen comments |
| `Examples:` | Example code changes |
| `Format:` | Clang tools (.clang-format, .clang-tidy), code formatting |
| `Magic:` | Changes to Magic class, core library functionality |
| `Release:` | Version bumps, changelog, release preparation |
| `Scripts:` | Shell and Python scripts in scripts/ directory |
| `Tests:` | Test files, test infrastructure |
| `Toolchains:` | CMake toolchain files, compiler configurations |
| `Utility:` | Utility classes (ProgressTracker, Percentage) |

**Examples:**

```bash
git commit -m "Magic: Add buffer identification support"
git commit -m "Docs: Fix rendering issues in README"
git commit -m "CMake: Add new test preset for coverage"
git commit -m "Tests: Add regression test for empty path handling"
git commit -m "CI/CD: Trigger builds on push to main branch"
git commit -m "DevEnv: Update container to Fedora 43"
```

For multi-component changes, use the primary component or combine:

```bash
git commit -m "Magic, Tests: Add IdentifyBuffer with unit tests"
```

### Step 4: Build and Test

```bash
# Build and run all tests
./scripts/workflows.sh -p linux-x86_64-clang-tests -c

# Or with GCC
./scripts/workflows.sh -p linux-x86_64-gcc-tests -c
```

### Step 5: Format Your Code

```bash
./scripts/workflows.sh -p format-source-code
```

### Step 6: Update CHANGELOG.md

Add your changes under the "Next Release" section:

```markdown
## Next Release

+ **[ENHANCEMENT]** Magic: Add buffer identification support.

+ **[BUGFIX]** Magic: Fix null pointer crash when database not loaded.

+ **[DOCUMENTATION]** Docs: Improve README examples.

+ **[QUALITY]** Magic: Refactor error handling for consistency.
```

---

## Writing Tests

### Test File Organization

Each feature area has its own test file:

| Test File | What It Tests |
|-----------|---------------|
| `magic_open_close_test.cpp` | Open(), Close(), IsOpen() |
| `magic_flags_test.cpp` | GetFlags(), SetFlags() |
| `magic_identify_file_test.cpp` | IdentifyFile() |
| `magic_identify_directory_test.cpp` | IdentifyFiles() with directories |
| `magic_identify_container_test.cpp` | IdentifyFiles() with containers |
| `magic_load_database_file_test.cpp` | LoadDatabaseFile() |
| `magic_parameters_test.cpp` | GetParameter(), SetParameter() |
| `magic_compile_test.cpp` | Compile() |
| `magic_check_test.cpp` | Check() |
| `magic_version_test.cpp` | GetVersion() |
| `magic_special_members_test.cpp` | Copy, move, assignment |
| `magic_progress_tracker_test.cpp` | ProgressTracker |
| `magic_percentage_test.cpp` | Percentage utility class |
| `magic_to_string_test.cpp` | ToString() functions |

### Test Structure Template

```cpp
/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_yourfeature_test.cpp
 * @brief Unit tests for YourFeature functionality.
 *
 * Tests cover:
 * - Normal operation scenarios
 * - Error handling (exceptions and noexcept variants)
 * - Edge cases
 *
 * @see Magic::YourFeature()
 */

#include <gtest/gtest.h>
#include "magic.hpp"

using namespace Recognition;

/**
 * @brief Test fixture for YourFeature tests.
 */
struct MagicYourFeatureTest : testing::Test {
protected:
    // Setup code runs before each test
    MagicYourFeatureTest()
    {
        // Initialize test fixtures
    }
    
    // Members available to all tests
    Magic m_valid_magic{Magic::Flags::Mime};
    Magic m_closed_magic;  // Default-constructed = closed
};

// Test naming: snake_case describing the scenario
TEST_F(MagicYourFeatureTest, returns_expected_value_for_valid_input)
{
    // Arrange
    auto input = /* ... */;
    
    // Act
    auto result = m_valid_magic.YourFeature(input);
    
    // Assert
    EXPECT_EQ(result, expected_value);
}

TEST_F(MagicYourFeatureTest, throws_exception_when_magic_is_closed)
{
    EXPECT_THROW(m_closed_magic.YourFeature(), MagicException);
}

TEST_F(MagicYourFeatureTest, noexcept_variant_returns_nullopt_when_closed)
{
    auto result = m_closed_magic.YourFeature(std::nothrow);
    EXPECT_FALSE(result.has_value());
}
```

### Running Specific Tests

```bash
# Run all tests
./build/tests/magicxx_tests

# Run tests matching a pattern
./build/tests/magicxx_tests --gtest_filter="MagicFlagsTest.*"

# Run a specific test
./build/tests/magicxx_tests --gtest_filter="MagicFlagsTest.set_flags_returns_true_for_valid_magic"

# List all tests without running
./build/tests/magicxx_tests --gtest_list_tests
```

---

## Writing Documentation

### Doxygen Comments for Public API

Every public class, method, and type must have Doxygen documentation:

```cpp
/**
 * @brief Identify the type of a file by examining its contents.
 *
 * Uses the magic database to determine the file type based on
 * content analysis rather than file extension.
 *
 * @param[in] file Path to the file to identify.
 *
 * @returns The identified file type as a string (format depends on flags).
 *
 * @throws MagicException If magic is not valid or file cannot be read.
 * @throws std::filesystem::filesystem_error If path is invalid.
 *
 * @pre IsValid() returns true.
 * @post Return value is non-empty on success.
 *
 * @code{.cpp}
 * Magic magic{Magic::Flags::Mime};
 * auto type = magic.IdentifyFile("/path/to/file");
 * std::println("Type: {}", type);  // "application/pdf; charset=binary"
 * @endcode
 *
 * @see IdentifyFiles() for batch identification.
 * @see Flags::Mime, Flags::MimeType, Flags::MimeEncoding
 *
 * @since 10.0.0
 */
[[nodiscard]] FileTypeT IdentifyFile(const std::filesystem::path& file) const;
```

### The `@since` Tag

The `@since` tag documents which version introduced the **exact symbol name**. This is important because users looking at documentation should be able to find the same symbol in the specified version.

**Rules for `@since` tags:**

1. **New API**: Use the upcoming release version (e.g., `@since 10.0.0`)

2. **Renamed API (breaking change)**: Update to the new version. If you rename `identify_file()` to `IdentifyFile()`, change `@since 5.0.0` to `@since 10.0.0` because a user going back to v5.0.0 won't find `IdentifyFile()`.

3. **Unchanged API**: Keep the original `@since` tag.

**Example — Renaming a method:**

```cpp
// Before (v9.x): @since 5.0.0 for identify_file()
// After (v10.0.0): @since 10.0.0 for IdentifyFile()

/**
 * @brief Identify the type of a file.
 * @since 10.0.0
 */
[[nodiscard]] FileTypeT IdentifyFile(const std::filesystem::path& file) const;
```

### Building Documentation

```bash
./scripts/workflows.sh -p documentation
```

The generated documentation site is located at `./build/documentation/html/index.html`. Open this file in your web browser to view the API documentation.

---

## Build and Test Commands

### Available Workflow Presets

Use `./scripts/workflows.sh -l` to see the full list. Key presets for contributors:

#### Utility Presets

| Preset | Description |
|--------|-------------|
| **`documentation`** | Generate Doxygen documentation |
| **`format-source-code`** | Format all source files |
| **`generate-source-package`** | Generate source package |
| **`generate-default-database-files`** | Generate default database files |

#### Linux Build Presets (Development Container)

| Preset | Description |
|--------|-------------|
| **`linux-x86_64-clang`** | Build libraries (Clang, Release) |
| **`linux-x86_64-gcc`** | Build libraries (GCC, Release) |
| **`linux-x86_64-clang-debug`** | Build libraries (Clang, Debug) |
| **`linux-x86_64-gcc-debug`** | Build libraries (GCC, Debug) |
| **`linux-x86_64-clang-tests`** | Build + run tests (Clang, Release) |
| **`linux-x86_64-gcc-tests`** | Build + run tests (GCC, Release) |
| **`linux-x86_64-clang-tests-debug`** | Build + run tests (Clang, Debug) |
| **`linux-x86_64-gcc-tests-debug`** | Build + run tests (GCC, Debug) |
| **`linux-x86_64-clang-examples`** | Build examples (Clang, Release) |
| **`linux-x86_64-gcc-examples`** | Build examples (GCC, Release) |
| **`linux-x86_64-clang-examples-debug`** | Build examples (Clang, Debug) |
| **`linux-x86_64-gcc-examples-debug`** | Build examples (GCC, Debug) |
| **`linux-x86_64-clang-packages`** | Build packages (Clang, Release) |
| **`linux-x86_64-gcc-packages`** | Build packages (GCC, Release) |

#### Windows Build Presets (Cross-compilation)

| Preset | Description |
|--------|-------------|
| **`windows-x86_64-clang`** | Build libraries (Clang, Release) |
| **`windows-x86_64-mingw64`** | Build libraries (MinGW64, Release) |
| **`windows-x86_64-clang-debug`** | Build libraries (Clang, Debug) |
| **`windows-x86_64-mingw64-debug`** | Build libraries (MinGW64, Debug) |
| **`windows-x86_64-clang-tests`** | Build + run tests (Clang, Release) |
| **`windows-x86_64-mingw64-tests`** | Build + run tests (MinGW64, Release) |
| **`windows-x86_64-clang-tests-debug`** | Build + run tests (Clang, Debug) |
| **`windows-x86_64-mingw64-tests-debug`** | Build + run tests (MinGW64, Debug) |
| **`windows-x86_64-clang-examples`** | Build examples (Clang, Release) |
| **`windows-x86_64-mingw64-examples`** | Build examples (MinGW64, Release) |
| **`windows-x86_64-clang-examples-debug`** | Build examples (Clang, Debug) |
| **`windows-x86_64-mingw64-examples-debug`** | Build examples (MinGW64, Debug) |
| **`windows-x86_64-clang-packages`** | Build packages (Clang, Release) |
| **`windows-x86_64-mingw64-packages`** | Build packages (MinGW64, Release) |

### Common Commands

```bash
# List all available presets
./scripts/workflows.sh -l

# Build and test with cache clearing (recommended for clean builds)
./scripts/workflows.sh -p linux-x86_64-clang-tests -c

# Build and test without clearing cache (faster incremental builds)
./scripts/workflows.sh -p linux-x86_64-clang-tests

# Build and run examples (examples are executed automatically)
./scripts/workflows.sh -p linux-x86_64-clang-examples

# Direct CMake workflow (alternative to scripts/workflows.sh)
cmake --workflow --preset linux-x86_64-clang-tests
```

---

## Debugging

### Debug Build

```bash
# Build with debug symbols
./scripts/workflows.sh -p linux-x86_64-clang-tests-debug -c
```

### Using VS Code Debugger

The project includes pre-configured debug launchers in `.vscode/launch.json`:

1. Install the **LLDB DAP** extension (for LLDB) or **C/C++ Extension Pack** (for GDB)
2. Build with debug symbols: `./scripts/workflows.sh -p linux-x86_64-clang-tests-debug -c`
3. Open the **Run and Debug** panel (`Ctrl+Shift+D`)
4. Select a launch configuration:
   - **GDB Debug** — Debug using GDB
   - **LLDB Debug** — Debug using LLDB
5. Set breakpoints in your code
6. Press **F5** to start debugging

> 💡 **Tip**: The debugger uses CMake's launch target. Select your target (e.g., `magicxx_tests`) from the CMake Tools status bar before debugging.

---

## Creating a Pull Request

### Before Submitting

Complete this checklist:

- [ ] Branch created from `main` (or release branch for bugfix)
- [ ] Code follows <a href="STYLE_GUIDE.md">STYLE_GUIDE.md</a>
- [ ] All tests pass: `./scripts/workflows.sh -p linux-x86_64-clang-tests`
- [ ] Code formatted: `./scripts/workflows.sh -p format-source-code`
- [ ] New tests added for new functionality
- [ ] Doxygen comments added for new public APIs
- [ ] CHANGELOG.md updated
- [ ] Linked to an existing issue (or created one)

### Push Your Changes

⚠️ **Important**: Git operations (`push`, `pull`, `fetch`) don't work inside the container. Use your **host VS Code window** for git operations.

> **Why?** The container doesn't have access to your SSH keys or git credentials configured on your host machine. These are intentionally not shared for security reasons.

**Recommended workflow:**

1. **Keep your original VS Code window open** (the one you used before attaching to the container)
2. **Develop** in the container-connected VS Code window
3. **When ready to push**, switch back to your original VS Code window (host)
4. **Push from the host** using VS Code's **Source Control panel** (Ctrl+Shift+G) or the integrated terminal

```bash
# Or use the terminal in the host VS Code window:
cd libmagicxx
git add .
git commit -m "Magic: Brief description of changes"
git push origin your-branch-name
```

### Create the Pull Request

1. Go to your fork on GitHub
2. Click **"Compare & pull request"**
3. Fill in the <a href="https://github.com/oguztoraman/libmagicxx/blob/main/PULL_REQUEST_TEMPLATE.md">PR template</a> — it includes a checklist and description section

### PR Title Format

```
[BUGFIX] Fix null pointer crash in IdentifyFile, Fixes issue #123.
[ENHANCEMENT] Add buffer identification support, Fixes issue #456.
[DOCUMENTATION] Improve README examples, Fixes issue #789.
[QUALITY] Refactor error handling for consistency, Fixes issue #012.
```

---

## Reporting Issues

### Before Creating an Issue

1. Search existing issues to avoid duplicates
2. Check if it's already fixed in `main` branch

### Issue Templates

When creating an issue, use the appropriate template:

- **Bug Report**: For reporting bugs
- **Feature Request**: For suggesting new features
- **Documentation**: For documentation improvements
- **Code Quality**: For code quality or style issues

### Tips for Good Reports

- **Be specific**: Include exact steps to reproduce, error messages, and system information
- **Provide context**: Explain what you were trying to accomplish
- **Include versions**: Libmagicxx version, compiler, OS
- **Attach logs**: Stack traces, build output, or screenshots if relevant

The issue templates will guide you through providing all necessary information.

---

## Review Process

### What Reviewers Look For

1. **Correctness**: Does the code work as intended?
2. **Tests**: Are there adequate tests?
3. **Style**: Does it follow STYLE_GUIDE.md?
4. **Documentation**: Are public APIs documented?
5. **ABI Stability**: Do changes break binary compatibility?

### Responding to Feedback

1. Address each comment
2. Push new commits (don't force-push during review)
3. Reply to comments explaining your changes
4. Request re-review when ready

### After Merge

- The CI will run final checks
- Your changes appear in the next release
- You'll be credited in the release notes

---

## Troubleshooting

### Container Won't Start

```bash
# Check if Podman is running
podman ps

# Re-pull the latest image from GHCR
python ./scripts/launch_container.py -u

# Or if using local build, rebuild the container
python ./scripts/launch_container.py -l -u
```

### Container Image Pull Fails

If you can't pull from GHCR, build locally instead:

```bash
python ./scripts/launch_container.py --local
```

### Build Fails with "No such file"

```bash
# Reinitialize the project (rebuilds external dependencies)
./scripts/initialize.sh
```

### Tests Fail After Pulling Latest Changes

```bash
# Clean build from scratch
./scripts/workflows.sh -p linux-x86_64-clang-tests -c
```

### Git Push Fails from Container

Git operations (`push`, `pull`, `fetch`) don't work inside the container. **Exit the container first**, then run git commands on your host machine.

### Clangd Shows Wrong Errors

```bash
# Regenerate compile_commands.json
./scripts/workflows.sh -p linux-x86_64-clang -c
```

### Code Formatting Changes Unexpected Files

Only your changes should be formatted. If formatting touches many files, someone may have committed unformatted code. Format and commit separately:

```bash
./scripts/workflows.sh -p format-source-code
git add .
git commit -m "Format code"
```

---

## FAQ

### Q: Do I need to sign a CLA?

No, we don't require a Contributor License Agreement. By submitting a PR, you agree your contributions are licensed under LGPL v3.

### Q: Can I work on multiple issues at once?

Yes, create separate branches for each issue.

### Q: How long until my PR is reviewed?

We aim to review PRs within a few days. Complex changes may take longer.

### Q: What if CI fails?

Check the CI logs on GitHub. Common issues:
- Formatting: Run `./scripts/workflows.sh -p format-source-code`
- Test failures: Run tests locally and fix
- Build errors: Check compiler output

### Q: What C++ standard is required?

C++23. All code must compile with GCC 14+ and Clang 19+.

### Q: How do I add a new CMake option?

1. Add to `cmake/options.cmake`
2. Follow conventions in `cmake/STYLE_GUIDE.md`
3. Document in README.md

### Q: What if I need help?

- Check existing issues and discussions
- Create a new issue with the "question" label
- Be specific about what you're trying to do

---

## Thank You

Thank you for contributing to Libmagicxx! Your efforts help improve the project for everyone. Every contribution, no matter how small, is appreciated.

Happy coding! 🎉
