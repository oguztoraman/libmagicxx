/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic.hpp
 * @brief Main header file for the libmagicxx library.
 *
 * This file contains the Magic class, a modern C++23 wrapper for libmagic —
 * the library that powers the Unix file command. It provides type-safe,
 * RAII-based file type identification based on content rather than file extensions.
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 *
 * @see https://github.com/oguztoraman/libmagicxx
 * @see https://github.com/file/file (underlying libmagic)
 *
 * @example magic_examples.cpp
 *
 * @defgroup magic_core Core Magic API
 * @brief Core file type identification functionality.
 *
 * The Magic class provides the primary interface for identifying file types
 * using magic number analysis. It wraps the underlying libmagic C library
 * with a modern, type-safe C++23 API.
 *
 * ### Quick Start
 *
 * @code{.cpp}
 * #include <magic.hpp>
 * #include <print>
 *
 * using namespace Recognition;
 *
 * int main() {
 *     // Create and configure Magic instance
 *     Magic magic{Magic::Flags::Mime};
 *
 *     // Identify a file
 *     auto type = magic.IdentifyFile("/path/to/file");
 *     std::println("File type: {}", type);
 *
 *     return 0;
 * }
 * @endcode
 *
 * ### Exception Safety
 *
 * Most methods have two overloads:
 * - **Throwing version**: Throws exceptions on error (default)
 * - **Noexcept version**: Returns `std::optional` or `std::expected` (pass `std::nothrow`)
 *
 * @code{.cpp}
 * // Throwing version
 * try {
 *     auto type = magic.IdentifyFile(path);
 * } catch (const MagicException& e) {
 *     // Handle error
 * }
 *
 * // Noexcept version
 * auto result = magic.IdentifyFile(path, std::nothrow);
 * if (result) {
 *     // Use *result
 * } else {
 *     // Handle error via result.error()
 * }
 * @endcode
 */

#ifndef MAGIC_HPP
#define MAGIC_HPP

#include <bitset>
#include <expected>
#include <map>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "magic_exception.hpp"
#include "progress_tracker.hpp"
#include "utility.hpp"

/**
 * @namespace Recognition
 * @brief Root namespace for the libmagicxx library.
 *
 * The Recognition namespace contains all public types and functions provided
 * by the libmagicxx library for file type identification.
 *
 * ### Main Components
 *
 * - **Magic**: The primary class for file type identification
 * - **MagicException**: Base exception class and derived exception types
 * - **Utility**: Helper types and functions (ProgressTracker, Percentage, etc.)
 *
 * @since 10.0.0
 */
namespace Recognition {
/**
 * @class Magic
 * @ingroup magic_core
 *
 * @brief A modern C++23 wrapper for libmagic — the library that powers the Unix file command.
 *
 * The Magic class provides a high-level, type-safe interface for identifying
 * file types based on their content (magic numbers) rather than file extensions.
 * It wraps the underlying libmagic C library with RAII semantics, exception safety,
 * and modern C++ features.
 *
 * ### Key Features
 *
 * - **RAII Resource Management**: Automatic cleanup of libmagic resources
 * - **Dual API**: Throwing and noexcept overloads for all operations
 * - **Progress Tracking**: Monitor batch file identification progress
 * - **Flexible Configuration**: Extensive flags and parameters for customization
 * - **Thread Safety**: Each Magic instance is independent (not thread-safe within instance)
 *
 * ### Lifecycle
 *
 * A Magic instance follows a three-state lifecycle:
 *
 * @verbatim
 * +-----------------------------------------------------------------------------------+
 * |                              Magic State Diagram                                  |
 * +-----------------------------------------------------------------------------------+
 * |                                                                                   |
 * |  Constructor()                                       Constructor(flags)           |
 * |       |                                                     |                     |
 * |       V                                                     V                     |
 * |   +------------+  Open(flags)  +------------+         +------------+              |
 * |   |   CLOSED   |-------------->|   OPENED   |<--------|   VALID    |              |
 * |   +--+---------+               +--+------+--+  Open   +---------+--+              |
 * |      |    ^   ^                   |  ^   |     (flags)    ^     |                 |
 * |      +----+   |                   |  +---+                |     |                 |
 * |      Close()  |                   |  Open(flags)          |     |                 |
 * |               |                   |                       |     |                 |
 * |               |                   +-----------------------+     |                 |
 * |               |                       LoadDatabaseFile()        |                 |
 * |               |                                                 |                 |
 * |               +-------------------------------------------------+                 |
 * |                                     Close()                                       |
 * |                                                                                   |
 * +-----------------------------------------------------------------------------------+
 * @endverbatim
 *
 * #### State Descriptions
 *
 * | State | IsOpen() | IsDatabaseLoaded() | IsValid() | Can Identify? |
 * |-------|----------|-------------------|-----------|---------------|
 * | **Closed** | `false` | `false` | `false` | No |
 * | **Opened** | `true` | `false` | `false` | No |
 * | **Valid** | `true` | `true` | `true` | Yes |
 *
 * #### Usage Patterns
 *
 * @code{.cpp}
 * // Pattern 1: Single-step construction (recommended)
 * Magic magic1{Magic::Flags::Mime};  // Opens and loads default database
 * assert(magic1.IsValid());
 *
 * // Pattern 2: Manual lifecycle control
 * Magic magic2;                       // Closed state
 * assert(!magic2.IsOpen());
 * magic2.Open(Magic::Flags::Mime);    // Opened state
 * assert(magic2.IsOpen() && !magic2.IsDatabaseLoaded());
 * magic2.LoadDatabaseFile();          // Valid state
 * assert(magic2.IsValid());
 *
 * // Pattern 3: Reopen with different flags
 * magic2.Open(Magic::Flags::Extension);  // Back to Opened (database unloaded)
 * magic2.LoadDatabaseFile();             // Valid again with new flags
 *
 * // Pattern 4: Check before use
 * if (magic2.IsValid()) {
 *     auto type = magic2.IdentifyFile("file.txt");
 * }
 *
 * // Pattern 5: Boolean conversion (same as IsValid)
 * if (magic2) {
 *     auto type = magic2.IdentifyFile("file.txt");
 * }
 * @endcode
 *
 * @note Close() is idempotent—safe to call multiple times on any state.
 * @note The destructor automatically closes the instance.
 *
 * ### Output Formats
 *
 * The Magic class can return file type information in several formats,
 * controlled by the Flags enum:
 *
 * | Flag | Output Example |
 * |------|----------------|
 * | None | "ASCII text" |
 * | MimeType | "text/plain" |
 * | MimeEncoding | "us-ascii" |
 * | Mime | "text/plain; charset=us-ascii" |
 * | Extension | "txt/asc" |
 * | Apple | "TEXT/ttxt" |
 *
 * @note Each Magic instance holds its own libmagic cookie. Instances are
 *       move-only (not copyable) due to resource ownership.
 *
 * @warning The Magic class is **not thread-safe**. Do not share a single
 *          instance across threads. Create separate instances per thread.
 *
 * @see Magic::Flags for configuration options
 * @see Magic::Parameters for tunable limits
 * @see MagicException for error handling
 *
 * @since 10.0.0
 */
class Magic {
public:
    /**
     * @brief Flags for configuring Magic behavior.
     * @ingroup magic_core
     *
     * The Flags enum controls how Magic identifies files and formats output.
     * Flags can be combined using bitwise OR operations.
     *
     * ### Common Flag Combinations
     *
     * @code{.cpp}
     * // Get MIME type only
     * Magic magic1{Magic::Flags::MimeType};
     *
     * // Get full MIME with encoding
     * Magic magic2{Magic::Flags::Mime};
     *
     * // Follow symlinks and decompress files
     * Magic magic3{Magic::Flags::Symlink | Magic::Flags::Compress};
     *
     * // Using a container of flags
     * Magic magic4{{Magic::Flags::Mime, Magic::Flags::Debug}};
     * @endcode
     *
     * @see SetFlags() to change flags after construction
     * @see GetFlags() to retrieve current flags
     *
     * @since 10.0.0
     */
    enum class Flags : unsigned long long {
        /* clang-format off */
        None             = 0ULL,       /**< No special handling. Default textual output. */
        Debug            = 1ULL << 0,  /**< Print debugging messages to stderr. Useful for troubleshooting. */
        Symlink          = 1ULL << 1,  /**< If the file is a symlink, follow it and identify the target. */
        Compress         = 1ULL << 2,  /**< If the file is compressed, decompress and identify contents. */
        Devices          = 1ULL << 3,  /**< Open block/character devices and examine their contents. */
        MimeType         = 1ULL << 4,  /**< Return MIME type (e.g., "text/plain") instead of description. */
        ContinueSearch   = 1ULL << 5,  /**< Return all matches, not just the first one. */
        CheckDatabase    = 1ULL << 6,  /**< Check database consistency and print warnings to stderr. */
        PreserveAtime    = 1ULL << 7,  /**< Preserve access time of analyzed files (if supported by OS). */
        Raw              = 1ULL << 8,  /**< Don't convert unprintable characters to \\ooo octal. */
        Error            = 1ULL << 9,  /**< Treat OS errors as real errors instead of printing in buffer. */
        MimeEncoding     = 1ULL << 10, /**< Return MIME encoding (e.g., "us-ascii") instead of description. */
        Mime             = 1ULL << 11, /**< Shorthand for MimeType | MimeEncoding. Returns full MIME. */
        Apple            = 1ULL << 12, /**< Return Apple creator and type codes. */
        Extension        = 1ULL << 13, /**< Return slash-separated list of file extensions. */
        CompressTransp   = 1ULL << 14, /**< Report on uncompressed data only, hide compression layer. */
        NoCompressFork   = 1ULL << 15, /**< Don't use decompressors that require fork(). */
        Nodesc           = 1ULL << 16, /**< Shorthand for Extension | Mime | Apple. */
        NoCheckCompress  = 1ULL << 17, /**< Skip compressed file inspection. */
        NoCheckTar       = 1ULL << 18, /**< Skip tar archive examination. */
        NoCheckSoft      = 1ULL << 19, /**< Skip magic file consultation. */
        NoCheckApptype   = 1ULL << 20, /**< Skip EMX application type check (EMX only). */
        NoCheckElf       = 1ULL << 21, /**< Skip ELF details printing. */
        NoCheckText      = 1ULL << 22, /**< Skip text file type detection. */
        NoCheckCdf       = 1ULL << 23, /**< Skip MS Compound Document inspection. */
        NoCheckCsv       = 1ULL << 24, /**< Skip CSV file examination. */
        NoCheckTokens    = 1ULL << 25, /**< Skip known token search in ASCII files. */
        NoCheckEncoding  = 1ULL << 26, /**< Skip text encoding detection. */
        NoCheckJson      = 1ULL << 27, /**< Skip JSON file examination. */
        NoCheckSimh      = 1ULL << 28, /**< Skip SIMH tape file examination. */
        NoCheckBuiltin   = 1ULL << 29  /**< Use only magic file, skip all built-in tests. */
        /* clang-format on */
    };

    /**
     * @class FlagsMask
     *
     * @brief Bridge class between Flags enum class and the internal bitmask.
     *
     * Provides implicit conversion from individual Flags values and supports
     * bitwise OR for combining flags. This allows Flags to be used directly
     * wherever a FlagsMaskT is expected, without requiring explicit conversion.
     *
     * @code{.cpp}
     * // Single flag converts implicitly
     * Magic magic1{Magic::Flags::Mime};
     *
     * // Combined flags via operator|
     * Magic magic2{Magic::Flags::Mime | Magic::Flags::Compress};
     * @endcode
     *
     * @since 11.0.0
     */
    class FlagsMask {
    public:
        /**
         * @brief Default constructor. Creates an empty flags mask.
         */
        constexpr FlagsMask() noexcept = default;

        /**
         * @brief Implicit constructor from a single flag.
         *
         * This is the key bridge that allows Flags values to be used
         * wherever FlagsMaskT is expected, without explicit conversion.
         *
         * @param[in] flag The flag to set in the mask.
         */
        constexpr FlagsMask(Flags flag) noexcept
          : m_mask{static_cast<unsigned long long>(flag)}
        { }

        /**
         * @brief Combine this mask with another mask.
         *
         * @param[in] other The other mask to combine.
         *
         * @returns A new FlagsMask with all bits from both masks set.
         */
        constexpr FlagsMask operator|(const FlagsMask& other) const noexcept
        {
            FlagsMask result;
            result.m_mask = m_mask | other.m_mask;
            return result;
        }

        /**
        /**
         * @brief Test whether a specific bit is set.
         *
         * @param[in] pos Bit position to test.
         *
         * @returns true if the bit at the given position is set.
         */
        constexpr bool operator[](std::size_t pos) const
        {
            return m_mask[pos];
        }

        /**
         * @brief Get the number of bits in the mask.
         *
         * @returns The size of the underlying bitmask (30).
         */
        constexpr std::size_t size() const noexcept
        {
            return m_mask.size();
        }

        /**
         * @brief Test whether no bits are set.
         *
         * @returns true if no bits are set, false otherwise.
         */
        constexpr bool none() const noexcept
        {
            return m_mask.none();
        }

    private:
        std::bitset<30uz>
            m_mask{}; /**< Internal bitmask storing combined flag values. */
    };

    /**
     * @brief Combine two Flags values into a FlagsMask.
     *
     * @param[in] lhs Left-hand side flag.
     * @param[in] rhs Right-hand side flag.
     *
     * @returns A FlagsMask with both flags set.
     */
    friend constexpr FlagsMask operator|(Flags lhs, Flags rhs) noexcept
    {
        return FlagsMask{lhs} | FlagsMask{rhs};
    }

    /**
     * @brief Combine a Flags value with a FlagsMask.
     *
     * Enables expressions like `Flags::A | (Flags::B | Flags::C)`.
     *
     * @param[in] lhs Left-hand side flag.
     * @param[in] rhs Right-hand side mask.
     *
     * @returns A FlagsMask with all bits from both operands set.
     */
    friend constexpr FlagsMask operator|(
        Flags            lhs,
        const FlagsMask& rhs
    ) noexcept
    {
        return FlagsMask{lhs} | rhs;
    }

    /**
     * @brief Combine a FlagsMask with a Flags value (non-member symmetric overload).
     *
     * Enables expressions like `(Flags::A | Flags::B) | Flags::C`
     * without relying on member lookup.
     *
     * @param[in] lhs Left-hand side mask.
     * @param[in] rhs Right-hand side flag.
     *
     * @returns A FlagsMask with all bits from both operands set.
     */
    friend constexpr FlagsMask operator|(
        const FlagsMask& lhs,
        Flags            rhs
    ) noexcept
    {
        return lhs | FlagsMask{rhs};
    }

    /**
     * @typedef FlagsMaskT
     *
     * @brief Bitmask type representing a set of Magic::Flags used to configure the Magic instance.
     *
     * @since 10.0.0
     */
    using FlagsMaskT = FlagsMask;

    /**
     * @typedef FileTypeT
     *
     * @brief String type representing the detected type of a file.
     *
     * @since 10.0.0
     */
    using FileTypeT = std::string;

    /**
     * @typedef ErrorMessageT
     *
     * @brief String type representing an error message from file identification.
     *
     * @since 10.0.0
     */
    using ErrorMessageT = std::string;

    /**
     * @typedef ExpectedFileTypeT
     *
     * @brief Result type for file identification, containing either a file type or an error message.
     *
     * @since 10.0.0
     */
    using ExpectedFileTypeT = std::expected<FileTypeT, ErrorMessageT>;

    /**
     * @typedef FileTypeMapT
     *
     * @brief Map from file paths to their detected types.
     *
     * @since 10.0.0
     */
    using FileTypeMapT = std::map<std::filesystem::path, FileTypeT>;

    /**
     * @typedef FileTypeEntryT
     *
     * @brief Key-value pair representing a single file and its detected type.
     *
     * @since 10.0.0
     */
    using FileTypeEntryT = FileTypeMapT::value_type;

    /**
     * @typedef ExpectedFileTypeMapT
     *
     * @brief Map from file paths to expected file type results (success or error).
     *
     * @since 10.0.0
     */
    using ExpectedFileTypeMapT = std::map<
        std::filesystem::path,
        ExpectedFileTypeT
    >;

    /**
     * @typedef ExpectedFileTypeEntryT
     *
     * @brief Key-value pair representing a single file and its expected file type result.
     *
     * @since 10.0.0
     */
    using ExpectedFileTypeEntryT = ExpectedFileTypeMapT::value_type;

    /**
     * @typedef ProgressTrackerT
     *
     * @brief Alias for a shared pointer to a progress tracker used for monitoring file identification progress.
     *
     * @see Utility::ProgressTracker
     *
     * @since 10.0.0
     */
    using ProgressTrackerT = Utility::SharedProgressTrackerT;

    /**
     * @brief Parameters for tuning Magic behavior limits.
     * @ingroup magic_core
     *
     * The Parameters enum provides access to various internal limits that
     * control how deeply Magic analyzes files. Adjusting these can help
     * balance between thoroughness and performance.
     *
     * @code{.cpp}
     * Magic magic{Magic::Flags::Mime};
     *
     * // Limit bytes scanned for faster performance
     * magic.SetParameter(Magic::Parameters::BytesMax, 1024 * 1024);
     *
     * // Get current value
     * auto bytes = magic.GetParameter(Magic::Parameters::BytesMax);
     *
     * // Get all parameters
     * auto params = magic.GetParameters();
     * for (const auto& [param, value] : params) {
     *     std::println("{}: {}", ToString(param), value);
     * }
     * @endcode
     *
     * @see SetParameter() to modify a single parameter
     * @see SetParameters() to modify multiple parameters
     * @see GetParameter() to retrieve a single parameter value
     * @see GetParameters() to retrieve all parameter values
     *
     * @since 10.0.0
     */
    enum class Parameters : std::size_t {
        /* clang-format off */
        IndirMax     = 0uz, /**< Maximum recursion depth for indirect magic (default: 15). */
        NameMax      = 1uz, /**< Maximum use count for name/use magic entries (default: 30). */
        ElfPhnumMax  = 2uz, /**< Maximum ELF program headers to process (default: 128). */
        ElfShnumMax  = 3uz, /**< Maximum ELF section headers to process (default: 32768). */
        ElfNotesMax  = 4uz, /**< Maximum ELF notes to process (default: 256). */
        RegexMax     = 5uz, /**< Maximum regex search length in bytes (default: 8192). */
        BytesMax     = 6uz, /**< Maximum bytes to read from file (default: 7340032 = 7MB). */
        EncodingMax  = 7uz, /**< Maximum bytes to scan for encoding detection (default: 1048576 = 1MB). */
        ElfShsizeMax = 8uz, /**< Maximum ELF section size to process (default: 134217728 = 128MB). */
        MagWarnMax   = 9uz  /**< Maximum warnings to tolerate from magic file (default: 64). */
        /* clang-format on */
    };

    /**
     * @typedef FlagsContainerT
     *
     * @brief Container type holding a collection of Magic::Flags.
     *
     * @since 10.0.0
     */
    using FlagsContainerT = std::vector<Flags>;

    /**
     * @typedef ParameterValueMapT
     *
     * @brief Map from Magic::Parameters to their corresponding values.
     *
     * @since 10.0.0
     */
    using ParameterValueMapT = std::map<Parameters, std::size_t>;

    /**
     * @typedef ParameterValueT
     *
     * @brief Key-value pair representing a single parameter and its value.
     *
     * @since 10.0.0
     */
    using ParameterValueT = ParameterValueMapT::value_type;

    /**
     * @brief Path to the default magic database file.
     *
     * This static member holds the path to the default magic database file
     * that was configured at build time. It is used as the default parameter
     * for LoadDatabaseFile() and constructors.
     *
     * @note On Linux, this typically points to "/usr/share/magicxx/databases/magic".
     *       On Windows, this typically points to "C:/Program Files/magicxx/databases/magic".
     *
     * @see LoadDatabaseFile()
     *
     * @since 10.0.0
     */
    static std::string_view DEFAULT_DATABASE_FILE;

    /**
     * @defgroup constructors Constructors
     * @ingroup magic_core
     * @brief Magic construction methods.
     *
     * Constructors for creating Magic instances in various initial states.
     *
     * @{
     */

    /**
     * @brief Default constructor. Creates an unopened Magic instance.
     *
     * The instance is not valid for file identification until Open() and
     * LoadDatabaseFile() are called.
     *
     * @code{.cpp}
     * Magic magic;  // Not yet valid
     * magic.Open(Magic::Flags::Mime);
     * magic.LoadDatabaseFile();
     * // Now valid for identification
     * @endcode
     *
     * @see Open()
     * @see LoadDatabaseFile()
     * @see IsValid()
     *
     * @since 10.0.0
     */
    Magic() noexcept;

    /**
     * @brief Construct and initialize Magic with flags and database.
     *
     * This constructor opens Magic with the specified flags and loads the
     * magic database file in a single step. On success, the instance is
     * immediately ready for file identification.
     *
     * @param[in] flags_mask    Configuration flags (use Flags enum values combined with |).
     * @param[in] database_file Path to magic database file (default: DEFAULT_DATABASE_FILE).
     *
     * @throws MagicOpenError             If opening the Magic instance fails.
     * @throws EmptyPath                  If database_file path is empty.
     * @throws PathDoesNotExist           If database_file does not exist.
     * @throws PathIsNotRegularFile       If database_file is not a regular file.
     * @throws MagicLoadDatabaseFileError If loading the database fails.
     *
     * @code{.cpp}
     * // Use MIME output with default database
     * Magic magic1{Magic::Flags::Mime};
     *
     * // Combine flags
     * Magic magic2{Magic::Flags::Mime | Magic::Flags::Compress};
     *
     * // Custom database path
     * Magic magic3{Magic::Flags::Mime, "/custom/path/magic"};
     * @endcode
     *
     * @note The ".mgc" extension is automatically appended to the database path if needed.
     *
     * @see LoadDatabaseFile()
     *
     * @since 10.0.0
     */
    explicit Magic(
        FlagsMaskT                   flags_mask,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    );

    /**
     * @brief Construct and initialize Magic with flags and database (noexcept version).
     *
     * Non-throwing variant that silently fails if initialization errors occur.
     * Check IsValid() after construction to verify success.
     *
     * @param[in] flags_mask    Configuration flags (use Flags enum values combined with |).
     * @param[in] tag           Pass `std::nothrow` to select this overload.
     * @param[in] database_file Path to magic database file (default: DEFAULT_DATABASE_FILE).
     *
     * @code{.cpp}
     * Magic magic{Magic::Flags::Mime, std::nothrow};
     * if (magic.IsValid()) {
     *     // Safe to use
     * }
     * @endcode
     *
     * @note The ".mgc" extension is automatically appended to the database path if needed.
     *
     * @see IsValid()
     *
     * @since 10.0.0
     */
    Magic(
        FlagsMaskT                   flags_mask,
        const std::nothrow_t&        tag,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /**
     * @brief Construct and initialize Magic with a container of flags.
     *
     * Alternative constructor accepting a container of individual flags
     * instead of a bitmask. Useful when flags are determined at runtime.
     *
     * @param[in] flags_container Vector or other container of Flags values.
     * @param[in] database_file   Path to magic database file (default: DEFAULT_DATABASE_FILE).
     *
     * @throws MagicOpenError             If opening the Magic instance fails.
     * @throws EmptyPath                  If database_file path is empty.
     * @throws PathDoesNotExist           If database_file does not exist.
     * @throws PathIsNotRegularFile       If database_file is not a regular file.
     * @throws MagicLoadDatabaseFileError If loading the database fails.
     *
     * @code{.cpp}
     * std::vector<Magic::Flags> flags = {Magic::Flags::Mime, Magic::Flags::Compress};
     * Magic magic{flags};
     * @endcode
     *
     * @note The ".mgc" extension is automatically appended to the database path if needed.
     *
     * @see FlagsContainerT
     *
     * @since 10.0.0
     */
    explicit Magic(
        const FlagsContainerT&       flags_container,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    );

    /**
     * @brief Construct and initialize Magic with a container of flags (noexcept version).
     *
     * Non-throwing variant that silently fails if initialization errors occur.
     * Check IsValid() after construction to verify success.
     *
     * @param[in] flags_container Vector or other container of Flags values.
     * @param[in] tag             Pass `std::nothrow` to select this overload.
     * @param[in] database_file   Path to magic database file (default: DEFAULT_DATABASE_FILE).
     *
     * @note The ".mgc" extension is automatically appended to the database path if needed.
     *
     * @see IsValid()
     *
     * @since 10.0.0
     */
    Magic(
        const FlagsContainerT&       flags_container,
        const std::nothrow_t&        tag,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /** @} constructors */

    /**
     * @defgroup special_member_functions Special Member Functions
     * @ingroup magic_core
     * @brief Move, copy, and destruction operations.
     *
     * Special member functions controlling Magic lifetime and resource ownership.
     *
     * @{
     */

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the Magic instance from `other` to this object.
     * After the move, `other` is left in a closed state and must be reopened
     * before use.
     *
     * @param[in,out] other The Magic instance to move from. Left closed after move.
     *
     * @code{.cpp}
     * Magic magic1{Magic::Flags::Mime};
     * Magic magic2{std::move(magic1)};  // magic1 is now closed
     * // magic2 is valid, magic1 is not
     * @endcode
     *
     * @since 10.0.0
     */
    Magic(Magic&& other) noexcept;

    /**
     * @brief Deleted copy constructor.
     *
     * Magic instances cannot be copied because they own unique libmagic resources.
     * Use move semantics instead.
     *
     * @since 10.0.0
     */
    Magic(const Magic&) = delete;

    /**
     * @brief Move assignment operator.
     *
     * Transfers ownership of the Magic instance from `other` to this object.
     * Any existing resources in this object are released first.
     * After the move, `other` is left in a closed state.
     *
     * @param[in,out] other The Magic instance to move from. Left closed after move.
     * @returns Reference to this object.
     *
     * @since 10.0.0
     */
    Magic& operator=(Magic&& other) noexcept;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Magic instances cannot be copied because they own unique libmagic resources.
     * Use move semantics instead.
     *
     * @since 10.0.0
     */
    Magic& operator=(const Magic&) = delete;

    /**
     * @brief Destructor.
     *
     * Automatically releases all libmagic resources. Equivalent to calling Close().
     *
     * @since 10.0.0
     */
    ~Magic();

    /** @} special_member_functions */

    /**
     * @defgroup validity_checking Validity Checking
     * @ingroup magic_core
     * @brief Instance and database validation methods.
     *
     * Methods for checking instance validity and database file integrity.
     *
     * @{
     */

    /**
     * @brief Boolean conversion operator for validity checking.
     *
     * Allows using a Magic instance directly in boolean contexts.
     * Returns true if the instance is valid for file identification
     * (opened and database loaded).
     *
     * @returns `true` if valid (opened and database loaded), `false` otherwise.
     *
     * @code{.cpp}
     * Magic magic{Magic::Flags::Mime, std::nothrow};
     * if (magic) {
     *     auto type = magic.IdentifyFile("file.txt");
     * }
     * @endcode
     *
     * @see IsValid()
     * @see IsOpen()
     * @see IsDatabaseLoaded()
     *
     * @since 10.0.0
     */
    [[nodiscard]] operator bool() const noexcept;

    /**
     * @brief Check magic database file for validity.
     *
     * Static utility method that validates the entries in a magic database file.
     * Useful for verifying custom database files before use.
     *
     * @param[in] database_file Path to database file to check (default: DEFAULT_DATABASE_FILE).
     *
     * @returns `true` if the database has valid entries, `false` otherwise.
     *
     * @code{.cpp}
     * if (Magic::Check("/path/to/custom.mgc")) {
     *     std::println("Database is valid");
     * }
     * @endcode
     *
     * @note This is a static method - no Magic instance is required.
     *
     * @see Compile()
     *
     * @since 10.0.0
     */
    [[nodiscard]] static bool Check(
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /** @} validity_checking */

    /**
     * @defgroup instance_control Instance Control
     * @ingroup magic_core
     * @brief Instance lifecycle management.
     *
     * Methods for closing instances and compiling databases.
     *
     * @{
     */

    /**
     * @brief Close the Magic instance.
     *
     * Releases all libmagic resources and transitions the instance to the
     * **Closed** state. After calling Close(), the instance is no longer
     * valid for file identification until Open() and LoadDatabaseFile()
     * are called again.
     *
     * @par Lifecycle Transition
     * - From **Opened** → **Closed**
     * - From **Valid** → **Closed**
     * - From **Closed** → **Closed** (no-op)
     *
     * @note Safe to call multiple times (idempotent). Automatically called
     *       by the destructor.
     *
     * @see @ref Magic Core "Magic Lifecycle" for state diagram
     * @see Open()
     * @see IsOpen()
     *
     * @since 10.0.0
     */
    void Close() noexcept;

    /**
     * @brief Compile a magic database file.
     *
     * Static utility method that compiles magic source files into binary format.
     * The compiled file is named with ".mgc" appended to the source filename.
     *
     * @param[in] database_file Path to database file to compile (default: DEFAULT_DATABASE_FILE).
     *
     * @returns `true` on successful compilation, `false` otherwise.
     *
     * @code{.cpp}
     * if (Magic::Compile("/path/to/magic")) {
     *     // Creates /path/to/magic.mgc
     *     std::println("Database compiled successfully");
     * }
     * @endcode
     *
     * @note This is a static method - no Magic instance is required.
     *
     * @see Check()
     *
     * @since 10.0.0
     */
    [[nodiscard]] static bool Compile(
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /** @} instance_control */

    /**
     * @defgroup flag_management Flag Management
     * @ingroup magic_core
     * @brief Flag retrieval methods.
     *
     * Methods for querying current configuration flags.
     *
     * @{
     */

    /**
     * @brief Get the current flags.
     *
     * Retrieves the flags currently configured for this Magic instance.
     *
     * @returns Container of active Flags values.
     *
     * @throws MagicIsClosed If the Magic instance is closed.
     *
     * @code{.cpp}
     * auto flags = magic.GetFlags();
     * std::println("Active flags: {}", ToString(flags));
     * @endcode
     *
     * @see SetFlags()
     * @see ToString(const FlagsContainerT&)
     *
     * @since 10.0.0
     */
    [[nodiscard]] FlagsContainerT GetFlags() const;

    /**
     * @brief Get the current flags (noexcept version).
     *
     * @param[in] tag Pass `std::nothrow` to select this overload.
     *
     * @returns Container of active Flags values, or `std::nullopt` if closed.
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::optional<FlagsContainerT> GetFlags(
        const std::nothrow_t& tag
    ) const noexcept;

    /** @} flag_management */

    /**
     * @defgroup parameter_management Parameter Management
     * @ingroup magic_core
     * @brief Parameter retrieval methods.
     *
     * Methods for querying libmagic parameter values.
     *
     * @{
     */

    /**
     * @brief Get the value of a specific parameter.
     *
     * Retrieves the current value of the specified parameter.
     *
     * @param[in] parameter The parameter to query.
     *
     * @returns Current value of the parameter.
     *
     * @throws MagicIsClosed If the Magic instance is closed.
     *
     * @code{.cpp}
     * auto maxBytes = magic.GetParameter(Magic::Parameters::BytesMax);
     * std::println("Maximum bytes to scan: {}", maxBytes);
     * @endcode
     *
     * @see Parameters for available parameters
     * @see SetParameter()
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::size_t GetParameter(Parameters parameter) const;

    /**
     * @brief Get the value of a specific parameter (noexcept version).
     *
     * @param[in] parameter The parameter to query.
     * @param[in] tag       Pass `std::nothrow` to select this overload.
     *
     * @returns Current value of the parameter, or `std::nullopt` if closed.
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::optional<std::size_t> GetParameter(
        Parameters            parameter,
        const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get all parameter values.
     *
     * Retrieves a map of all parameters and their current values.
     *
     * @returns Map from Parameters enum values to their current values.
     *
     * @throws MagicIsClosed If the Magic instance is closed.
     *
     * @code{.cpp}
     * auto params = magic.GetParameters();
     * for (const auto& [param, value] : params) {
     *     std::println("{}: {}", ToString(param), value);
     * }
     * @endcode
     *
     * @see ParameterValueMapT
     * @see SetParameters()
     *
     * @since 10.0.0
     */
    [[nodiscard]] ParameterValueMapT GetParameters() const;

    /**
     * @brief Get all parameter values (noexcept version).
     *
     * @param[in] tag Pass `std::nothrow` to select this overload.
     *
     * @returns Map from Parameters to values, or `std::nullopt` if closed.
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::optional<ParameterValueMapT> GetParameters(
        const std::nothrow_t& tag
    ) const noexcept;

    /** @} parameter_management */

    /**
     * @defgroup static_utilities Static Utilities
     * @ingroup magic_core
     * @brief Static utility methods.
     *
     * Class-level methods that don't require an instance.
     *
     * @{
     */

    /**
     * @brief Get the libmagic library version.
     *
     * Returns the version number of the underlying libmagic library.
     *
     * @returns Version string in format "X.YY" (e.g., "5.45").
     *
     * @code{.cpp}
     * std::println("libmagic version: {}", Magic::GetVersion());
     * @endcode
     *
     * @note This is a static method - no Magic instance is required.
     *
     * @since 10.0.0
     */
    [[nodiscard]] static std::string GetVersion() noexcept;

    /** @} static_utilities */

    /**
     * @defgroup file_identification File Identification
     * @ingroup magic_core
     * @brief Core file type detection methods.
     *
     * Methods for identifying file types using magic number analysis.
     *
     * @{
     */

    /**
     * @brief Identify the type of a single file.
     *
     * Analyzes the content of the specified file and returns its type
     * based on magic number analysis.
     *
     * @param[in] path Path to the file to identify.
     *
     * @returns File type string (format depends on configured Flags).
     *
     * @throws MagicIsClosed          If the Magic instance is closed.
     * @throws MagicDatabaseNotLoaded If no database is loaded.
     * @throws EmptyPath              If path is empty.
     * @throws PathDoesNotExist       If path does not exist.
     * @throws MagicIdentifyFileError If identification fails.
     *
     * @code{.cpp}
     * Magic magic{Magic::Flags::Mime};
     * auto type = magic.IdentifyFile("/etc/passwd");
     * // type = "text/plain; charset=us-ascii"
     * @endcode
     *
     * @see IdentifyFiles() for batch identification
     *
     * @since 10.0.0
     */
    [[nodiscard]] FileTypeT IdentifyFile(
        const std::filesystem::path& path
    ) const;

    /**
     * @brief Identify the type of a single file (noexcept version).
     *
     * @param[in] path Path to the file to identify.
     * @param[in] tag  Pass `std::nothrow` to select this overload.
     *
     * @returns ExpectedFileTypeT containing the file type or error message.
     *
     * @code{.cpp}
     * auto result = magic.IdentifyFile("/path/to/file", std::nothrow);
     * if (result) {
     *     std::println("Type: {}", *result);
     * } else {
     *     std::println("Error: {}", result.error());
     * }
     * @endcode
     *
     * @see ExpectedFileTypeT
     *
     * @since 10.0.0
     */
    [[nodiscard]] ExpectedFileTypeT IdentifyFile(
        const std::filesystem::path& path,
        const std::nothrow_t&        tag
    ) const noexcept;

    /**
     * @brief Identify all files in a directory.
     *
     * Recursively scans a directory and identifies the type of each regular file.
     *
     * @param[in] directory Path to the directory to scan.
     * @param[in] option    Directory iteration options (default: follow_directory_symlink).
     *
     * @returns Map from file paths to their identified types.
     *
     * @throws MagicIsClosed          If the Magic instance is closed.
     * @throws MagicDatabaseNotLoaded If no database is loaded.
     * @throws EmptyPath              If directory path is empty.
     * @throws PathDoesNotExist       If directory does not exist.
     * @throws PathIsNotDirectory     If path is not a directory.
     * @throws FilesystemError        If filesystem operation fails.
     * @throws MagicIdentifyFileError If identification fails for any file.
     *
     * @code{.cpp}
     * auto results = magic.IdentifyFiles("/path/to/directory");
     * for (const auto& [path, type] : results) {
     *     std::println("{} -> {}", path.string(), type);
     * }
     * @endcode
     *
     * @see FileTypeMapT
     *
     * @since 10.0.0
     */
    [[nodiscard]] FileTypeMapT IdentifyFiles(
        const std::filesystem::path&       directory,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const
    {
        return IdentifyDirectoryImpl(directory, option);
    }

    /**
     * @brief Identify all files in a directory with progress tracking.
     *
     * Same as IdentifyFiles(directory, option) but with progress monitoring.
     *
     * @param[in]  directory        Path to the directory to scan.
     * @param[out] progress_tracker Shared progress tracker (must not be null).
     * @param[in]  option           Directory iteration options.
     *
     * @returns Map from file paths to their identified types.
     *
     * @throws NullTracker If progress_tracker is null.
     * @throws ... Same exceptions as IdentifyFiles(directory, option).
     *
     * @code{.cpp}
     * using namespace std::chrono_literals;
     *
     * auto tracker = Utility::MakeSharedProgressTracker();
     *
     * // Start identification in another thread
     * auto future = std::async([&magic, tracker] {
     *     return magic.IdentifyFiles("/large/directory", tracker);
     * });
     *
     * // Monitor progress
     * while (!tracker->IsCompleted()) {
     *     std::println("Progress: {}", tracker->GetCompletionPercentage().ToString());
     *     std::this_thread::sleep_for(100ms);
     * }
     * @endcode
     *
     * @see ProgressTrackerT
     * @see Utility::MakeSharedProgressTracker()
     *
     * @since 10.0.0
     */
    [[nodiscard]] FileTypeMapT IdentifyFiles(
        const std::filesystem::path&       directory,
        ProgressTrackerT                   progress_tracker,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const
    {
        return IdentifyDirectoryImpl(directory, option, progress_tracker);
    }

    /**
     * @brief Identify all files in a directory (noexcept version).
     *
     * @param[in] directory Path to the directory to scan.
     * @param[in] tag       Pass `std::nothrow` to select this overload.
     * @param[in] option    Directory iteration options.
     *
     * @returns Map from file paths to expected results (type or error per file).
     *
     * @see ExpectedFileTypeMapT
     *
     * @since 10.0.0
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyFiles(
        const std::filesystem::path&       directory,
        const std::nothrow_t&              tag,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const noexcept
    {
        return IdentifyDirectoryImpl(directory, tag, option);
    }

    /**
     * @brief Identify all files in a directory with progress tracking (noexcept version).
     *
     * @param[in]  directory        Path to the directory to scan.
     * @param[in]  tag              Pass `std::nothrow` to select this overload.
     * @param[out] progress_tracker Shared progress tracker (must not be null).
     * @param[in]  option           Directory iteration options.
     *
     * @returns Map from file paths to expected results (type or error per file).
     *
     * @since 10.0.0
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyFiles(
        const std::filesystem::path&       directory,
        const std::nothrow_t&              tag,
        ProgressTrackerT                   progress_tracker,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const noexcept
    {
        return IdentifyDirectoryImpl(directory, tag, option, progress_tracker);
    }

    /**
     * @brief Identify multiple files from a container.
     *
     * Identifies the type of each file in the provided container.
     *
     * @param[in] files Container of file paths to identify (must satisfy Utility::FileContainer concept).
     *
     * @returns Map from file paths to their identified types.
     *
     * @throws MagicIsClosed          If the Magic instance is closed.
     * @throws MagicDatabaseNotLoaded If no database is loaded.
     * @throws EmptyPath              If any file path is empty.
     * @throws PathDoesNotExist       If any file does not exist.
     * @throws MagicIdentifyFileError If identification fails for any file.
     *
     * @code{.cpp}
     * std::vector<std::filesystem::path> files = {
     *     "/path/to/file1.txt",
     *     "/path/to/file2.png"
     * };
     * auto results = magic.IdentifyFiles(files);
     * @endcode
     *
     * @see Utility::FileContainer
     *
     * @since 10.0.0
     */
    [[nodiscard]] FileTypeMapT IdentifyFiles(
        const Utility::FileContainer auto& files
    ) const
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)}
        );
    }

    /**
     * @brief Identify multiple files with progress tracking.
     *
     * @param[in]  files            Container of file paths to identify.
     * @param[out] progress_tracker Shared progress tracker (must not be null).
     *
     * @returns Map from file paths to their identified types.
     *
     * @throws NullTracker If progress_tracker is null.
     * @throws ... Same exceptions as IdentifyFiles(files).
     *
     * @since 10.0.0
     */
    [[nodiscard]] FileTypeMapT IdentifyFiles(
        const Utility::FileContainer auto& files,
        ProgressTrackerT                   progress_tracker
    ) const
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)},
            progress_tracker
        );
    }

    /**
     * @brief Identify multiple files from a container (noexcept version).
     *
     * @param[in] files Container of file paths to identify.
     * @param[in] tag   Pass `std::nothrow` to select this overload.
     *
     * @returns Map from file paths to expected results (type or error per file).
     *
     * @since 10.0.0
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyFiles(
        const Utility::FileContainer auto& files,
        const std::nothrow_t&              tag
    ) const noexcept
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)},
            tag
        );
    }

    /**
     * @brief Identify multiple files with progress tracking (noexcept version).
     *
     * @param[in]  files            Container of file paths to identify.
     * @param[in]  tag              Pass `std::nothrow` to select this overload.
     * @param[out] progress_tracker Shared progress tracker (must not be null).
     *
     * @returns Map from file paths to expected results (type or error per file).
     *
     * @since 10.0.0
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyFiles(
        const Utility::FileContainer auto& files,
        const std::nothrow_t&              tag,
        ProgressTrackerT                   progress_tracker
    ) const noexcept
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)},
            tag,
            progress_tracker
        );
    }

    /** @} file_identification */

    /**
     * @defgroup state_queries State Queries
     * @ingroup magic_core
     * @brief Instance state inspection methods.
     *
     * Methods for querying the current lifecycle state.
     *
     * @{
     */

    /**
     * @brief Check if a magic database is loaded.
     *
     * @returns `true` if a database is loaded (instance in **Valid** state),
     *         `false` otherwise (**Closed** or **Opened** state).
     *
     * @par Lifecycle State Check
     * | State | Returns |
     * |-------|--------|
     * | Closed | `false` |
     * | Opened | `false` |
     * | Valid | `true` |
     *
     * @see @ref Magic Core "Magic Lifecycle" for state diagram
     * @see LoadDatabaseFile()
     * @see IsOpen()
     * @see IsValid()
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool IsDatabaseLoaded() const noexcept;

    /**
     * @brief Check if the Magic instance is open.
     *
     * @returns `true` if open (**Opened** or **Valid** state),
     *         `false` if **Closed**.
     *
     * @par Lifecycle State Check
     * | State | Returns |
     * |-------|--------|
     * | Closed | `false` |
     * | Opened | `true` |
     * | Valid | `true` |
     *
     * @note An open instance may still not be valid for identification
     *       if no database is loaded. Use IsValid() to check both.
     *
     * @see @ref Magic Core "Magic Lifecycle" for state diagram
     * @see Open()
     * @see Close()
     * @see IsValid()
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool IsOpen() const noexcept;

    /**
     * @brief Check if the Magic instance is valid for file identification.
     *
     * An instance is valid if it is both open and has a database loaded,
     * i.e., it is in the **Valid** state.
     *
     * @returns `true` if in **Valid** state (open and database loaded),
     *         `false` if in **Closed** or **Opened** state.
     *
     * @par Lifecycle State Check
     * | State | Returns |
     * |-------|--------|
     * | Closed | `false` |
     * | Opened | `false` |
     * | Valid | `true` |
     *
     * @code{.cpp}
     * Magic magic{Magic::Flags::Mime, std::nothrow};
     * if (magic.IsValid()) {
     *     auto type = magic.IdentifyFile("file.txt");
     * }
     * @endcode
     *
     * @see @ref Magic Core "Magic Lifecycle" for state diagram
     * @see IsOpen()
     * @see IsDatabaseLoaded()
     * @see operator bool()
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool IsValid() const noexcept;

    /** @} state_queries */

    /**
     * @defgroup database_loading Database Loading
     * @ingroup magic_core
     * @brief Magic database loading methods.
     *
     * Methods for loading magic database files to enable identification.
     *
     * @{
     */

    /**
     * @brief Load a magic database file.
     *
     * Loads the specified magic database file into the Magic instance.
     * Must be called after Open() before file identification can be performed.
     *
     * @param[in] database_file Path to the database file (default: DEFAULT_DATABASE_FILE).
     *
     * @throws MagicIsClosed              If the Magic instance is closed.
     * @throws EmptyPath                  If database_file path is empty.
     * @throws PathDoesNotExist           If database_file does not exist.
     * @throws PathIsNotRegularFile       If database_file is not a regular file.
     * @throws MagicLoadDatabaseFileError If loading fails.
     *
     * @code{.cpp}
     * Magic magic;
     * magic.Open(Magic::Flags::Mime);
     * magic.LoadDatabaseFile();  // Load default database
     *
     * // Or load custom database
     * magic.LoadDatabaseFile("/custom/magic");
     * @endcode
     *
     * @note The ".mgc" extension is automatically appended if appropriate.
     *
     * @see Open()
     * @see IsDatabaseLoaded()
     *
     * @since 10.0.0
     */
    void LoadDatabaseFile(
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    );

    /**
     * @brief Load a magic database file (noexcept version).
     *
     * @param[in] tag           Pass `std::nothrow` to select this overload.
     * @param[in] database_file Path to the database file (default: DEFAULT_DATABASE_FILE).
     *
     * @returns `true` on success, `false` on failure.
     *
     * @note The ".mgc" extension is automatically appended if appropriate.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool LoadDatabaseFile(
        const std::nothrow_t&        tag,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /** @} database_loading */

    /**
     * @defgroup open_reopen Open Reopen
     * @ingroup magic_core
     * @brief Instance initialization methods.
     *
     * Methods for opening and reopening the Magic instance.
     *
     * @{
     */

    /**
     * @brief Open Magic with specified flags.
     *
     * Opens (or reopens) the Magic instance with the specified configuration flags.
     * If already open, the instance is closed first then reopened.
     *
     * @par Lifecycle Transition
     * - From **Closed** → **Opened**
     * - From **Opened** → **Opened** (reopens with new flags)
     * - From **Valid** → **Opened** (database unloaded, must reload)
     *
     * @param[in] flags_mask Configuration flags (use Flags enum values combined with |).
     *
     * @throws MagicOpenError If opening fails.
     *
     * @code{.cpp}
     * Magic magic;
     * magic.Open(Magic::Flags::Mime | Magic::Flags::Compress);
     * magic.LoadDatabaseFile();
     * @endcode
     *
     * @note A database must be loaded after opening before identification works.
     * @warning Calling Open() on a **Valid** instance unloads the database.
     *          You must call LoadDatabaseFile() again.
     *
     * @see @ref Magic Core "Magic Lifecycle" for state diagram
     * @see Close()
     * @see LoadDatabaseFile()
     *
     * @since 10.0.0
     */
    void Open(FlagsMaskT flags_mask);

    /**
     * @brief Open Magic with specified flags (noexcept version).
     *
     * @param[in] flags_mask Configuration flags.
     * @param[in] tag        Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool Open(
        FlagsMaskT            flags_mask,
        const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Open Magic with a container of flags.
     *
     * @param[in] flags_container Container of Flags values.
     *
     * @throws MagicOpenError If opening fails.
     *
     * @since 10.0.0
     */
    void Open(const FlagsContainerT& flags_container);

    /**
     * @brief Open Magic with a container of flags (noexcept version).
     *
     * @param[in] flags_container Container of Flags values.
     * @param[in] tag             Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool Open(
        const FlagsContainerT& flags_container,
        const std::nothrow_t&  tag
    ) noexcept;

    /** @} open_reopen */

    /**
     * @defgroup flag_modification Flag Modification
     * @ingroup magic_core
     * @brief Flag modification methods.
     *
     * Methods for changing configuration flags after opening.
     *
     * @{
     */

    /**
     * @brief Set new flags for the Magic instance.
     *
     * Changes the configuration flags for an already-open Magic instance.
     *
     * @param[in] flags_mask New configuration flags.
     *
     * @throws MagicIsClosed      If the Magic instance is closed.
     * @throws MagicSetFlagsError If setting flags fails.
     *
     * @code{.cpp}
     * magic.SetFlags(Magic::Flags::MimeType);  // Change to MIME type only
     * @endcode
     *
     * @see GetFlags()
     *
     * @since 10.0.0
     */
    void SetFlags(FlagsMaskT flags_mask);

    /**
     * @brief Set new flags (noexcept version).
     *
     * @param[in] flags_mask New configuration flags.
     * @param[in] tag        Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool SetFlags(
        FlagsMaskT            flags_mask,
        const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set new flags from a container.
     *
     * @param[in] flags_container Container of Flags values.
     *
     * @throws MagicIsClosed      If the Magic instance is closed.
     * @throws MagicSetFlagsError If setting flags fails.
     *
     * @since 10.0.0
     */
    void SetFlags(const FlagsContainerT& flags_container);

    /**
     * @brief Set new flags from a container (noexcept version).
     *
     * @param[in] flags_container Container of Flags values.
     * @param[in] tag             Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool SetFlags(
        const FlagsContainerT& flags_container,
        const std::nothrow_t&  tag
    ) noexcept;

    /** @} flag_modification */

    /**
     * @defgroup parameter_modification Parameter Modification
     * @ingroup magic_core
     * @brief Parameter modification methods.
     *
     * Methods for changing libmagic parameter values.
     *
     * @{
     */

    /**
     * @brief Set a single parameter value.
     *
     * @param[in] parameter The parameter to modify.
     * @param[in] value     The new value for the parameter.
     *
     * @throws MagicIsClosed          If the Magic instance is closed.
     * @throws MagicSetParameterError If setting the parameter fails.
     *
     * @code{.cpp}
     * // Limit file scanning to 1MB for performance
     * magic.SetParameter(Magic::Parameters::BytesMax, 1024 * 1024);
     * @endcode
     *
     * @see Parameters
     * @see GetParameter()
     *
     * @since 10.0.0
     */
    void SetParameter(Parameters parameter, std::size_t value);

    /**
     * @brief Set a single parameter value (noexcept version).
     *
     * @param[in] parameter The parameter to modify.
     * @param[in] value     The new value for the parameter.
     * @param[in] tag       Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool SetParameter(
        Parameters            parameter,
        std::size_t           value,
        const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set multiple parameter values.
     *
     * @param[in] parameters Map of parameters to their new values.
     *
     * @throws MagicIsClosed          If the Magic instance is closed.
     * @throws MagicSetParameterError If setting any parameter fails.
     *
     * @code{.cpp}
     * magic.SetParameters({
     *     {Magic::Parameters::BytesMax, 1024 * 1024},
     *     {Magic::Parameters::RegexMax, 4096}
     * });
     * @endcode
     *
     * @see ParameterValueMapT
     * @see GetParameters()
     *
     * @since 10.0.0
     */
    void SetParameters(const ParameterValueMapT& parameters);

    /**
     * @brief Set multiple parameter values (noexcept version).
     *
     * @param[in] parameters Map of parameters to their new values.
     * @param[in] tag        Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool SetParameters(
        const ParameterValueMapT& parameters,
        const std::nothrow_t&     tag
    ) noexcept;

    /** @} parameter_modification */

private:
    /** @brief Forward declaration of the implementation class (Pimpl idiom). */
    class MagicPrivate;

    /** @brief Pointer to the implementation (Pimpl idiom). */
    std::unique_ptr<MagicPrivate> m_impl;

    /** @brief Default container type for file paths used in implementation. */
    using DefaultFileContainerT = std::vector<std::filesystem::path>;

    /**
     * @brief Implementation for directory identification (throwing version).
     *
     * @param[in] directory         Path to the directory to identify.
     * @param[in] option            Directory iteration options.
     * @param[in] progress_tracker  Optional tracker for reporting progress.
     *
     * @returns Map of file paths to their identified types.
     */
    [[nodiscard]] FileTypeMapT IdentifyDirectoryImpl(
        const std::filesystem::path&       directory,
        std::filesystem::directory_options option,
        ProgressTrackerT progress_tracker = Utility::MakeSharedProgressTracker()
    ) const;

    /**
     * @brief Implementation for directory identification (noexcept version).
     *
     * @param[in] directory         Path to the directory to identify.
     * @param[in] tag               Tag to select the noexcept overload.
     * @param[in] option            Directory iteration options.
     * @param[in] progress_tracker  Optional tracker for reporting progress.
     *
     * @returns Expected map of file paths to their identified types, or error.
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyDirectoryImpl(
        const std::filesystem::path&       directory,
        const std::nothrow_t&              tag,
        std::filesystem::directory_options option,
        ProgressTrackerT progress_tracker = Utility::MakeSharedProgressTracker()
    ) const noexcept;

    /**
     * @brief Implementation for container identification (throwing version).
     *
     * @param[in] files             Container of file paths to identify.
     * @param[in] progress_tracker  Optional tracker for reporting progress.
     *
     * @returns Map of file paths to their identified types.
     */
    [[nodiscard]] FileTypeMapT IdentifyContainerImpl(
        const DefaultFileContainerT& files,
        ProgressTrackerT progress_tracker = Utility::MakeSharedProgressTracker()
    ) const;

    /**
     * @brief Implementation for container identification (noexcept version).
     *
     * @param[in] files             Container of file paths to identify.
     * @param[in] tag               Tag to select the noexcept overload.
     * @param[in] progress_tracker  Optional tracker for reporting progress.
     *
     * @returns Expected map of file paths to their identified types, or error.
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyContainerImpl(
        const DefaultFileContainerT& files,
        const std::nothrow_t&        tag,
        ProgressTrackerT progress_tracker = Utility::MakeSharedProgressTracker()
    ) const noexcept;

    /** @brief Friend declaration for ToString(Flags) free function. */
    friend std::string ToString(Flags);

    /** @brief Friend declaration for ToString(Parameters) free function. */
    friend std::string ToString(Parameters);
};

/**
 * @defgroup magic_to_string String Conversion Functions
 * @ingroup magic_core
 * @brief Free functions for converting Magic types to human-readable strings.
 *
 * These functions provide convenient string representations of Magic's
 * result types for logging, debugging, and display purposes.
 *
 * @{
 */

/**
 * @brief Convert a file type entry to a string.
 * @ingroup magic_to_string
 *
 * Formats a single file path and its identified type as a string.
 *
 * @param[in] file_type_entry The file type entry (path -> type pair).
 * @param[in] type_separator  Separator between path and type (default: " -> ").
 *
 * @returns Formatted string: "path -> type".
 *
 * @code{.cpp}
 * Magic::FileTypeEntryT entry = {"/path/to/file.txt", "text/plain"};
 * std::println("{}", ToString(entry));
 * // Output: /path/to/file.txt -> text/plain
 * @endcode
 *
 * @see Magic::FileTypeEntryT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::FileTypeEntryT& file_type_entry,
    const std::string&           type_separator = " -> "
);

/**
 * @brief Convert a file type map to a string.
 * @ingroup magic_to_string
 *
 * Formats all file paths and their identified types as a multi-line string.
 *
 * @param[in] file_type_map   Map of file paths to their types.
 * @param[in] type_separator  Separator between path and type (default: " -> ").
 * @param[in] file_separator  Separator between entries (default: "\\n").
 *
 * @returns Formatted string with all entries.
 *
 * @code{.cpp}
 * auto results = magic.IdentifyFiles(files);
 * std::println("{}", ToString(results));
 * // Output:
 * // /path/to/file1.txt -> text/plain
 * // /path/to/image.png -> image/png
 * @endcode
 *
 * @see Magic::FileTypeMapT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::FileTypeMapT& file_type_map,
    const std::string&         type_separator = " -> ",
    const std::string&         file_separator = "\n"
);

/**
 * @brief Convert an expected file type result to a string.
 * @ingroup magic_to_string
 *
 * Returns the file type if successful, or the error message if failed.
 *
 * @param[in] expected_file_type The expected result from noexcept identification.
 *
 * @returns The file type string on success, or error message on failure.
 *
 * @code{.cpp}
 * auto result = magic.IdentifyFile("/path/to/file", std::nothrow);
 * std::println("{}", ToString(result));
 * @endcode
 *
 * @see Magic::ExpectedFileTypeT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::ExpectedFileTypeT& expected_file_type
);

/**
 * @brief Convert an expected file type entry to a string.
 * @ingroup magic_to_string
 *
 * Formats a file path and its expected result (type or error) as a string.
 *
 * @param[in] expected_file_type_entry The file path and expected result pair.
 * @param[in] type_separator           Separator between path and result (default: " -> ").
 *
 * @returns Formatted string: "path -> type" or "path -> [error message]".
 *
 * @see Magic::ExpectedFileTypeEntryT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::ExpectedFileTypeEntryT& expected_file_type_entry,
    const std::string&                   type_separator = " -> "
);

/**
 * @brief Convert an expected file type map to a string.
 * @ingroup magic_to_string
 *
 * Formats all file paths and their expected results as a multi-line string.
 *
 * @param[in] expected_file_type_map Map of file paths to expected results.
 * @param[in] type_separator         Separator between path and result (default: " -> ").
 * @param[in] file_separator         Separator between entries (default: "\\n").
 *
 * @returns Formatted string with all entries.
 *
 * @see Magic::ExpectedFileTypeMapT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::ExpectedFileTypeMapT& expected_file_type_map,
    const std::string&                 type_separator = " -> ",
    const std::string&                 file_separator = "\n"
);

/**
 * @brief Convert a Magic flag to its string name.
 * @ingroup magic_to_string
 *
 * Returns the symbolic name of a single flag value.
 *
 * @param[in] flag The flag to convert.
 *
 * @returns String name of the flag (e.g., "Mime", "MimeType").
 *
 * @code{.cpp}
 * std::println("{}", ToString(Magic::Flags::Mime));
 * // Output: Mime
 * @endcode
 *
 * @see Magic::Flags
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(Magic::Flags flag);

/**
 * @brief Convert a container of flags to a string.
 * @ingroup magic_to_string
 *
 * Formats multiple flags as a comma-separated list.
 *
 * @param[in] flags     Container of flags to convert.
 * @param[in] separator Separator between flags (default: ", ").
 *
 * @returns Formatted string of flag names.
 *
 * @code{.cpp}
 * Magic::FlagsContainerT flags = {Magic::Flags::Mime, Magic::Flags::Compress};
 * std::println("{}", ToString(flags));
 * // Output: Mime, Compress
 * @endcode
 *
 * @see Magic::FlagsContainerT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::FlagsContainerT& flags,
    const std::string&            separator = ", "
);

/**
 * @brief Convert a Magic parameter to its string name.
 * @ingroup magic_to_string
 *
 * Returns the symbolic name of a single parameter value.
 *
 * @param[in] parameter The parameter to convert.
 *
 * @returns String name of the parameter (e.g., "BytesMax", "RegexMax").
 *
 * @code{.cpp}
 * std::println("{}", ToString(Magic::Parameters::BytesMax));
 * // Output: BytesMax
 * @endcode
 *
 * @see Magic::Parameters
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(Magic::Parameters parameter);

/**
 * @brief Convert a parameter-value pair to a string.
 * @ingroup magic_to_string
 *
 * Formats a parameter and its value as "name: value".
 *
 * @param[in] parameter_value The parameter and its value.
 * @param[in] value_separator Separator between name and value (default: ": ").
 *
 * @returns Formatted string: "ParameterName: value".
 *
 * @code{.cpp}
 * Magic::ParameterValueT pv = {Magic::Parameters::BytesMax, 1048576};
 * std::println("{}", ToString(pv));
 * // Output: BytesMax: 1048576
 * @endcode
 *
 * @see Magic::ParameterValueT
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::ParameterValueT& parameter_value,
    const std::string&            value_separator = ": "
);

/**
 * @brief Convert a parameter-value map to a string.
 * @ingroup magic_to_string
 *
 * Formats all parameters and their values as a formatted list.
 *
 * @param[in] parameters          Map of parameters to values.
 * @param[in] value_separator     Separator between name and value (default: ": ").
 * @param[in] parameter_separator Separator between entries (default: ", ").
 *
 * @returns Formatted string of all parameter-value pairs.
 *
 * @code{.cpp}
 * auto params = magic.GetParameters();
 * std::println("{}", ToString(params));
 * // Output: BytesMax: 1048576, RegexMax: 8192, ...
 * @endcode
 *
 * @see Magic::ParameterValueMapT
 * @see Magic::GetParameters()
 *
 * @since 10.0.0
 */
[[nodiscard]] std::string ToString(
    const Magic::ParameterValueMapT& parameters,
    const std::string&               value_separator     = ": ",
    const std::string&               parameter_separator = ", "
);

/** @} magic_to_string */
} /* namespace Recognition */

#endif /* MAGIC_HPP */
