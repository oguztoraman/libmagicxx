/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic.cpp
 * @brief Implementation of the Magic class and supporting utilities.
 *
 * This file contains the complete implementation of the libmagicxx library,
 * including:
 * - Magic::MagicPrivate (PIMPL implementation class)
 * - Magic class public method implementations
 * - ToString() free function overloads for string conversion
 *
 * @section magic_cpp_architecture Architecture
 *
 * The implementation uses the PIMPL (Pointer to IMPLementation) idiom to:
 * - Hide implementation details from the public header
 * - Minimize compilation dependencies (libmagic headers are internal)
 * - Enable ABI stability across library versions
 *
 * @section magic_cpp_libmagic libmagic Integration
 *
 * The underlying libmagic C library is wrapped in the `Detail` namespace
 * to isolate its symbols. All libmagic calls go through MagicPrivate, which:
 * - Manages the magic cookie (libmagic handle) via RAII
 * - Converts between C++23 types and C types
 * - Translates libmagic error codes to exceptions or `std::expected`
 *
 * @section magic_cpp_components Key Components
 *
 * | Component | Purpose |
 * |-----------|---------|
 * | `Detail::magic_*` | Wrapped libmagic C functions |
 * | `MagicPrivate` | PIMPL class holding cookie and state |
 * | `MagicPrivate::FlagsConverter` | Converts between flag representations |
 * | `MagicPrivate::LibmagicPairConverter` | Extracts values from flag/param pairs |
 * | `MagicPrivate::ThrowExceptionOnFailure` | Error handling template |
 * | `ToString()` overloads | String conversion utilities |
 *
 * @section magic_cpp_lifecycle Lifecycle Implementation
 *
 * The three-state lifecycle (Closed -> Opened -> Valid) is tracked by:
 * - `m_cookie`: nullptr means Closed, non-null means Opened
 * - `m_is_database_loaded`: true only after successful LoadDatabaseFile()
 * - `IsValid()` = `IsOpen() && IsDatabaseLoaded()`
 *
 * @see magic.hpp for the public API documentation
 * @see Magic class documentation for lifecycle state diagram
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 */

#include <cmath>

#include <array>
#include <utility>

#include "magic.hpp"

namespace Recognition {
/**
 * @defgroup magic_implementation Implementation Details
 * @ingroup magic_core
 * @brief Internal implementation details for the Magic class.
 *
 * This group contains implementation-specific types, constants, and
 * helper classes used by the Magic class PIMPL implementation.
 *
 * ### Quick Start (Internal Usage)
 *
 * @code{.cpp}
 * // MagicPrivate is accessed only through the Magic public interface
 * Magic magic{Magic::Flags::Mime};
 *
 * // Internally, Magic delegates to MagicPrivate:
 * // magic.IdentifyFile(path) -> m_impl->IdentifyFile(path)
 * auto type = magic.IdentifyFile("/path/to/file");
 * @endcode
 *
 * ### Error Handling
 *
 * Internal methods use a template-based approach for consistent error handling:
 * - **ThrowExceptionOnFailure**: Validates preconditions and throws on failure
 * - **GetErrorMessage()**: Retrieves libmagic error strings
 *
 * @code{.cpp}
 * // Throwing version pattern
 * void MagicPrivate::SomeMethod() {
 *     ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
 *     auto result = Detail::magic_operation(m_cookie.get());
 *     ThrowExceptionOnFailure<MagicOperationError>(result, GetErrorMessage());
 * }
 *
 * // Noexcept version pattern
 * std::expected<T, E> MagicPrivate::SomeMethod(std::nothrow_t) noexcept {
 *     if (!IsOpen()) {
 *         return std::unexpected{"Magic is closed"};
 *     }
 *     // ... operation ...
 * }
 * @endcode
 */
/**
 * @namespace Recognition::Detail
 * @ingroup magic_implementation
 * @brief Internal namespace for libmagic C library integration.
 *
 * This namespace isolates the libmagic C header to prevent symbol
 * pollution in client code. All libmagic functions are accessed
 * through this namespace within the implementation.
 */
namespace Detail {
#include <magic.h>
} /* namespace Detail */

/**
 * @class Magic::MagicPrivate
 * @ingroup magic_implementation
 * @brief PIMPL implementation class for Magic.
 *
 * MagicPrivate encapsulates all libmagic interactions and maintains
 * the internal state of a Magic instance. This class is the heart of
 * the implementation, providing:
 *
 * - **Cookie Management**: RAII wrapper for `magic_t` handle
 * - **Flag Conversion**: Bidirectional conversion between C++ and C flags
 * - **Error Handling**: Template-based exception throwing
 * - **State Tracking**: `m_cookie` and `m_is_database_loaded` flags
 *
 * @section magic_private_design Design Patterns
 *
 * ### PIMPL Pattern
 *
 * MagicPrivate implements the Pointer to IMPLementation (PIMPL) idiom:
 *
 * @code{.cpp}
 * // In magic.hpp (public header)
 * class Magic {
 *     class MagicPrivate;                        // Forward declaration only
 *     std::unique_ptr<MagicPrivate> m_impl;      // Pointer to implementation
 * };
 *
 * // In magic.cpp (implementation file)
 * class Magic::MagicPrivate { ... };  // Full definition here
 * @endcode
 *
 * This provides:
 * - **Compilation Firewall**: Changes to MagicPrivate don't recompile clients
 * - **ABI Stability**: Internal changes don't break binary compatibility
 * - **Header Isolation**: libmagic headers stay internal
 *
 * ### RAII Cookie Management
 *
 * The libmagic handle (`magic_t`) is wrapped in a `std::unique_ptr` with
 * a custom deleter that calls `magic_close()`:
 *
 * @code{.cpp}
 * using CookieT = std::unique_ptr<
 *     Detail::magic_set,
 *     decltype([](Detail::magic_t cookie) noexcept {
 *         Detail::magic_close(cookie);
 *     })
 * >;
 * @endcode
 *
 * @section magic_private_members Member Variables
 *
 * | Member | Type | Purpose |
 * |--------|------|--------|
 * | `m_cookie` | `CookieT` | RAII-managed libmagic handle (nullptr = Closed) |
 * | `m_flags_mask` | `FlagsMaskT` | Current configuration flags bitmask |
 * | `m_is_database_loaded` | `bool` | True only after successful LoadDatabaseFile() |
 *
 * @section magic_private_converters Converter Classes
 *
 * | Converter | Purpose |
 * |-----------|---------|
 * | `FlagsConverter` | Converts between FlagsContainerT, FlagsMaskT, and libmagic int |
 * | `LibmagicPairConverter` | Extracts int or string from libmagic constant pairs |
 *
 * @section magic_private_constants Constants
 *
 * | Constant | Value | Purpose |
 * |----------|-------|--------|
 * | `LIBMAGIC_ERROR` | -1 | libmagic error return value |
 * | `LIBMAGIC_FLAGS_COUNT` | 30 | Number of supported flags |
 * | `LIBMAGIC_PARAMETER_COUNT` | 10 | Number of tunable parameters |
 * | `LIBMAGIC_FLAGS` | array | Maps Flags enum bit positions to libmagic constants |
 * | `LIBMAGIC_PARAMETERS` | array | Maps Parameters enum to libmagic constants |
 *
 * @section magic_private_error_handling Error Handling
 *
 * The `ThrowExceptionOnFailure` template provides centralized error handling:
 *
 * @code{.cpp}
 * // Throws MagicIsClosed if IsOpen() returns false
 * ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
 *
 * // Throws MagicOpenError with error message if result is -1
 * ThrowExceptionOnFailure<MagicOpenError>(result, GetErrorMessage());
 * @endcode
 *
 * @section magic_private_lifecycle Lifecycle Management
 *
 * The three-state lifecycle is tracked by member variables:
 *
 * | State | m_cookie | m_is_database_loaded | IsValid() |
 * |-------|----------|---------------------|----------|
 * | **Closed** | `nullptr` | `false` | `false` |
 * | **Opened** | valid ptr | `false` | `false` |
 * | **Valid** | valid ptr | `true` | `true` |
 *
 * @note MagicPrivate is move-only (copy operations deleted).
 * @note The destructor automatically releases resources via CookieT.
 *
 * @see Magic for the public interface
 * @see magic.hpp for public API documentation
 *
 * @since 10.0.0
 */
class Magic::MagicPrivate {
public:
    /**
     * @brief Bitmask type for file identification validation options.
     *
     * A 3-bit bitset used to control which precondition checks are
     * performed before file identification operations.
     *
     * @see IdentifyFileOptions
     * @see IdentifyFile()
     */
    using IdentifyFileOptionsMaskT = std::bitset<3uz>;

    /**
     * @brief Options controlling file identification precondition checks.
     *
     * These options determine which validation checks are performed
     * before attempting to identify a file. They can be combined
     * using bitwise OR to enable multiple checks.
     *
     * ### Validation Flow
     *
     * @code{.cpp}
     * // Example validation sequence with CheckEverything:
     * // 1. CheckIsValid -> Verify Magic instance is valid
     * // 2. CheckPathEmpty -> Verify path string is non-empty
     * // 3. CheckPathExists -> Verify file exists on disk
     * auto type = IdentifyFile(path, CheckEverything);
     * @endcode
     *
     * ### Option Combinations
     *
     * | Option | Checks |
     * |--------|--------||
     * | `CheckNothing` | None - skip all validation |
     * | `CheckIsValid` | Magic instance validity |
     * | `CheckPathEmpty` | Path string non-empty |
     * | `CheckPathExists` | File exists on disk |
     * | `CheckPath` | CheckPathEmpty + CheckPathExists |
     * | `CheckEverything` | CheckIsValid + CheckPath |
     *
     * @see IdentifyFile()
     * @see IdentifyFiles()
     */
    enum IdentifyFileOptions : unsigned long long {
        /* clang-format off */
        CheckNothing    = 0ULL,                              /**< Skip all checks */
        CheckIsValid    = 1ULL << 0,                         /**< Verify instance is valid */
        CheckPathEmpty  = 1ULL << 1,                         /**< Verify path is non-empty */
        CheckPathExists = 1ULL << 2,                         /**< Verify path exists on disk */
        CheckPath       = CheckPathEmpty | CheckPathExists,  /**< Check both path conditions */
        CheckEverything = CheckIsValid | CheckPath           /**< Perform all validations */
        /* clang-format on */
    };

    /**
     * @defgroup impl_constructors Constructors
     * @ingroup magic_implementation
     * @brief MagicPrivate construction methods.
     *
     * Constructors for creating MagicPrivate instances in various
     * initial states (closed, opened, or valid).
     *
     * @{
     */

    /**
     * @brief Default constructor creates a closed instance.
     *
     * Creates a MagicPrivate in the **Closed** state with:
     * - `m_cookie` = nullptr
     * - `m_flags_mask` = 0
     * - `m_is_database_loaded` = false
     *
     * @par Lifecycle State
     * After construction: **Closed**
     *
     * @see Open() to transition to Opened state
     */
    MagicPrivate() noexcept = default;

    /**
     * @brief Construct and initialize with flags (throwing version).
     *
     * Creates a MagicPrivate, opens it with the specified flags, and
     * loads the magic database file in a single step.
     *
     * @param[in] flags_mask    Configuration flags as a bitmask.
     * @param[in] database_file Path to magic database file to load.
     *
     * @throws MagicOpenError             If `magic_open()` fails.
     * @throws EmptyPath                  If database_file is empty.
     * @throws PathDoesNotExist           If database_file does not exist.
     * @throws PathIsNotRegularFile       If database_file is not a regular file.
     * @throws MagicLoadDatabaseFileError If `magic_load()` fails.
     *
     * @par Lifecycle Transition
     * **Closed** → **Opened** → **Valid**
     *
     * @par Implementation
     * @code{.cpp}
     * Open(flags_mask);            // Closed -> Opened
     * LoadDatabaseFile(database_file);  // Opened -> Valid
     * @endcode
     *
     * @see Magic::Magic(FlagsMaskT, const std::filesystem::path&)
     */
    MagicPrivate(
        FlagsMaskT                   flags_mask,
        const std::filesystem::path& database_file
    )
    {
        Open(flags_mask);
        LoadDatabaseFile(database_file);
    }

    /**
     * @brief Construct and initialize with flags (noexcept version).
     *
     * Non-throwing variant that silently fails on errors. The short-circuit
     * evaluation ensures LoadDatabaseFile() is only called if Open() succeeds.
     *
     * @param[in] flags_mask    Configuration flags as a bitmask.
     * @param[in] tag           Pass `std::nothrow` to select this overload.
     * @param[in] database_file Path to magic database file to load.
     *
     * @par Lifecycle State
     * After construction:
     * - **Valid** if both Open() and LoadDatabaseFile() succeed
     * - **Opened** if only Open() succeeds
     * - **Closed** if Open() fails
     *
     * @par Implementation
     * Uses short-circuit && to ensure proper sequencing:
     * @code{.cpp}
     * Open(flags_mask, std::nothrow)           // Only proceed if true
     * && LoadDatabaseFile(std::nothrow, database_file);
     * @endcode
     *
     * @note Check `IsValid()` after construction to verify success.
     *
     * @see Magic::Magic(FlagsMaskT, const std::nothrow_t&, const std::filesystem::path&)
     */
    MagicPrivate(
        FlagsMaskT                             flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path&           database_file
    ) noexcept
    {
        static_cast<void>(
            Open(flags_mask, std::nothrow)
            && LoadDatabaseFile(std::nothrow, database_file)
        );
    }

    /**
     * @brief Construct with flags container (throwing version).
     *
     * Alternative constructor accepting a container of individual Flags
     * values instead of a bitmask.
     *
     * @param[in] flags_container Container (vector) of Flags enum values.
     * @param[in] database_file   Path to magic database file to load.
     *
     * @throws MagicOpenError             If `magic_open()` fails.
     * @throws EmptyPath                  If database_file is empty.
     * @throws PathDoesNotExist           If database_file does not exist.
     * @throws PathIsNotRegularFile       If database_file is not a regular file.
     * @throws MagicLoadDatabaseFileError If `magic_load()` fails.
     *
     * @par Lifecycle Transition
     * **Closed** → **Opened** → **Valid**
     *
     * @par Usage Example
     * @code{.cpp}
     * FlagsContainerT flags = {Flags::Mime, Flags::Compress};
     * MagicPrivate impl{flags, "/path/to/magic"};
     * @endcode
     *
     * @see Magic::Magic(const FlagsContainerT&, const std::filesystem::path&)
     */
    MagicPrivate(
        const FlagsContainerT&       flags_container,
        const std::filesystem::path& database_file
    )
    {
        Open(flags_container);
        LoadDatabaseFile(database_file);
    }

    /**
     * @brief Construct with flags container (noexcept version).
     *
     * Non-throwing variant accepting a container of Flags values.
     * Silently fails if initialization encounters errors.
     *
     * @param[in] flags_container Container (vector) of Flags enum values.
     * @param[in] tag             Pass `std::nothrow` to select this overload.
     * @param[in] database_file   Path to magic database file to load.
     *
     * @par Lifecycle State
     * After construction:
     * - **Valid** if both Open() and LoadDatabaseFile() succeed
     * - **Opened** if only Open() succeeds
     * - **Closed** if Open() fails
     *
     * @note Check `IsValid()` after construction to verify success.
     *
     * @see Magic::Magic(const FlagsContainerT&, const std::nothrow_t&, const std::filesystem::path&)
     */
    MagicPrivate(
        const FlagsContainerT&                 flags_container,
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path&           database_file
    ) noexcept
    {
        static_cast<void>(
            Open(flags_container, std::nothrow)
            && LoadDatabaseFile(std::nothrow, database_file)
        );
    }

    /** @} impl_constructors */

    /**
     * @defgroup impl_special_member_functions Special Member Functions
     * @ingroup magic_implementation
     * @brief Move, copy, and destruction operations.
     *
     * Special member functions controlling MagicPrivate lifetime
     * and resource ownership transfer.
     *
     * @{
     */

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of all resources from the source instance. After
     * the move, the source is left in the **Closed** state (m_cookie = nullptr).
     *
     * @note The moved-from object is safe to destroy or reassign.
     *
     * @see Magic::Magic(Magic&&)
     */
    MagicPrivate(MagicPrivate&&) noexcept = default;

    /**
     * @brief Deleted copy constructor.
     *
     * MagicPrivate cannot be copied because each instance owns a unique
     * libmagic cookie handle. Use move semantics instead.
     *
     * @see Magic::Magic(const Magic&)
     */
    MagicPrivate(const MagicPrivate&) = delete;

    /**
     * @brief Move assignment operator.
     *
     * Releases any existing resources, then transfers ownership from the
     * source instance. After the move, the source is left in the **Closed** state.
     *
     * @returns Reference to this instance.
     *
     * @see Magic::operator=(Magic&&)
     */
    MagicPrivate& operator=(MagicPrivate&&) noexcept = default;

    /**
     * @brief Deleted copy assignment operator.
     *
     * MagicPrivate cannot be copied because each instance owns a unique
     * libmagic cookie handle. Use move semantics instead.
     *
     * @see Magic::operator=(const Magic&)
     */
    MagicPrivate& operator=(const MagicPrivate&) = delete;

    /**
     * @brief Destructor.
     *
     * Automatically releases the libmagic cookie via CookieT's custom
     * deleter (calls `magic_close()`).
     *
     * @par Resource Release
     * The CookieT unique_ptr ensures `Detail::magic_close()` is called
     * when the instance is destroyed, preventing resource leaks.
     *
     * @see CookieT
     */
    ~MagicPrivate() = default;

    /** @} impl_special_member_functions */

    /**
     * @defgroup impl_validity_checking Validity Checking
     * @ingroup magic_implementation
     * @brief Database validation methods.
     *
     * Methods for checking magic database file validity before use.
     *
     * @{
     */

    /**
     * @brief Check if a magic database file is valid.
     *
     * Validates the entries in a magic database file by calling
     * `magic_check()` from libmagic. This verifies the database
     * can be parsed correctly.
     *
     * @param[in] database_file Path to the database file to validate.
     *
     * @returns `true` if the database has valid entries, `false` if:
     *         - Instance is not open
     *         - database_file is empty
     *         - Database validation fails
     *
     * @par Preconditions
     * - Instance must be in **Opened** or **Valid** state
     * - database_file must be non-empty
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_check(m_cookie.get(), database_file.string().c_str());
     * @endcode
     *
     * @see Magic::Check()
     * @see Compile()
     */
    [[nodiscard]] bool Check(
        const std::filesystem::path& database_file
    ) const noexcept
    {
        if (!IsOpen() || database_file.empty()) {
            return false;
        }
        using Detail::magic_check;
        auto result = magic_check(
            m_cookie.get(),
            database_file.string().c_str()
        );
        return result != LIBMAGIC_ERROR;
    }

    /** @} impl_validity_checking */

    /**
     * @defgroup impl_instance_control Instance Control
     * @ingroup magic_implementation
     * @brief Instance lifecycle management.
     *
     * Methods for compiling databases and controlling instance state.
     *
     * @{
     */

    /**
     * @brief Compile a magic database from source.
     *
     * Compiles magic source files into binary format by calling
     * `magic_compile()` from libmagic. The compiled output file
     * is named by appending ".mgc" to the source filename.
     *
     * @param[in] database_file Path to the database source file.
     *
     * @returns `true` on successful compilation, `false` if:
     *         - Instance is not open
     *         - database_file is empty
     *         - Compilation fails
     *
     * @par Preconditions
     * - Instance must be in **Opened** or **Valid** state
     * - database_file must be non-empty
     *
     * @par Output File
     * If database_file is "/path/to/magic", the compiled output
     * will be "/path/to/magic.mgc".
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_compile(m_cookie.get(), database_file.string().c_str());
     * @endcode
     *
     * @see Magic::Compile()
     * @see Check()
     */
    [[nodiscard]] bool Compile(
        const std::filesystem::path& database_file
    ) const noexcept
    {
        if (!IsOpen() || database_file.empty()) {
            return false;
        }
        using Detail::magic_compile;
        auto result = magic_compile(
            m_cookie.get(),
            database_file.string().c_str()
        );
        return result != LIBMAGIC_ERROR;
    }

    /** @} impl_instance_control */

    /**
     * @defgroup impl_flag_management Flag Management
     * @ingroup magic_implementation
     * @brief Flag retrieval methods.
     *
     * Methods for querying current configuration flags.
     *
     * @{
     */

    /**
     * @brief Get current flags (throwing version).
     *
     * Retrieves the flags currently configured for this instance.
     * Uses FlagsConverter to transform the internal bitmask to
     * a container of individual Flags enum values.
     *
     * @returns Container of active Flags values.
     *
     * @throws MagicIsClosed If instance is not open.
     *
     * @par Implementation
     * @code{.cpp}
     * ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
     * return FlagsConverter(m_flags_mask);  // Implicit conversion
     * @endcode
     *
     * @see Magic::GetFlags()
     * @see SetFlags()
     * @see FlagsConverter
     */
    [[nodiscard]] FlagsContainerT GetFlags() const
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        return FlagsConverter(m_flags_mask);
    }

    /**
     * @brief Get current flags (noexcept version).
     *
     * Non-throwing variant that returns `std::nullopt` if the
     * instance is not open.
     *
     * @param[in] tag Pass `std::nothrow` to select this overload.
     *
     * @returns Container of active Flags values, or `std::nullopt` if closed.
     *
     * @see Magic::GetFlags(const std::nothrow_t&)
     * @see SetFlags()
     */
    [[nodiscard]] std::optional<FlagsContainerT> GetFlags(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!IsOpen()) {
            return std::nullopt;
        }
        return {FlagsConverter(m_flags_mask)};
    }

    /** @} impl_flag_management */

    /**
     * @defgroup impl_parameter_management Parameter Management
     * @ingroup magic_implementation
     * @brief Parameter retrieval methods.
     *
     * Methods for querying libmagic parameter values.
     *
     * @{
     */

    /**
     * @brief Get a single parameter value (throwing version).
     *
     * Retrieves the current value of the specified libmagic parameter
     * by calling `magic_getparam()`.
     *
     * @param[in] parameter The parameter to query.
     *
     * @returns Current value of the parameter.
     *
     * @throws MagicIsClosed If instance is not open.
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_getparam(m_cookie.get(), param_constant, &value);
     * @endcode
     *
     * @see Magic::GetParameter()
     * @see SetParameter()
     * @see LIBMAGIC_PARAMETERS
     */
    [[nodiscard]] std::size_t GetParameter(Parameters parameter) const
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        std::size_t value{};
        Detail::magic_getparam(
            m_cookie.get(),
            LibmagicPairConverter(
                LIBMAGIC_PARAMETERS[std::to_underlying(parameter)]
            ),
            &value
        );
        return value;
    }

    /**
     * @brief Get a single parameter value (noexcept version).
     *
     * Non-throwing variant that returns `std::nullopt` if the
     * instance is not open.
     *
     * @param[in] parameter The parameter to query.
     * @param[in] tag       Pass `std::nothrow` to select this overload.
     *
     * @returns Current value of the parameter, or `std::nullopt` if closed.
     *
     * @see Magic::GetParameter(Parameters, const std::nothrow_t&)
     * @see SetParameter()
     */
    [[nodiscard]] std::optional<std::size_t> GetParameter(
        Parameters                             parameter,
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!IsOpen()) {
            return std::nullopt;
        }
        std::size_t value{};
        Detail::magic_getparam(
            m_cookie.get(),
            LibmagicPairConverter(
                LIBMAGIC_PARAMETERS[std::to_underlying(parameter)]
            ),
            &value
        );
        return value;
    }

    /**
     * @brief Get all parameter values (throwing version).
     *
     * Retrieves a map of all parameters and their current values
     * by iterating over all Parameters enum values and calling
     * GetParameter() for each.
     *
     * @returns Map from Parameters enum values to their current values.
     *
     * @throws MagicIsClosed If instance is not open.
     *
     * @par Implementation
     * @code{.cpp}
     * for (i = 0; i < LIBMAGIC_PARAMETER_COUNT; ++i) {
     *     parameter_value_map[Parameters(i)] = GetParameter(Parameters(i));
     * }
     * @endcode
     *
     * @see Magic::GetParameters()
     * @see SetParameters()
     * @see LIBMAGIC_PARAMETER_COUNT
     */
    [[nodiscard]] ParameterValueMapT GetParameters() const
    {
        ParameterValueMapT parameter_value_map;
        for (std::size_t i{}; i < LIBMAGIC_PARAMETER_COUNT; ++i) {
            auto parameter                 = static_cast<Parameters>(i);
            parameter_value_map[parameter] = GetParameter(parameter);
        }
        return parameter_value_map;
    }

    /**
     * @brief Get all parameter values (noexcept version).
     *
     * Non-throwing variant that returns `std::nullopt` if the
     * instance is not open.
     *
     * @param[in] tag Pass `std::nothrow` to select this overload.
     *
     * @returns Map from Parameters to values, or `std::nullopt` if closed.
     *
     * @see Magic::GetParameters(const std::nothrow_t&)
     * @see SetParameters()
     */
    [[nodiscard]] std::optional<ParameterValueMapT> GetParameters(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!IsOpen()) {
            return std::nullopt;
        }
        ParameterValueMapT parameter_value_map;
        for (std::size_t i{}; i < LIBMAGIC_PARAMETER_COUNT; ++i) {
            auto parameter                 = static_cast<Parameters>(i);
            parameter_value_map[parameter] = GetParameter(parameter);
        }
        return parameter_value_map;
    }

    /** @} impl_parameter_management */

    /**
     * @defgroup impl_file_identification File Identification
     * @ingroup magic_implementation
     * @brief Core file type detection methods.
     *
     * Methods for identifying file types using magic number analysis.
     *
     * @{
     */

    /**
     * @brief Identify a single file's type (throwing version).
     *
     * Analyzes the content of the specified file using libmagic and
     * returns its type based on magic number analysis. The validation
     * checks performed depend on the options parameter.
     *
     * @param[in] path    Path to the file to identify.
     * @param[in] options Validation options to check before identification.
     *
     * @returns The file type string (format depends on configured Flags).
     *
     * @throws MagicIsClosed          If instance is closed (when CheckIsValid set).
     * @throws MagicDatabaseNotLoaded If database not loaded (when CheckIsValid set).
     * @throws EmptyPath              If path is empty (when CheckPathEmpty set).
     * @throws PathDoesNotExist       If file doesn't exist (when CheckPathExists set).
     * @throws MagicIdentifyFileError If `magic_file()` returns nullptr.
     *
     * @par Validation Flow
     * The options bitmask controls which checks are performed:
     * 1. `CheckIsValid` → Verify IsOpen() and IsDatabaseLoaded()
     * 2. `CheckPathEmpty` → Verify !path.empty()
     * 3. `CheckPathExists` → Verify std::filesystem::exists(path)
     *
     * @par libmagic Call
     * @code{.cpp}
     * auto type_cstr = Detail::magic_file(m_cookie.get(), path.c_str());
     * @endcode
     *
     * @see Magic::IdentifyFile()
     * @see IdentifyFiles()
     * @see IdentifyFileOptions
     */
    [[nodiscard]] FileTypeT IdentifyFile(
        const std::filesystem::path& path,
        IdentifyFileOptionsMaskT     options
    ) const
    {
        if ((options & CHECK_IS_VALID_OPTION) == CHECK_IS_VALID_OPTION) {
            MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
            MagicPrivate::ThrowExceptionOnFailure<MagicDatabaseNotLoaded>(
                IsDatabaseLoaded()
            );
        }
        if ((options & CHECK_PATH_EMPTY_OPTION) == CHECK_PATH_EMPTY_OPTION) {
            MagicPrivate::ThrowExceptionOnFailure<EmptyPath>(!path.empty());
        }
        if ((options & CHECK_PATH_EXISTS_OPTION) == CHECK_PATH_EXISTS_OPTION) {
            std::error_code error_code{};
            MagicPrivate::ThrowExceptionOnFailure<PathDoesNotExist>(
                std::filesystem::exists(path, error_code),
                path.string()
            );
        }
        auto type_cstr = Detail::magic_file(
            m_cookie.get(),
            path.string().c_str()
        );
        MagicPrivate::ThrowExceptionOnFailure<MagicIdentifyFileError>(
            type_cstr != nullptr,
            GetErrorMessage(),
            path.string()
        );
        return type_cstr;
    }

    /**
     * @brief Identify a single file's type (noexcept version).
     *
     * Non-throwing variant that returns a `std::expected` with either
     * the file type or an error message.
     *
     * @param[in] path    Path to the file to identify.
     * @param[in] options Validation options to check before identification.
     * @param[in] tag     Pass `std::nothrow` to select this overload.
     *
     * @returns ExpectedFileTypeT containing file type on success,
     *         or error message on failure.
     *
     * @par Error Handling
     * Returns `std::unexpected` with appropriate error message:
     * - MagicIsClosed::what() if instance not open
     * - MagicDatabaseNotLoaded::what() if database not loaded
     * - EmptyPath::what() if path is empty
     * - PathDoesNotExist::what() if file doesn't exist
     * - MagicIdentifyFileError::what() if magic_file() fails
     *
     * @see Magic::IdentifyFile(const std::filesystem::path&, const std::nothrow_t&)
     * @see ExpectedFileTypeT
     */
    [[nodiscard]] ExpectedFileTypeT IdentifyFile(
        const std::filesystem::path&           path,
        IdentifyFileOptionsMaskT               options,
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if ((options & CHECK_IS_VALID_OPTION) == CHECK_IS_VALID_OPTION) {
            if (!IsOpen()) {
                return std::unexpected{MagicIsClosed{}.what()};
            }
            if (!IsDatabaseLoaded()) {
                return std::unexpected{MagicDatabaseNotLoaded{}.what()};
            }
        }
        if ((options & CHECK_PATH_EMPTY_OPTION) == CHECK_PATH_EMPTY_OPTION) {
            if (path.empty()) {
                return std::unexpected{EmptyPath{}.what()};
            }
        }
        if ((options & CHECK_PATH_EXISTS_OPTION) == CHECK_PATH_EXISTS_OPTION) {
            std::error_code error_code{};
            if (!std::filesystem::exists(path, error_code)) {
                return std::unexpected{PathDoesNotExist{path.string()}.what()};
            }
        }
        auto type_cstr = Detail::magic_file(
            m_cookie.get(),
            path.string().c_str()
        );
        if (!type_cstr) {
            return std::unexpected{
                MagicIdentifyFileError{GetErrorMessage(), path.string()}
                .what()
            };
        }
        return {type_cstr};
    }

    /**
     * @brief Validate preconditions for directory identification (throwing).
     *
     * Performs comprehensive validation before directory scanning:
     * 1. Verifies instance is open and database is loaded
     * 2. Validates directory path is non-empty and exists
     * 3. Confirms path is actually a directory
     * 4. Ensures progress tracker is not null
     * 5. Collects all files via recursive directory iteration
     *
     * @param[in] directory        Directory path to scan.
     * @param[in] option           Filesystem directory iteration options.
     * @param[in] progress_tracker Tracker for progress updates (must not be null).
     *
     * @returns Container of file paths found in the directory.
     *
     * @throws MagicIsClosed          If instance is not open.
     * @throws MagicDatabaseNotLoaded If database is not loaded.
     * @throws EmptyPath              If directory path is empty.
     * @throws PathDoesNotExist       If directory does not exist.
     * @throws PathIsNotDirectory     If path is not a directory.
     * @throws NullTracker            If progress_tracker is null.
     * @throws FilesystemError        If recursive_directory_iterator fails.
     *
     * @par Implementation
     * Uses `std::filesystem::recursive_directory_iterator` to collect
     * all files in the directory tree.
     *
     * @see Magic::IdentifyFiles(const std::filesystem::path&, ...)
     * @see IdentifyFiles()
     */
    [[nodiscard]] DefaultFileContainerT IdentifyDirectoryPreliminaryChecks(
        const std::filesystem::path&       directory,
        std::filesystem::directory_options option,
        ProgressTrackerT                   progress_tracker
    ) const
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        MagicPrivate::ThrowExceptionOnFailure<MagicDatabaseNotLoaded>(
            IsDatabaseLoaded()
        );
        MagicPrivate::ThrowExceptionOnFailure<EmptyPath>(!directory.empty());
        std::error_code error_code{};
        MagicPrivate::ThrowExceptionOnFailure<PathDoesNotExist>(
            std::filesystem::exists(directory, error_code),
            directory.string()
        );
        MagicPrivate::ThrowExceptionOnFailure<PathIsNotDirectory>(
            std::filesystem::is_directory(directory, error_code),
            directory.string()
        );
        MagicPrivate::ThrowExceptionOnFailure<NullTracker>(
            progress_tracker != nullptr
        );
        auto files = std::filesystem::recursive_directory_iterator{
            directory,
            option,
            error_code
        };
        MagicPrivate::ThrowExceptionOnFailure<FilesystemError>(
            !error_code,
            directory.string(),
            error_code.message()
        );
        return {std::ranges::begin(files), std::ranges::end(files)};
    }

    /**
     * @brief Validate preconditions for directory identification (noexcept).
     *
     * Non-throwing variant that returns `std::nullopt` on any validation
     * failure. Unlike the throwing version, this silently fails on:
     * - Instance not valid (not open or database not loaded)
     * - Empty directory path
     * - Directory doesn't exist
     * - Path is not a directory
     * - Progress tracker is null
     * - Filesystem iteration error
     *
     * @param[in] directory        Directory path to scan.
     * @param[in] tag              Pass `std::nothrow` to select this overload.
     * @param[in] option           Filesystem directory iteration options.
     * @param[in] progress_tracker Tracker for progress updates.
     *
     * @returns Optional container of file paths found in directory,
     *         or `std::nullopt` on any validation failure.
     *
     * @see Magic::IdentifyFiles(const std::filesystem::path&, const std::nothrow_t&, ...)
     */
    [[nodiscard]] std::optional<DefaultFileContainerT>
        IdentifyDirectoryPreliminaryChecks(
            const std::filesystem::path&           directory,
            [[maybe_unused]] const std::nothrow_t& tag,
            std::filesystem::directory_options     option,
            ProgressTrackerT                       progress_tracker
        ) const noexcept
    {
        if (!IsValid()) {
            return std::nullopt;
        }
        if (directory.empty()) {
            return std::nullopt;
        }
        std::error_code error_code{};
        if (!std::filesystem::exists(directory, error_code)) {
            return std::nullopt;
        }
        if (!std::filesystem::is_directory(directory, error_code)) {
            return std::nullopt;
        }
        if (!progress_tracker) {
            return std::nullopt;
        }
        std::filesystem::recursive_directory_iterator files{
            directory,
            option,
            error_code
        };
        if (error_code) {
            return std::nullopt;
        }
        return {
            {std::ranges::begin(files), std::ranges::end(files)}
        };
    }

    /**
     * @brief Validate preconditions for container identification (throwing).
     *
     * Performs validation before identifying files from a user-provided
     * container. This is simpler than directory validation since the
     * caller provides the file list directly.
     *
     * @param[in] progress_tracker Tracker for progress updates (must not be null).
     *
     * @throws MagicIsClosed          If instance is not open.
     * @throws MagicDatabaseNotLoaded If database is not loaded.
     * @throws NullTracker            If progress_tracker is null.
     *
     * @see Magic::IdentifyFiles(const Utility::FileContainer auto&, ...)
     * @see IdentifyFiles()
     */
    void IdentifyContainerPreliminaryChecks(
        ProgressTrackerT progress_tracker
    ) const
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        MagicPrivate::ThrowExceptionOnFailure<MagicDatabaseNotLoaded>(
            IsDatabaseLoaded()
        );
        MagicPrivate::ThrowExceptionOnFailure<NullTracker>(
            progress_tracker != nullptr
        );
    }

    /**
     * @brief Validate preconditions for container identification (noexcept).
     *
     * Non-throwing variant that returns false on any validation failure.
     * Simple boolean check of IsValid() && progress_tracker.
     *
     * @param[in] tag              Pass `std::nothrow` to select this overload.
     * @param[in] progress_tracker Tracker for progress updates.
     *
     * @returns `true` if instance is valid AND tracker is non-null,
     *         `false` otherwise.
     *
     * @see Magic::IdentifyFiles(const Utility::FileContainer auto&, const std::nothrow_t&, ...)
     */
    bool IdentifyContainerPreliminaryChecks(
        [[maybe_unused]] const std::nothrow_t& tag,
        ProgressTrackerT                       progress_tracker
    ) const noexcept
    {
        return IsValid() && progress_tracker;
    }

    /**
     * @brief Identify multiple files with progress tracking (throwing).
     *
     * Iterates over all files in the container, identifies each one,
     * and updates the progress tracker as work completes.
     *
     * @param[in]     files            Container of file paths to identify.
     * @param[in]     option           Validation options to apply per file.
     * @param[in,out] progress_tracker Progress tracker to update.
     *
     * @returns Map from file paths to their identified types.
     *
     * @par Progress Tracking
     * The progress tracker is:
     * 1. Reset with the total file count
     * 2. Advanced after each file identification
     * 3. Marked as completed when all files are processed
     *
     * @par Implementation
     * Uses `std::ranges::for_each` with Utility::AdvanceTracker RAII helper.
     *
     * @see Magic::IdentifyFiles()
     * @see IdentifyDirectoryPreliminaryChecks()
     * @see IdentifyContainerPreliminaryChecks()
     */
    [[nodiscard]] FileTypeMapT IdentifyFiles(
        const DefaultFileContainerT& files,
        IdentifyFileOptionsMaskT     option,
        ProgressTrackerT             progress_tracker
    ) const
    {
        FileTypeMapT file_type_map;
        progress_tracker->Reset(std::ranges::distance(files));
        std::ranges::for_each(files, [&](const auto& file) {
            Utility::AdvanceTracker tracker_advancer{progress_tracker};
            file_type_map[file] = IdentifyFile(file, option);
        });
        progress_tracker->MarkAsCompleted();
        return file_type_map;
    }

    /**
     * @brief Identify multiple files with progress tracking (noexcept).
     *
     * Non-throwing variant that stores results as `std::expected` values,
     * allowing per-file error handling without stopping the batch.
     *
     * @param[in]     files            Container of file paths to identify.
     * @param[in]     tag              Pass `std::nothrow` to select this overload.
     * @param[in]     option           Validation options to apply per file.
     * @param[in,out] progress_tracker Progress tracker to update.
     *
     * @returns Map from file paths to expected file type results.
     *
     * @par Empty Path Handling
     * Files with empty paths are silently skipped (not added to result map).
     * The `CHECK_PATH_EMPTY_OPTION` bit is cleared from options.
     *
     * @see Magic::IdentifyFiles(const Utility::FileContainer auto&, const std::nothrow_t&, ...)
     * @see ExpectedFileTypeMapT
     */
    [[nodiscard]] ExpectedFileTypeMapT IdentifyFiles(
        const DefaultFileContainerT&           files,
        [[maybe_unused]] const std::nothrow_t& tag,
        IdentifyFileOptionsMaskT               option,
        ProgressTrackerT                       progress_tracker
    ) const noexcept
    {
        ExpectedFileTypeMapT expected_file_type_map;
        option &= ~CHECK_PATH_EMPTY_OPTION;
        progress_tracker->Reset(std::ranges::distance(files));
        std::ranges::for_each(files, [&](const auto& file) {
            Utility::AdvanceTracker tracker_advancer{progress_tracker};
            if (file.empty()) {
                return;
            }
            expected_file_type_map[file] = IdentifyFile(
                file,
                option,
                std::nothrow
            );
        });
        return expected_file_type_map;
    }

    /** @} impl_file_identification */

    /**
     * @defgroup impl_state_queries State Queries
     * @ingroup magic_implementation
     * @brief Instance state inspection methods.
     *
     * Methods for querying the current lifecycle state.
     *
     * @{
     */

    /**
     * @brief Check if a database is loaded.
     *
     * Returns the state of the `m_is_database_loaded` flag, which
     * is set to `true` only after a successful LoadDatabaseFile() call.
     *
     * @returns `true` if database is loaded (Valid state possible),
     *         `false` if not loaded (Closed or Opened state).
     *
     * @par State Indicator
     * This is one of two flags that determine the lifecycle state:
     * - IsOpen() returns true → **Opened** or **Valid**
     * - IsDatabaseLoaded() returns true → **Valid** (when also open)
     *
     * @see Magic::IsDatabaseLoaded()
     * @see IsOpen()
     * @see IsValid()
     * @see LoadDatabaseFile()
     */
    [[nodiscard]] bool IsDatabaseLoaded() const noexcept
    {
        return m_is_database_loaded;
    }

    /**
     * @brief Check if instance is open.
     *
     * Returns whether the libmagic cookie has been allocated.
     * A non-null cookie indicates the instance is at least in
     * the **Opened** state.
     *
     * @returns `true` if cookie is allocated (Opened or Valid state),
     *         `false` if cookie is null (Closed state).
     *
     * @par Implementation
     * @code{.cpp}
     * return m_cookie != nullptr;
     * @endcode
     *
     * @see Magic::IsOpen()
     * @see Open()
     * @see IsValid()
     * @see CookieT
     */
    [[nodiscard]] bool IsOpen() const noexcept
    {
        return m_cookie != nullptr;
    }

    /**
     * @brief Check if instance is valid for file identification.
     *
     * An instance is **Valid** when both conditions are met:
     * - Cookie is allocated (IsOpen() returns true)
     * - Database is loaded (IsDatabaseLoaded() returns true)
     *
     * @returns `true` if ready for file identification (Valid state),
     *         `false` otherwise (Closed or Opened state).
     *
     * @par Implementation
     * @code{.cpp}
     * return IsOpen() && IsDatabaseLoaded();
     * @endcode
     *
     * @see Magic::IsValid()
     * @see Magic::operator bool()
     * @see IsOpen()
     * @see IsDatabaseLoaded()
     */
    [[nodiscard]] bool IsValid() const noexcept
    {
        return IsOpen() && IsDatabaseLoaded();
    }

    /** @} impl_state_queries */

    /**
     * @defgroup impl_database_loading Database Loading
     * @ingroup magic_implementation
     * @brief Magic database loading methods.
     *
     * Methods for loading magic database files to enable identification.
     *
     * @{
     */

    /**
     * @brief Load a magic database file (throwing version).
     *
     * Loads the specified magic database file into this instance.
     * On success, transitions from **Opened** to **Valid** state.
     *
     * @param[in] database_file Path to the magic database file.
     *
     * @throws MagicIsClosed              If instance is not open.
     * @throws EmptyPath                  If database_file is empty.
     * @throws PathDoesNotExist           If database_file does not exist.
     * @throws PathIsNotRegularFile       If database_file is not a regular file.
     * @throws MagicLoadDatabaseFileError If `magic_load()` fails.
     *
     * @par Lifecycle Transition
     * **Opened** → **Valid**
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_load(m_cookie.get(), database_file.c_str());
     * @endcode
     *
     * @note Automatically appends ".mgc" extension if needed.
     * @note Sets `m_is_database_loaded = true` only on success.
     *
     * @see Magic::LoadDatabaseFile()
     * @see Open()
     * @see IsValid()
     */
    void LoadDatabaseFile(const std::filesystem::path& database_file)
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        MagicPrivate::ThrowExceptionOnFailure<EmptyPath>(
            !database_file.empty()
        );
        std::error_code error_code{};
        MagicPrivate::ThrowExceptionOnFailure<PathDoesNotExist>(
            std::filesystem::exists(database_file, error_code),
            database_file.string()
        );
        MagicPrivate::ThrowExceptionOnFailure<PathIsNotRegularFile>(
            std::filesystem::is_regular_file(database_file, error_code),
            database_file.string()
        );
        m_is_database_loaded = false;
        MagicPrivate::ThrowExceptionOnFailure<MagicLoadDatabaseFileError>(
            Detail::magic_load(m_cookie.get(), database_file.string().c_str()),
            GetErrorMessage(),
            database_file.string()
        );
        m_is_database_loaded = true;
    }

    /**
     * @brief Load a magic database file (noexcept version).
     *
     * Non-throwing variant that returns success/failure status.
     * On success, transitions from **Opened** to **Valid** state.
     *
     * @param[in] tag           Pass `std::nothrow` to select this overload.
     * @param[in] database_file Path to the magic database file.
     *
     * @returns `true` on success (now in Valid state),
     *         `false` on any failure (state unchanged).
     *
     * @par Lifecycle Transition
     * **Opened** → **Valid** on success
     *
     * @par Failure Conditions
     * Returns false if:
     * - Instance is not open
     * - database_file is empty
     * - database_file does not exist
     * - database_file is not a regular file
     * - magic_load() returns error
     *
     * @see Magic::LoadDatabaseFile(const std::nothrow_t&, const std::filesystem::path&)
     * @see Open()
     */
    [[nodiscard]] bool LoadDatabaseFile(
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path&           database_file
    ) noexcept
    {
        if (!IsOpen() || database_file.empty()) {
            return false;
        }
        std::error_code error_code{};
        if (!std::filesystem::exists(database_file, error_code)) {
            return false;
        }
        if (!std::filesystem::is_regular_file(database_file, error_code)) {
            return false;
        }
        m_is_database_loaded = Detail::magic_load(
                                   m_cookie.get(),
                                   database_file.string().c_str()
                               )
                            != LIBMAGIC_ERROR;
        return IsDatabaseLoaded();
    }

    /** @} impl_database_loading */

    /**
     * @defgroup impl_open_reopen Open Reopen
     * @ingroup magic_implementation
     * @brief Instance initialization methods.
     *
     * Methods for opening and reopening the libmagic cookie.
     *
     * @{
     */

    /**
     * @brief Open the magic instance with flags (throwing version).
     *
     * Allocates the libmagic cookie with the specified configuration flags.
     * This is required before loading a database or identifying files.
     *
     * @param[in] flags_mask Configuration flags as a bitmask.
     *
     * @throws MagicOpenError If `magic_open()` fails.
     *
     * @par Lifecycle Transitions
     * - **Closed** → **Opened**
     * - **Opened** → **Opened** (reopens, database unloaded)
     * - **Valid** → **Opened** (reopens, database unloaded)
     *
     * @par libmagic Call
     * @code{.cpp}
     * m_cookie.reset(Detail::magic_open(FlagsConverter(flags_mask)));
     * @endcode
     *
     * @note Calling Open() on an already-open instance will reopen it,
     *       which unloads any previously loaded database.
     *
     * @see Magic::Open(FlagsMaskT)
     * @see LoadDatabaseFile()
     * @see FlagsConverter
     */
    void Open(FlagsMaskT flags_mask)
    {
        m_is_database_loaded = false;
        m_cookie.reset(Detail::magic_open(FlagsConverter(flags_mask)));
        MagicPrivate::ThrowExceptionOnFailure<MagicOpenError>(
            IsOpen(),
            GetErrorMessage()
        );
        m_flags_mask = flags_mask;
    }

    /**
     * @brief Open the magic instance with flags (noexcept version).
     *
     * Non-throwing variant that returns success/failure status.
     *
     * @param[in] flags_mask Configuration flags as a bitmask.
     * @param[in] tag        Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success (now in Opened state),
     *         `false` if magic_open() fails.
     *
     * @par Lifecycle Transitions
     * - **Closed** → **Opened** on success
     * - **Opened/Valid** → **Opened** on success (database unloaded)
     * - No transition on failure
     *
     * @see Magic::Open(FlagsMaskT, const std::nothrow_t&)
     * @see LoadDatabaseFile()
     */
    [[nodiscard]] bool Open(
        FlagsMaskT                             flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        m_is_database_loaded = false;
        m_cookie.reset(Detail::magic_open(FlagsConverter(flags_mask)));
        if (!IsOpen()) {
            return false;
        }
        m_flags_mask = flags_mask;
        return true;
    }

    /**
     * @brief Open with flags container (throwing version).
     *
     * Convenience overload accepting a container of individual Flags
     * values. Converts to bitmask and delegates to Open(FlagsMaskT).
     *
     * @param[in] flags_container Container (vector) of Flags enum values.
     *
     * @throws MagicOpenError If `magic_open()` fails.
     *
     * @see Magic::Open(const FlagsContainerT&)
     * @see Open(FlagsMaskT)
     * @see FlagsConverter
     */
    void Open(const FlagsContainerT& flags_container)
    {
        Open(FlagsMaskT{FlagsConverter(flags_container)});
    }

    /**
     * @brief Open with flags container (noexcept version).
     *
     * Non-throwing convenience overload accepting a container of Flags.
     *
     * @param[in] flags_container Container (vector) of Flags enum values.
     * @param[in] tag             Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @see Magic::Open(const FlagsContainerT&, const std::nothrow_t&)
     */
    [[nodiscard]] bool Open(
        const FlagsContainerT&                 flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return Open(FlagsMaskT{FlagsConverter(flags_container)}, std::nothrow);
    }

    /** @} impl_open_reopen */

    /**
     * @defgroup impl_flag_modification Flag Modification
     * @ingroup magic_implementation
     * @brief Flag modification methods.
     *
     * Methods for changing configuration flags after opening.
     *
     * @{
     */

    /**
     * @brief Set new flags for the instance (throwing version).
     *
     * Changes the configuration flags for this instance by calling
     * `magic_setflags()`. This affects how subsequent file
     * identifications are performed.
     *
     * @param[in] flags_mask New configuration flags as a bitmask.
     *
     * @throws MagicIsClosed      If instance is not open.
     * @throws MagicSetFlagsError If `magic_setflags()` fails.
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_setflags(m_cookie.get(), FlagsConverter(flags_mask));
     * @endcode
     *
     * @note Updates internal m_flags_mask on success.
     *
     * @see Magic::SetFlags(FlagsMaskT)
     * @see GetFlags()
     */
    void SetFlags(FlagsMaskT flags_mask)
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        MagicPrivate::ThrowExceptionOnFailure<MagicSetFlagsError>(
            Detail::magic_setflags(m_cookie.get(), FlagsConverter(flags_mask)),
            GetErrorMessage(),
            FlagsConverter(flags_mask)
        );
        m_flags_mask = flags_mask;
    }

    /**
     * @brief Set new flags for the instance (noexcept version).
     *
     * Non-throwing variant that returns success/failure status.
     *
     * @param[in] flags_mask New configuration flags as a bitmask.
     * @param[in] tag        Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` if closed or setflags fails.
     *
     * @see Magic::SetFlags(FlagsMaskT, const std::nothrow_t&)
     * @see GetFlags()
     */
    [[nodiscard]] bool SetFlags(
        FlagsMaskT                             flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        if (!IsOpen()) {
            return false;
        }
        auto result = Detail::magic_setflags(
                          m_cookie.get(),
                          FlagsConverter(flags_mask)
                      )
                   != LIBMAGIC_ERROR;
        if (!result) {
            return false;
        }
        m_flags_mask = flags_mask;
        return true;
    }

    /**
     * @brief Set new flags from container (throwing version).
     *
     * Convenience overload accepting a container of individual Flags.
     * Converts to bitmask and delegates to SetFlags(FlagsMaskT).
     *
     * @param[in] flags_container Container (vector) of Flags enum values.
     *
     * @throws MagicIsClosed      If instance is not open.
     * @throws MagicSetFlagsError If `magic_setflags()` fails.
     *
     * @see Magic::SetFlags(const FlagsContainerT&)
     */
    void SetFlags(const FlagsContainerT& flags_container)
    {
        SetFlags(FlagsMaskT{FlagsConverter(flags_container)});
    }

    /**
     * @brief Set new flags from container (noexcept version).
     *
     * Non-throwing convenience overload accepting a container of Flags.
     *
     * @param[in] flags_container Container (vector) of Flags enum values.
     * @param[in] tag             Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` on failure.
     *
     * @see Magic::SetFlags(const FlagsContainerT&, const std::nothrow_t&)
     */
    [[nodiscard]] bool SetFlags(
        const FlagsContainerT&                 flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return SetFlags(
            FlagsMaskT{FlagsConverter(flags_container)},
            std::nothrow
        );
    }

    /** @} impl_flag_modification */

    /**
     * @defgroup impl_parameter_modification Parameter Modification
     * @ingroup magic_implementation
     * @brief Parameter modification methods.
     *
     * Methods for changing libmagic parameter values.
     *
     * @{
     */

    /**
     * @brief Set a single parameter value (throwing version).
     *
     * Changes the value of a libmagic parameter by calling
     * `magic_setparam()`. Parameters control various internal
     * limits and behaviors.
     *
     * @param[in] parameter The parameter to set.
     * @param[in] value     The new value for the parameter.
     *
     * @throws MagicIsClosed          If instance is not open.
     * @throws MagicSetParameterError If `magic_setparam()` fails.
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_setparam(m_cookie.get(), param_constant, &value);
     * @endcode
     *
     * @see Magic::SetParameter()
     * @see GetParameter()
     * @see LIBMAGIC_PARAMETERS
     */
    void SetParameter(Parameters parameter, std::size_t value)
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        const auto& libmagic_parameter{
            LIBMAGIC_PARAMETERS[std::to_underlying(parameter)]
        };
        MagicPrivate::ThrowExceptionOnFailure<MagicSetParameterError>(
            Detail::magic_setparam(
                m_cookie.get(),
                LibmagicPairConverter(libmagic_parameter),
                &value
            ),
            GetErrorMessage(),
            LibmagicPairConverter(libmagic_parameter),
            value
        );
    }

    /**
     * @brief Set a single parameter value (noexcept version).
     *
     * Non-throwing variant that returns success/failure status.
     *
     * @param[in] parameter The parameter to set.
     * @param[in] value     The new value for the parameter.
     * @param[in] tag       Pass `std::nothrow` to select this overload.
     *
     * @returns `true` on success, `false` if closed or setparam fails.
     *
     * @see Magic::SetParameter(Parameters, std::size_t, const std::nothrow_t&)
     * @see GetParameter()
     */
    [[nodiscard]] bool SetParameter(
        Parameters                             parameter,
        std::size_t                            value,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        if (!IsOpen()) {
            return false;
        }
        const auto& libmagic_parameter{
            LIBMAGIC_PARAMETERS[std::to_underlying(parameter)]
        };
        return Detail::magic_setparam(
                   m_cookie.get(),
                   LibmagicPairConverter(libmagic_parameter),
                   &value
               )
            != LIBMAGIC_ERROR;
    }

    /**
     * @brief Set multiple parameter values (throwing version).
     *
     * Sets all parameters specified in the map by iterating and
     * calling SetParameter() for each entry.
     *
     * @param[in] parameters Map from Parameters enum to new values.
     *
     * @throws MagicIsClosed          If instance is not open.
     * @throws MagicSetParameterError If any `magic_setparam()` fails.
     *
     * @note If an exception is thrown mid-iteration, some parameters
     *       may already have been set.
     *
     * @see Magic::SetParameters(const ParameterValueMapT&)
     * @see GetParameters()
     */
    void SetParameters(const ParameterValueMapT& parameters)
    {
        std::ranges::for_each(
            parameters,
            [&](const auto& parameter_value_pair) {
                const auto& parameter = parameter_value_pair.first;
                const auto& value     = parameter_value_pair.second;
                SetParameter(parameter, value);
            }
        );
    }

    /**
     * @brief Set multiple parameter values (noexcept version).
     *
     * Non-throwing variant that returns false on first failure.
     * Uses `std::ranges::find_if_not` to stop on first error.
     *
     * @param[in] parameters Map from Parameters enum to new values.
     * @param[in] tag        Pass `std::nothrow` to select this overload.
     *
     * @returns `true` if all parameters set successfully,
     *         `false` if any failed (partial updates may have occurred).
     *
     * @see Magic::SetParameters(const ParameterValueMapT&, const std::nothrow_t&)
     * @see GetParameters()
     */
    [[nodiscard]] bool SetParameters(
        const ParameterValueMapT&              parameters,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return std::ranges::find_if_not(
                   parameters,
                   [&](const auto& parameter_value_pair) {
                       const auto& parameter = parameter_value_pair.first;
                       const auto& value     = parameter_value_pair.second;
                       return SetParameter(parameter, value, std::nothrow);
                   }
               )
            == parameters.end();
    }

    /** @} impl_parameter_modification */

    /**
     * @defgroup impl_error_handling Error Handling
     * @ingroup magic_implementation
     * @brief Centralized exception throwing utilities.
     *
     * Template-based error handling for consistent exception throwing
     * across all MagicPrivate methods.
     *
     * @{
     */

    /**
     * @brief Centralized error handling template.
     *
     * Throws an exception of type ExceptionT if the result indicates
     * failure. Works with both boolean and integer result types.
     *
     * @tparam ExceptionT     Exception type derived from MagicException.
     * @tparam ResultT        Either `int` or `bool`.
     * @tparam ExceptionArgsT Types of exception constructor arguments.
     *
     * @param[in] result Result to check (false or LIBMAGIC_ERROR = failure).
     * @param[in] args   Arguments forwarded to ExceptionT constructor.
     *
     * @throws ExceptionT If result is false (bool) or -1 (int).
     *
     * @par Usage Examples
     * @code{.cpp}
     * // Boolean check - throws if false
     * ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
     *
     * // Integer check - throws if -1
     * ThrowExceptionOnFailure<MagicOpenError>(result, GetErrorMessage());
     *
     * // With additional exception arguments
     * ThrowExceptionOnFailure<PathDoesNotExist>(
     *     std::filesystem::exists(path), path.string()
     * );
     * @endcode
     *
     * @see MagicException
     */
    template <
        std::derived_from<MagicException> ExceptionT,
        typename ResultT,
        typename... ExceptionArgsT
    >
    requires std::same_as<ResultT, int> || std::same_as<ResultT, bool>
    static void ThrowExceptionOnFailure(
        ResultT result,
        ExceptionArgsT&&... args
    )
    {
        bool throw_exception = false;
        if constexpr (std::same_as<int, ResultT>) {
            throw_exception = result == LIBMAGIC_ERROR;
        } else if constexpr (std::same_as<ResultT, bool>) {
            throw_exception = !result;
        }
        if (!throw_exception) {
            return;
        }
        throw ExceptionT{std::forward<ExceptionArgsT>(args)...};
    }

    /** @} impl_error_handling */

private:
    /**
     * @brief RAII cookie type wrapping libmagic handle.
     *
     * A `std::unique_ptr` specialized with a custom deleter lambda that
     * calls `magic_close()` on destruction. This ensures the libmagic
     * cookie is always properly released, even on exceptions.
     *
     * @par Custom Deleter
     * @code{.cpp}
     * [](Detail::magic_t cookie) noexcept {
     *     Detail::magic_close(cookie);
     * }
     * @endcode
     *
     * @see m_cookie
     * @see Open()
     */
    using CookieT = std::unique_ptr<
        Detail::magic_set,
        decltype([](Detail::magic_t cookie) noexcept {
            Detail::magic_close(cookie);
        })
    >;

    /**
     * @defgroup impl_member_variables Member Variables
     * @ingroup magic_implementation
     * @brief Core state variables for lifecycle management.
     *
     * These member variables track the internal state of a MagicPrivate
     * instance throughout its lifecycle.
     *
     * @{
     */
    CookieT m_cookie{nullptr}; /**< libmagic handle (nullptr = Closed state) */
    FlagsMaskT m_flags_mask{}; /**< Current configuration flags bitmask */
    bool       m_is_database_loaded{
        false
    }; /**< True only after successful LoadDatabaseFile() */
    /** @} impl_member_variables */

    /**
     * @defgroup impl_validation_option_masks Validation Option Masks
     * @ingroup magic_implementation
     * @brief Pre-computed option masks for file identification.
     *
     * Static bitmasks used for efficient option checking during validation.
     */
    /** @{ */
    static constexpr IdentifyFileOptionsMaskT CHECK_IS_VALID_OPTION{
        IdentifyFileOptions::CheckIsValid
    }; /**< Bitmask for the CheckIsValid option. */
    static constexpr IdentifyFileOptionsMaskT CHECK_PATH_EMPTY_OPTION{
        IdentifyFileOptions::CheckPathEmpty
    }; /**< Bitmask for the CheckPathEmpty option. */
    static constexpr IdentifyFileOptionsMaskT CHECK_PATH_EXISTS_OPTION{
        IdentifyFileOptions::CheckPathExists
    }; /**< Bitmask for the CheckPathExists option. */
    /** @} impl_validation_option_masks */

    /**
     * @defgroup impl_libmagic_constants Libmagic Constants
     * @ingroup magic_implementation
     * @brief Constants for libmagic integration.
     *
     * Error values and array sizes used throughout the implementation.
     */
    /** @{ */
    static constexpr auto LIBMAGIC_ERROR{
        -1
    }; /**< libmagic error return value */
    static constexpr auto LIBMAGIC_FLAGS_COUNT{
        FlagsMaskT{}.size()
    }; /**< Number of flags */
    static constexpr auto LIBMAGIC_PARAMETER_COUNT{
        10uz
    }; /**< Number of parameters */
    /** @} impl_libmagic_constants */

    /**
     * @defgroup impl_libmagic_type_aliases Libmagic Type Aliases
     * @ingroup magic_implementation
     * @brief Type definitions for libmagic interoperability.
     *
     * Type aliases for flag/parameter value-name pairs and arrays.
     */
    /** @{ */
    using LibmagicValueT     = int;         /**< libmagic integer type */
    using LibmagicValueNameT = std::string; /**< Flag/param name type */
    using LibmagicPairT      = std::pair<
             LibmagicValueT,
             const char*
         >; /**< Value-name pair */
    using LibmagicFlagsT = std::array<
        LibmagicPairT,
        LIBMAGIC_FLAGS_COUNT
    >; /**< Array type mapping all libmagic flag values to names. */
    using LibmagicParametersT = std::array<
        LibmagicPairT,
        LIBMAGIC_PARAMETER_COUNT
    >; /**< Array type mapping all libmagic parameter values to names. */
    /** @} impl_libmagic_type_aliases */

    /**
     * @brief The MAGIC_NONE flag pair for default output.
     *
     * @see Flags::None
     */
    static constexpr LibmagicPairT LIBMAGIC_FLAG_NONE{
        std::make_pair(MAGIC_NONE, "None")
    };

    /**
     * @brief Mapping from Magic::Flags bit positions to libmagic constants.
     *
     * Static lookup table mapping each Flags enum bit position to
     * the corresponding libmagic MAGIC_* constant and its
     * human-readable name.
     *
     * @see Flags
     * @see GetFlags()
     * @see SetFlags()
     */
    static constexpr LibmagicFlagsT LIBMAGIC_FLAGS{
        std::make_pair(MAGIC_DEBUG, "Debug"),
        std::make_pair(MAGIC_SYMLINK, "Symlink"),
        std::make_pair(MAGIC_COMPRESS, "Compress"),
        std::make_pair(MAGIC_DEVICES, "Devices"),
        std::make_pair(MAGIC_MIME_TYPE, "MimeType"),
        std::make_pair(MAGIC_CONTINUE, "ContinueSearch"),
        std::make_pair(MAGIC_CHECK, "CheckDatabase"),
        std::make_pair(MAGIC_PRESERVE_ATIME, "PreserveAtime"),
        std::make_pair(MAGIC_RAW, "Raw"),
        std::make_pair(MAGIC_ERROR, "Error"),
        std::make_pair(MAGIC_MIME_ENCODING, "MimeEncoding"),
        std::make_pair(MAGIC_MIME, "Mime"),
        std::make_pair(MAGIC_APPLE, "Apple"),
        std::make_pair(MAGIC_EXTENSION, "Extension"),
        std::make_pair(MAGIC_COMPRESS_TRANSP, "CompressTransp"),
        std::make_pair(MAGIC_NO_COMPRESS_FORK, "NoCompressFork"),
        std::make_pair(MAGIC_NODESC, "Nodesc"),
        std::make_pair(MAGIC_NO_CHECK_COMPRESS, "NoCheckCompress"),
        std::make_pair(MAGIC_NO_CHECK_TAR, "NoCheckTar"),
        std::make_pair(MAGIC_NO_CHECK_SOFT, "NoCheckSoft"),
        std::make_pair(MAGIC_NO_CHECK_APPTYPE, "NoCheckApptype"),
        std::make_pair(MAGIC_NO_CHECK_ELF, "NoCheckElf"),
        std::make_pair(MAGIC_NO_CHECK_TEXT, "NoCheckText"),
        std::make_pair(MAGIC_NO_CHECK_CDF, "NoCheckCdf"),
        std::make_pair(MAGIC_NO_CHECK_CSV, "NoCheckCsv"),
        std::make_pair(MAGIC_NO_CHECK_TOKENS, "NoCheckTokens"),
        std::make_pair(MAGIC_NO_CHECK_ENCODING, "NoCheckEncoding"),
        std::make_pair(MAGIC_NO_CHECK_JSON, "NoCheckJson"),
        std::make_pair(MAGIC_NO_CHECK_SIMH, "NoCheckSimh"),
        std::make_pair(MAGIC_NO_CHECK_BUILTIN, "NoCheckBuiltin")
    };

    /**
     * @brief Mapping from Magic::Parameters enum to libmagic constants.
     *
     * Static lookup table mapping Parameters enum ordinal values to
     * the corresponding libmagic MAGIC_PARAM_* constants and their
     * human-readable names.
     *
     * @see Parameters
     * @see GetParameter()
     * @see SetParameter()
     */
    static constexpr LibmagicParametersT LIBMAGIC_PARAMETERS{
        std::make_pair(MAGIC_PARAM_INDIR_MAX, "IndirMax"),
        std::make_pair(MAGIC_PARAM_NAME_MAX, "NameMax"),
        std::make_pair(MAGIC_PARAM_ELF_PHNUM_MAX, "ElfPhnumMax"),
        std::make_pair(MAGIC_PARAM_ELF_SHNUM_MAX, "ElfShnumMax"),
        std::make_pair(MAGIC_PARAM_ELF_NOTES_MAX, "ElfNotesMax"),
        std::make_pair(MAGIC_PARAM_REGEX_MAX, "RegexMax"),
        std::make_pair(MAGIC_PARAM_BYTES_MAX, "BytesMax"),
        std::make_pair(MAGIC_PARAM_ENCODING_MAX, "EncodingMax"),
        std::make_pair(MAGIC_PARAM_ELF_SHSIZE_MAX, "ElfShsizeMax"),
        std::make_pair(MAGIC_PARAM_MAGWARN_MAX, "MagWarnMax")
    };

    /**
     * @brief Retrieve the last error message from libmagic.
     *
     * Calls `magic_error()` to get the most recent error message
     * from the libmagic library. Returns an empty string if no
     * error message is available.
     *
     * @returns Error message string, or empty string if none.
     *
     * @par libmagic Call
     * @code{.cpp}
     * Detail::magic_error(m_cookie.get());
     * @endcode
     *
     * @note Used internally to populate exception messages.
     */
    [[nodiscard]] std::string GetErrorMessage() const noexcept
    {
        const char* magic_error_cstr = Detail::magic_error(m_cookie.get());
        return magic_error_cstr ? magic_error_cstr : "";
    }

    /**
     * @brief Converter between C++ flag types and libmagic flag values.
     * @ingroup magic_implementation
     *
     * FlagsConverter provides bidirectional conversion between the various
     * flag representations used in the library:
     *
     * ### Supported Conversions
     *
     * | From | To | Purpose |
     * |------|----|---------|
     * | `FlagsContainerT` | `FlagsMaskT` | Container to bitmask |
     * | `FlagsMaskT` | `LibmagicValueT` | Bitmask to libmagic int |
     * | `FlagsMaskT` | `FlagsContainerT` | Bitmask to container |
     * | `FlagsMaskT` | `LibmagicValueNameT` | Bitmask to string |
     *
     * ### Usage Examples
     * @code{.cpp}
     * // Container to libmagic int (for magic_open)
     * FlagsContainerT flags = {Flags::Mime, Flags::Compress};
     * LibmagicValueT libmagic_flags = FlagsConverter(flags);
     *
     * // Bitmask to container (for GetFlags)
     * FlagsMaskT mask = ...;
     * FlagsContainerT container = FlagsConverter(mask);
     *
     * // Bitmask to string (for ToString)
     * std::string names = FlagsConverter(mask);
     * @endcode
     *
     * @see Magic::Flags
     * @see Magic::FlagsContainerT
     * @see Magic::FlagsMaskT
     */
    struct FlagsConverter {
        /**
         * @brief Construct from a container of flags.
         *
         * Folds the container using bitwise OR to create the bitmask.
         *
         * @param[in] flags_container Container of individual Flags values.
         */
        explicit FlagsConverter(const FlagsContainerT& flags_container) noexcept
          : m_flags_mask{std::ranges::fold_left(
                flags_container,
                FlagsMask{
                    flags_container.empty() ? Flags::None
                                            : flags_container.front()
                },
                [](FlagsMask acc, Flags f) {
                    return acc | f;
                }
            )}
        { }

        /**
         * @brief Construct from a flags bitmask.
         *
         * @param[in] flags_mask Pre-computed flags bitmask.
         */
        explicit FlagsConverter(FlagsMaskT flags_mask) noexcept
          : m_flags_mask{flags_mask}
        { }

        /**
         * @brief Convert to libmagic integer flags.
         *
         * Iterates through the bitmask and ORs together the corresponding
         * libmagic constants for use with magic_open() and magic_setflags().
         *
         * @returns Libmagic-compatible integer flags value.
         */
        operator LibmagicValueT() const noexcept
        {
            LibmagicValueT flags = LibmagicPairConverter(LIBMAGIC_FLAG_NONE);
            for (std::size_t i{}; i < m_flags_mask.size(); ++i) {
                if (m_flags_mask[i]) {
                    flags |= LibmagicPairConverter(LIBMAGIC_FLAGS[i]);
                }
            }
            return flags;
        }

        /**
         * @brief Convert to container of individual flag values.
         *
         * Extracts each set bit in the bitmask and converts to the
         * corresponding Flags enum value.
         *
         * @returns Container of active Flags values.
         */
        operator FlagsContainerT() const
        {
            if (m_flags_mask.none()) {
                LibmagicValueT value = LibmagicPairConverter(
                    LIBMAGIC_FLAG_NONE
                );
                return {static_cast<Flags>(value)};
            }
            FlagsContainerT flags_container;
            for (std::size_t i{}; i < m_flags_mask.size(); ++i) {
                if (m_flags_mask[i]) {
                    flags_container.push_back(static_cast<Flags>(1ULL << i));
                }
            }
            return flags_container;
        }

        /**
         * @brief Convert to comma-separated flag names string.
         *
         * Creates a human-readable string of flag names for logging
         * and debugging purposes.
         *
         * @returns Comma-separated flag names (e.g., "Mime,Compress").
         */
        operator LibmagicValueNameT() const
        {
            if (m_flags_mask.none()) {
                return LibmagicPairConverter(LIBMAGIC_FLAG_NONE);
            }
            LibmagicValueNameT flags;
            for (std::size_t i{}; i < m_flags_mask.size(); ++i) {
                if (m_flags_mask[i]) {
                    flags.append(
                        LibmagicPairConverter(LIBMAGIC_FLAGS[i])
                    ) += ",";
                }
            }
            flags.erase(flags.find_last_of(','));
            return flags;
        }

        /**
         * @brief Convert to bitmask representation.
         *
         * @returns The underlying FlagsMaskT bitmask.
         */
        operator FlagsMaskT() const
        {
            return m_flags_mask;
        }

        const FlagsMaskT m_flags_mask; /**< The underlying flags bitmask */
    };

    /**
     * @brief Extracts values from libmagic constant pairs.
     * @ingroup magic_implementation
     *
     * LibmagicPairConverter wraps a `LibmagicPairT` (pair of int constant
     * and const char* name) and provides implicit conversions to either
     * the integer value or string name.
     *
     * ### Usage Example
     * @code{.cpp}
     * // Extract integer constant for libmagic API call
     * LibmagicValueT magic_mime = LibmagicPairConverter(LIBMAGIC_FLAGS[4]);
     *
     * // Extract string name for ToString/logging
     * LibmagicValueNameT name = LibmagicPairConverter(LIBMAGIC_FLAGS[4]);
     * // name == "MimeType"
     * @endcode
     *
     * @see LibmagicPairT
     * @see LIBMAGIC_FLAGS
     * @see LIBMAGIC_PARAMETERS
     */
    struct LibmagicPairConverter {
        /**
         * @brief Construct from a libmagic pair.
         *
         * @param[in] pair Reference to a (constant, name) pair.
         */
        constexpr explicit LibmagicPairConverter(
            const LibmagicPairT& pair
        ) noexcept
          : m_pair{pair}
        { }

        /**
         * @brief Convert to string name.
         *
         * @returns The human-readable name from the pair.
         */
        constexpr operator LibmagicValueNameT() const noexcept
        {
            return std::get<const char*>(m_pair);
        }

        /**
         * @brief Convert to integer constant value.
         *
         * @returns The libmagic constant (e.g., MAGIC_MIME).
         */
        constexpr operator LibmagicValueT() const noexcept
        {
            return std::get<LibmagicValueT>(m_pair);
        }

        const LibmagicPairT& m_pair; /**< Reference to the wrapped pair */
    };

    /**
     * @defgroup impl_friend_declarations Friend Declarations For ToString Access To Private Constants
     * @ingroup magic_implementation
     * @brief Friend declarations for string conversion access.
     *
     * Grants ToString() functions access to private LIBMAGIC_FLAGS and
     * LIBMAGIC_PARAMETERS arrays for generating human-readable names.
     */
    /** @{ */
    friend std::string ToString(Flags);
    friend std::string ToString(Parameters);
    /** @} impl_friend_declarations */
};

/* ===========================================================================
 * ToString() Free Function Implementations
 * ===========================================================================
 *
 * These functions provide string conversion utilities for Magic types.
 * They are used for logging, debugging, and user-facing output.
 *
 * See magic.hpp for the complete public API documentation with usage examples.
 */

std::string ToString(
    const Magic::FileTypeEntryT& file_type_entry,
    const std::string&           type_separator
)
{
    const auto& file      = file_type_entry.first;
    const auto& file_type = file_type_entry.second;
    return file.string() + type_separator + file_type;
}

std::string ToString(
    const Magic::FileTypeMapT& file_type_map,
    const std::string&         type_separator,
    const std::string&         file_separator
)
{
    return Utility::ToString(
        file_type_map,
        file_separator,
        [&type_separator](const Magic::FileTypeEntryT& file_type_entry) {
            return ToString(file_type_entry, type_separator);
        }
    );
}

std::string ToString(const Magic::ExpectedFileTypeT& expected_file_type)
{
    return expected_file_type.value_or(expected_file_type.error());
}

std::string ToString(
    const Magic::ExpectedFileTypeEntryT& expected_file_type_entry,
    const std::string&                   type_separator
)
{
    const auto& file               = expected_file_type_entry.first;
    const auto& expected_file_type = expected_file_type_entry.second;
    return std::format(
        "{}{}{}",
        file.string(),
        type_separator,
        ToString(expected_file_type)
    );
}

std::string ToString(
    const Magic::ExpectedFileTypeMapT& expected_file_type_map,
    const std::string&                 type_separator,
    const std::string&                 file_separator
)
{
    return Utility::ToString(
        expected_file_type_map,
        file_separator,
        [&type_separator](
            const Magic::ExpectedFileTypeEntryT& file_type_entry
        ) {
            return ToString(file_type_entry, type_separator);
        }
    );
}

std::string ToString(Magic::Flags flag)
{
    if (flag == Magic::Flags::None) {
        return Magic::MagicPrivate::LIBMAGIC_FLAG_NONE.second;
    }
    const auto& flags = Magic::MagicPrivate::LIBMAGIC_FLAGS;
    const auto& flag_name{flags[std::log2(std::to_underlying(flag))].second};
    return flag_name;
}

std::string ToString(
    const Magic::FlagsContainerT& flags,
    const std::string&            separator
)
{
    return Utility::ToString(flags, separator, [](Magic::Flags flag) {
        return ToString(flag);
    });
}

std::string ToString(Magic::Parameters parameter)
{
    const auto& parameters = Magic::MagicPrivate::LIBMAGIC_PARAMETERS;
    const auto& parameter_name{
        parameters[std::to_underlying(parameter)].second
    };
    return parameter_name;
}

std::string ToString(
    const Magic::ParameterValueT& parameter_value,
    const std::string&            value_separator
)
{
    const auto& parameter = parameter_value.first;
    const auto& value     = parameter_value.second;
    return std::format("{}{}{}", ToString(parameter), value_separator, value);
}

std::string ToString(
    const Magic::ParameterValueMapT& parameters,
    const std::string&               value_separator,
    const std::string&               parameter_separator
)
{
    return Utility::ToString(
        parameters,
        parameter_separator,
        [&value_separator](const Magic::ParameterValueT& parameter_value) {
            return ToString(parameter_value, value_separator);
        }
    );
}

/* ===========================================================================
 * Magic Class Method Implementations
 * ===========================================================================
 *
 * These are the public API implementations that delegate to MagicPrivate.
 * Each method follows the throwing/noexcept dual API pattern.
 *
 * See magic.hpp for the complete public API documentation with usage examples.
 */

std::string_view Magic::DEFAULT_DATABASE_FILE{MAGIC_DEFAULT_DATABASE_FILE};

Magic::Magic() noexcept
  : m_impl{std::make_unique<MagicPrivate>()}
{ }

Magic::Magic(FlagsMaskT flags_mask, const std::filesystem::path& database_file)
  : m_impl{std::make_unique<MagicPrivate>(flags_mask, database_file)}
{ }

Magic::Magic(
    FlagsMaskT                   flags_mask,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<MagicPrivate>(flags_mask, tag, database_file)}
{ }

Magic::Magic(
    const FlagsContainerT&       flags_container,
    const std::filesystem::path& database_file
)
  : m_impl{std::make_unique<MagicPrivate>(flags_container, database_file)}
{ }

Magic::Magic(
    const FlagsContainerT&       flags_container,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<MagicPrivate>(flags_container, tag, database_file)}
{ }

Magic::Magic(Magic&& other) noexcept
  : m_impl{std::exchange(other.m_impl, std::make_unique<MagicPrivate>())}
{ }

Magic& Magic::operator=(Magic&& other) noexcept
{
    m_impl = std::exchange(other.m_impl, std::make_unique<MagicPrivate>());
    return *this;
}

Magic::~Magic() = default;

Magic::operator bool() const noexcept
{
    return IsValid();
}

bool Magic::Check(const std::filesystem::path& database_file) noexcept
{
    MagicPrivate magic_for_check{};
    return magic_for_check.Open(Magic::Flags::None, std::nothrow)
        && magic_for_check.Check(database_file);
}

void Magic::Close() noexcept
{
    m_impl = std::make_unique<MagicPrivate>();
}

bool Magic::Compile(const std::filesystem::path& database_file) noexcept
{
    MagicPrivate magic_for_compile{};
    return magic_for_compile.Open(Magic::Flags::None, std::nothrow)
        && magic_for_compile.Compile(database_file);
}

Magic::FlagsContainerT Magic::GetFlags() const
{
    return m_impl->GetFlags();
}

std::optional<Magic::FlagsContainerT> Magic::GetFlags(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetFlags(tag);
}

std::size_t Magic::GetParameter(Magic::Parameters parameter) const
{
    return m_impl->GetParameter(parameter);
}

std::optional<std::size_t> Magic::GetParameter(
    Parameters            parameter,
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetParameter(parameter, tag);
}

Magic::ParameterValueMapT Magic::GetParameters() const
{
    return m_impl->GetParameters();
}

std::optional<Magic::ParameterValueMapT> Magic::GetParameters(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetParameters(tag);
}

std::string Magic::GetVersion() noexcept
{
    return std::format("{:2}", Detail::magic_version() / 100.);
}

Magic::FileTypeT Magic::IdentifyFile(const std::filesystem::path& path) const
{
    return m_impl->IdentifyFile(
        path,
        MagicPrivate::IdentifyFileOptions::CheckEverything
    );
}

Magic::ExpectedFileTypeT Magic::IdentifyFile(
    const std::filesystem::path& path,
    const std::nothrow_t&        tag
) const noexcept
{
    return m_impl->IdentifyFile(
        path,
        MagicPrivate::IdentifyFileOptions::CheckEverything,
        tag
    );
}

Magic::FileTypeMapT Magic::IdentifyDirectoryImpl(
    const std::filesystem::path&       directory,
    std::filesystem::directory_options option,
    ProgressTrackerT                   progress_tracker
) const
{
    return m_impl->IdentifyFiles(
        m_impl->IdentifyDirectoryPreliminaryChecks(
            directory,
            option,
            progress_tracker
        ),
        MagicPrivate::IdentifyFileOptions::CheckNothing,
        progress_tracker
    );
}

Magic::ExpectedFileTypeMapT Magic::IdentifyDirectoryImpl(
    const std::filesystem::path&       directory,
    const std::nothrow_t&              tag,
    std::filesystem::directory_options option,
    ProgressTrackerT                   progress_tracker
) const noexcept
{
    auto files = m_impl->IdentifyDirectoryPreliminaryChecks(
        directory,
        tag,
        option,
        progress_tracker
    );
    Utility::MarkTrackerAsCompleted completion_marker{progress_tracker};
    if (!files) {
        return {};
    }
    return m_impl->IdentifyFiles(
        files.value(),
        tag,
        MagicPrivate::IdentifyFileOptions::CheckNothing,
        progress_tracker
    );
}

Magic::FileTypeMapT Magic::IdentifyContainerImpl(
    const DefaultFileContainerT& files,
    ProgressTrackerT             progress_tracker
) const
{
    m_impl->IdentifyContainerPreliminaryChecks(progress_tracker);
    return m_impl->IdentifyFiles(
        files,
        MagicPrivate::IdentifyFileOptions::CheckPath,
        progress_tracker
    );
}

Magic::ExpectedFileTypeMapT Magic::IdentifyContainerImpl(
    const DefaultFileContainerT& files,
    const std::nothrow_t&        tag,
    ProgressTrackerT             progress_tracker
) const noexcept
{
    Utility::MarkTrackerAsCompleted completion_marker{progress_tracker};
    if (!m_impl->IdentifyContainerPreliminaryChecks(tag, progress_tracker)) {
        return {};
    }
    return m_impl->IdentifyFiles(
        files,
        tag,
        MagicPrivate::IdentifyFileOptions::CheckPathExists,
        progress_tracker
    );
}

bool Magic::IsDatabaseLoaded() const noexcept
{
    return m_impl->IsDatabaseLoaded();
}

bool Magic::IsOpen() const noexcept
{
    return m_impl->IsOpen();
}

bool Magic::IsValid() const noexcept
{
    return m_impl->IsValid();
}

void Magic::LoadDatabaseFile(const std::filesystem::path& database_file)
{
    m_impl->LoadDatabaseFile(database_file);
}

bool Magic::LoadDatabaseFile(
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
{
    return m_impl->LoadDatabaseFile(tag, database_file);
}

void Magic::Open(FlagsMaskT flags_mask)
{
    m_impl->Open(flags_mask);
}

bool Magic::Open(FlagsMaskT flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->Open(flags_mask, tag);
}

void Magic::Open(const FlagsContainerT& flags_container)
{
    m_impl->Open(flags_container);
}

bool Magic::Open(
    const FlagsContainerT& flags_container,
    const std::nothrow_t&  tag
) noexcept
{
    return m_impl->Open(flags_container, tag);
}

void Magic::SetFlags(FlagsMaskT flags_mask)
{
    m_impl->SetFlags(flags_mask);
}

bool Magic::SetFlags(FlagsMaskT flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->SetFlags(flags_mask, tag);
}

void Magic::SetFlags(const FlagsContainerT& flags_container)
{
    m_impl->SetFlags(flags_container);
}

bool Magic::SetFlags(
    const FlagsContainerT& flags_container,
    const std::nothrow_t&  tag
) noexcept
{
    return m_impl->SetFlags(flags_container, tag);
}

void Magic::SetParameter(Parameters parameter, std::size_t value)
{
    m_impl->SetParameter(parameter, value);
}

bool Magic::SetParameter(
    Parameters            parameter,
    std::size_t           value,
    const std::nothrow_t& tag
) noexcept
{
    return m_impl->SetParameter(parameter, value, tag);
}

void Magic::SetParameters(const ParameterValueMapT& parameters)
{
    m_impl->SetParameters(parameters);
}

bool Magic::SetParameters(
    const ParameterValueMapT& parameters,
    const std::nothrow_t&     tag
) noexcept
{
    return m_impl->SetParameters(parameters, tag);
}
} /* namespace Recognition */
