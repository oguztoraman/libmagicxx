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
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman
 * @license LGPL-3.0-only
 */

#include <cmath>

#include <array>
#include <utility>

#include "magic.hpp"

namespace Recognition {
/**
 * @namespace Recognition::Detail
 * @brief Internal namespace for libmagic C library integration.
 *
 * This namespace isolates the libmagic C header to prevent symbol
 * pollution in client code. All libmagic functions are accessed
 * through this namespace within the implementation.
 *
 * @internal
 */
namespace Detail {
#include <magic.h>
} /* namespace Detail */

/**
 * @class Magic::MagicPrivate
 * @brief PIMPL implementation class for Magic.
 *
 * MagicPrivate encapsulates all libmagic interactions and maintains
 * the internal state of a Magic instance. It provides:
 *
 * - **Cookie Management**: RAII wrapper for `magic_t` handle
 * - **Flag Conversion**: Bidirectional conversion between C++ and C flags
 * - **Error Handling**: Template-based exception throwing
 * - **State Tracking**: `m_cookie` and `m_is_database_loaded` flags
 *
 * @section magic_private_members Member Variables
 *
 * | Member | Type | Purpose |
 * |--------|------|---------|
 * | `m_cookie` | `CookieT` | RAII-managed libmagic handle |
 * | `m_flags_mask` | `FlagsMaskT` | Current configuration flags |
 * | `m_is_database_loaded` | `bool` | Database load state |
 *
 * @section magic_private_constants Constants
 *
 * | Constant | Purpose |
 * |----------|---------|
 * | `LIBMAGIC_ERROR` | libmagic error return value (-1) |
 * | `LIBMAGIC_FLAGS` | Array mapping Flags enum to libmagic constants |
 * | `LIBMAGIC_PARAMETERS` | Array mapping Parameters enum to libmagic constants |
 *
 * @internal
 * @see Magic for the public interface
 */
class Magic::MagicPrivate {
public:
    /**
     * @brief Bitmask type for file identification options.
     */
    using IdentifyFileOptionsMaskT = std::bitset<3uz>;

    /**
     * @brief Options controlling file identification validation.
     *
     * These options determine which precondition checks are performed
     * before attempting to identify a file.
     */
    enum IdentifyFileOptions : unsigned long long {
        /* clang-format off */
        CheckNothing    = 0ULL,                              ///< Skip all checks
        CheckIsValid    = 1ULL << 0,                         ///< Verify instance is valid
        CheckPathEmpty  = 1ULL << 1,                         ///< Verify path is non-empty
        CheckPathExists = 1ULL << 2,                         ///< Verify path exists on disk
        CheckPath       = CheckPathEmpty | CheckPathExists,  ///< Check both path conditions
        CheckEverything = CheckIsValid | CheckPath           ///< Perform all validations
        /* clang-format on */
    };

    /// @brief Default constructor creates a closed instance.
    MagicPrivate() noexcept = default;

    /**
     * @brief Construct and initialize with flags (throwing version).
     * @param flags_mask Configuration flags.
     * @param database_file Path to magic database file.
     * @throws MagicOpenError If opening fails.
     * @throws MagicLoadDatabaseFileError If loading fails.
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
     * @param flags_mask Configuration flags.
     * @param tag Pass `std::nothrow` to select this overload.
     * @param database_file Path to magic database file.
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
     * @param flags_container Container of Flags values.
     * @param database_file Path to magic database file.
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
     * @param flags_container Container of Flags values.
     * @param tag Pass `std::nothrow` to select this overload.
     * @param database_file Path to magic database file.
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

    /// @brief Move constructor.
    MagicPrivate(MagicPrivate&&) noexcept = default;

    /// @brief Copy constructor (deleted - Magic is move-only).
    MagicPrivate(const MagicPrivate&) = delete;

    /// @brief Move assignment operator.
    MagicPrivate& operator=(MagicPrivate&&) noexcept = default;

    /// @brief Copy assignment operator (deleted - Magic is move-only).
    MagicPrivate& operator=(const MagicPrivate&) = delete;

    /// @brief Destructor - releases libmagic cookie via CookieT RAII.
    ~MagicPrivate() = default;

    /**
     * @brief Check if a magic database file is valid.
     * @param database_file Path to the database file to validate.
     * @return `true` if the database is valid, `false` otherwise.
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

    /**
     * @brief Compile a magic database from source.
     * @param database_file Path to the database source file.
     * @return `true` on success, `false` on failure.
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

    /// @brief Get current flags (throwing version).
    [[nodiscard]] FlagsContainerT GetFlags() const
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        return FlagsConverter(m_flags_mask);
    }

    /// @brief Get current flags (noexcept version).
    [[nodiscard]] std::optional<FlagsContainerT> GetFlags(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!IsOpen()) {
            return std::nullopt;
        }
        return {FlagsConverter(m_flags_mask)};
    }

    /// @brief Get a single parameter value (throwing version).
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

    /// @brief Get a single parameter value (noexcept version).
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

    /// @brief Get all parameter values (throwing version).
    [[nodiscard]] ParameterValueMapT GetParameters() const
    {
        ParameterValueMapT parameter_value_map;
        for (std::size_t i{}; i < LIBMAGIC_PARAMETER_COUNT; ++i) {
            auto parameter                 = static_cast<Parameters>(i);
            parameter_value_map[parameter] = GetParameter(parameter);
        }
        return parameter_value_map;
    }

    /// @brief Get all parameter values (noexcept version).
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

    /**
     * @brief Identify a single file's type (throwing version).
     * @param path Path to the file to identify.
     * @param options Validation options to check before identification.
     * @return The file type string.
     * @throws MagicIsClosed If instance is closed (when CheckIsValid set).
     * @throws PathDoesNotExist If file doesn't exist (when CheckPathExists set).
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
     * @param path Path to the file to identify.
     * @param options Validation options to check before identification.
     * @param tag Pass `std::nothrow` to select this overload.
     * @return Expected containing file type or error message.
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
     * @param directory Directory path to scan.
     * @param option Filesystem directory iteration options.
     * @param progress_tracker Tracker for progress updates.
     * @return Container of file paths found in the directory.
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
     * @param directory Directory path to scan.
     * @param tag Pass `std::nothrow` to select this overload.
     * @param option Filesystem directory iteration options.
     * @param progress_tracker Tracker for progress updates.
     * @return Optional container of file paths, nullopt on validation failure.
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
     * @param progress_tracker Tracker for progress updates.
     * @throws MagicIsClosed If instance is not valid.
     * @throws NullTracker If progress_tracker is null.
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
     * @param tag Pass `std::nothrow` to select this overload.
     * @param progress_tracker Tracker for progress updates.
     * @return `true` if valid and tracker is non-null, `false` otherwise.
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
     * @param files Container of file paths to identify.
     * @param option Validation options to apply per file.
     * @param progress_tracker Progress tracker to update.
     * @return Map of file paths to their identified types.
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
     * @param files Container of file paths to identify.
     * @param tag Pass `std::nothrow` to select this overload.
     * @param option Validation options to apply per file.
     * @param progress_tracker Progress tracker to update.
     * @return Map of file paths to expected file type results.
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

    /// @brief Check if a database is loaded (Valid state indicator).
    [[nodiscard]] bool IsDatabaseLoaded() const noexcept
    {
        return m_is_database_loaded;
    }

    /// @brief Check if instance is open (cookie is non-null).
    [[nodiscard]] bool IsOpen() const noexcept
    {
        return m_cookie != nullptr;
    }

    /// @brief Check if instance is valid (open AND database loaded).
    [[nodiscard]] bool IsValid() const noexcept
    {
        return IsOpen() && IsDatabaseLoaded();
    }

    /**
     * @brief Load a magic database file (throwing version).
     *
     * Transitions: Opened -> Valid
     *
     * @param database_file Path to the database file.
     * @throws MagicIsClosed If not in Opened state.
     * @throws MagicLoadDatabaseFileError If loading fails.
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
     * Transitions: Opened -> Valid on success
     *
     * @param tag Pass `std::nothrow` to select this overload.
     * @param database_file Path to the database file.
     * @return `true` on success, `false` on failure.
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

    /**
     * @brief Open the magic instance with flags (throwing version).
     *
     * Transitions:
     * - Closed -> Opened
     * - Opened or Valid -> Opened (reopens, unloads database)
     *
     * @param flags_mask Configuration flags.
     * @throws MagicOpenError If magic_open() fails.
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
     * @param flags_mask Configuration flags.
     * @param tag Pass `std::nothrow` to select this overload.
     * @return `true` on success, `false` on failure.
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

    /// @brief Open with flags container (throwing version).
    void Open(const FlagsContainerT& flags_container)
    {
        Open(FlagsMaskT{FlagsConverter(flags_container)});
    }

    /// @brief Open with flags container (noexcept version).
    [[nodiscard]] bool Open(
        const FlagsContainerT&                 flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return Open(FlagsMaskT{FlagsConverter(flags_container)}, std::nothrow);
    }

    /// @brief Set new flags for the instance (throwing version).
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

    /// @brief Set new flags for the instance (noexcept version).
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

    /// @brief Set new flags from container (throwing version).
    void SetFlags(const FlagsContainerT& flags_container)
    {
        SetFlags(FlagsMaskT{FlagsConverter(flags_container)});
    }

    /// @brief Set new flags from container (noexcept version).
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

    /// @brief Set a single parameter value (throwing version).
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

    /// @brief Set a single parameter value (noexcept version).
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

    /// @brief Set multiple parameter values (throwing version).
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

    /// @brief Set multiple parameter values (noexcept version).
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

    /**
     * @brief Throws an ExceptionT exception with ExceptionArgsT
     *        if the result is false or equal to LIBMAGIC_ERROR.
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

private:
    /**
     * @brief RAII cookie type wrapping libmagic handle.
     *
     * Uses a custom deleter lambda to call magic_close() on destruction.
     */
    using CookieT = std::unique_ptr<
        Detail::magic_set,
        decltype([](Detail::magic_t cookie) noexcept {
            Detail::magic_close(cookie);
        })
    >;

    CookieT    m_cookie{nullptr}; ///< libmagic handle (nullptr = Closed)
    FlagsMaskT m_flags_mask{0};   ///< Current configuration flags
    bool       m_is_database_loaded{
        false
    }; ///< True when database loaded successfully

    /// @name Validation option masks
    /// @{
    static constexpr IdentifyFileOptionsMaskT CHECK_IS_VALID_OPTION{
        IdentifyFileOptions::CheckIsValid
    };
    static constexpr IdentifyFileOptionsMaskT CHECK_PATH_EMPTY_OPTION{
        IdentifyFileOptions::CheckPathEmpty
    };
    static constexpr IdentifyFileOptionsMaskT CHECK_PATH_EXISTS_OPTION{
        IdentifyFileOptions::CheckPathExists
    };
    /// @}

    /// @name libmagic constants
    /// @{
    static constexpr auto LIBMAGIC_ERROR{-1}; ///< libmagic error return value
    static constexpr auto LIBMAGIC_FLAGS_COUNT{
        FlagsMaskT{}.size()
    }; ///< Number of flags
    static constexpr auto LIBMAGIC_PARAMETER_COUNT{
        10uz
    }; ///< Number of parameters
    /// @}

    /// @name libmagic type aliases
    /// @{
    using LibmagicValueT     = int;         ///< libmagic integer type
    using LibmagicValueNameT = std::string; ///< Flag/param name type
    using LibmagicPairT      = std::pair<
             LibmagicValueT,
             const char*
         >; ///< Value-name pair
    using LibmagicFlagsT      = std::array<LibmagicPairT, LIBMAGIC_FLAGS_COUNT>;
    using LibmagicParametersT = std::array<
        LibmagicPairT,
        LIBMAGIC_PARAMETER_COUNT
    >;
    /// @}

    /// @brief The MAGIC_NONE flag pair for default output.
    static constexpr LibmagicPairT LIBMAGIC_FLAG_NONE{
        std::make_pair(MAGIC_NONE, "None")
    };

    /// @brief Mapping from Magic::Flags bit positions to libmagic constants.
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

    /// @brief Mapping from Magic::Parameters enum to libmagic constants.
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

    /// @brief Retrieve the last error message from libmagic.
    [[nodiscard]] std::string GetErrorMessage() const noexcept
    {
        const char* magic_error_cstr = Detail::magic_error(m_cookie.get());
        return magic_error_cstr ? magic_error_cstr : "";
    }

    /**
     * @brief Converter between C++ flag types and libmagic flag values.
     *
     * Provides implicit conversions:
     * - FlagsContainerT -> LibmagicValueT (for magic_open, magic_setflags)
     * - FlagsMaskT -> LibmagicValueT
     * - FlagsMaskT -> FlagsContainerT
     * - FlagsMaskT -> LibmagicValueNameT (for ToString)
     */
    struct FlagsConverter {
        /// @brief Construct from a container of flags.
        explicit FlagsConverter(const FlagsContainerT& flags_container) noexcept
          : m_flags_mask{std::ranges::fold_left(
                flags_container,
                flags_container.empty() ? Flags::None : flags_container.front(),
                std::bit_or<decltype(1ULL)>{}
            )}
        { }

        /// @brief Construct from a flags bitmask.
        explicit FlagsConverter(FlagsMaskT flags_mask) noexcept
          : m_flags_mask{flags_mask}
        { }

        /// @brief Convert to libmagic integer flags for API calls.
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

        /// @brief Convert to container of individual flag values.
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

        /// @brief Convert to comma-separated flag names string.
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

        /// @brief Convert to bitmask representation.
        operator FlagsMaskT() const
        {
            return m_flags_mask;
        }

        const FlagsMaskT m_flags_mask; ///< The underlying bitmask
    };

    /**
     * @brief Extracts values from libmagic constant pairs.
     *
     * Provides implicit conversions from LibmagicPairT to:
     * - LibmagicValueT (the integer constant)
     * - LibmagicValueNameT (the string name)
     */
    struct LibmagicPairConverter {
        /// @brief Construct from a libmagic pair.
        constexpr explicit LibmagicPairConverter(
            const LibmagicPairT& pair
        ) noexcept
          : m_pair{pair}
        { }

        /// @brief Convert to string name.
        constexpr operator LibmagicValueNameT() const noexcept
        {
            return std::get<const char*>(m_pair);
        }

        /// @brief Convert to integer constant value.
        constexpr operator LibmagicValueT() const noexcept
        {
            return std::get<LibmagicValueT>(m_pair);
        }

        const LibmagicPairT& m_pair; ///< Reference to the pair
    };

    /// @name Friend declarations for ToString access to private constants
    /// @{
    friend std::string ToString(Flags);
    friend std::string ToString(Parameters);
    /// @}
};

/* ============================================================================
 * ToString() Free Function Implementations
 * ============================================================================
 *
 * These functions provide string conversion utilities for Magic types.
 * They are used for logging, debugging, and user-facing output.
 */

/**
 * @brief Convert a file type entry to string.
 * @param file_type_entry The file path and type pair.
 * @param type_separator Separator between path and type (default: ": ").
 * @return Formatted string "path<separator>type".
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

/**
 * @brief Convert a file type map to string.
 * @param file_type_map Map of file paths to their types.
 * @param type_separator Separator between path and type.
 * @param file_separator Separator between entries (default: newline).
 * @return Formatted multi-line string.
 */
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

/// @brief Convert expected file type to string (value or error message).
std::string ToString(const Magic::ExpectedFileTypeT& expected_file_type)
{
    return expected_file_type.value_or(expected_file_type.error());
}

/// @brief Convert an expected file type entry to string.
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

/// @brief Convert expected file type map to string.
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

/// @brief Convert a single flag to its string name.
std::string ToString(Magic::Flags flag)
{
    if (flag == Magic::Flags::None) {
        return Magic::MagicPrivate::LIBMAGIC_FLAG_NONE.second;
    }
    const auto& flags = Magic::MagicPrivate::LIBMAGIC_FLAGS;
    const auto& flag_name{flags[std::log2(std::to_underlying(flag))].second};
    return flag_name;
}

/// @brief Convert a container of flags to a separated string.
std::string ToString(
    const Magic::FlagsContainerT& flags,
    const std::string&            separator
)
{
    return Utility::ToString(flags, separator, [](Magic::Flags flag) {
        return ToString(flag);
    });
}

/// @brief Convert a parameter enum value to its string name.
std::string ToString(Magic::Parameters parameter)
{
    const auto& parameters = Magic::MagicPrivate::LIBMAGIC_PARAMETERS;
    const auto& parameter_name{
        parameters[std::to_underlying(parameter)].second
    };
    return parameter_name;
}

/// @brief Convert a parameter-value pair to string.
std::string ToString(
    const Magic::ParameterValueT& parameter_value,
    const std::string&            value_separator
)
{
    const auto& parameter = parameter_value.first;
    const auto& value     = parameter_value.second;
    return std::format("{}{}{}", ToString(parameter), value_separator, value);
}

/// @brief Convert a parameter-value map to a separated string.
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

/* ============================================================================
 * Magic Class Method Implementations
 * ============================================================================
 *
 * These are the public API implementations that delegate to MagicPrivate.
 * Each method follows the throwing/noexcept dual API pattern.
 */

/// @brief Default database file path from CMake configuration.
std::string_view Magic::DEFAULT_DATABASE_FILE{MAGIC_DEFAULT_DATABASE_FILE};

/// @brief Default constructor creates a Closed instance.
Magic::Magic() noexcept
  : m_impl{std::make_unique<MagicPrivate>()}
{ }

/// @brief Construct with flags and optional database (throwing).
Magic::Magic(FlagsMaskT flags_mask, const std::filesystem::path& database_file)
  : m_impl{std::make_unique<MagicPrivate>(flags_mask, database_file)}
{ }

/// @brief Construct with flags and database (noexcept).
Magic::Magic(
    FlagsMaskT                   flags_mask,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<MagicPrivate>(flags_mask, tag, database_file)}
{ }

/// @brief Construct with flags container (throwing).
Magic::Magic(
    const FlagsContainerT&       flags_container,
    const std::filesystem::path& database_file
)
  : m_impl{std::make_unique<MagicPrivate>(flags_container, database_file)}
{ }

/// @brief Construct with flags container (noexcept).
Magic::Magic(
    const FlagsContainerT&       flags_container,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<MagicPrivate>(flags_container, tag, database_file)}
{ }

/// @brief Move constructor - transfers ownership, source becomes Closed.
Magic::Magic(Magic&& other) noexcept
  : m_impl{std::exchange(other.m_impl, std::make_unique<MagicPrivate>())}
{ }

/// @brief Move assignment - transfers ownership, source becomes Closed.
Magic& Magic::operator=(Magic&& other) noexcept
{
    m_impl = std::exchange(other.m_impl, std::make_unique<MagicPrivate>());
    return *this;
}

/// @brief Destructor - releases libmagic resources via MagicPrivate.
Magic::~Magic() = default;

/// @brief Boolean conversion - returns IsValid().
Magic::operator bool() const noexcept
{
    return IsValid();
}

/// @brief Static database validation using a temporary instance.
bool Magic::Check(const std::filesystem::path& database_file) noexcept
{
    MagicPrivate magic_for_check{};
    return magic_for_check.Open(Magic::Flags::None, std::nothrow)
        && magic_for_check.Check(database_file);
}

/// @brief Close and reset to Closed state.
void Magic::Close() noexcept
{
    m_impl = std::make_unique<MagicPrivate>();
}

/// @brief Static database compilation using a temporary instance.
bool Magic::Compile(const std::filesystem::path& database_file) noexcept
{
    MagicPrivate magic_for_compile{};
    return magic_for_compile.Open(Magic::Flags::None, std::nothrow)
        && magic_for_compile.Compile(database_file);
}

/// @brief Delegate to MagicPrivate::GetFlags() throwing version.
Magic::FlagsContainerT Magic::GetFlags() const
{
    return m_impl->GetFlags();
}

/// @brief Delegate to MagicPrivate::GetFlags() noexcept version.
std::optional<Magic::FlagsContainerT> Magic::GetFlags(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetFlags(tag);
}

/// @brief Delegate to MagicPrivate::GetParameter() throwing version.
std::size_t Magic::GetParameter(Magic::Parameters parameter) const
{
    return m_impl->GetParameter(parameter);
}

/// @brief Delegate to MagicPrivate::GetParameter() noexcept version.
std::optional<std::size_t> Magic::GetParameter(
    Parameters            parameter,
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetParameter(parameter, tag);
}

/// @brief Delegate to MagicPrivate::GetParameters() throwing version.
Magic::ParameterValueMapT Magic::GetParameters() const
{
    return m_impl->GetParameters();
}

/// @brief Delegate to MagicPrivate::GetParameters() noexcept version.
std::optional<Magic::ParameterValueMapT> Magic::GetParameters(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetParameters(tag);
}

/// @brief Return underlying libmagic version string.
std::string Magic::GetVersion() noexcept
{
    return std::format("{:2}", Detail::magic_version() / 100.);
}

/// @brief Identify a file (throwing version), delegates to MagicPrivate.
Magic::FileTypeT Magic::IdentifyFile(const std::filesystem::path& path) const
{
    return m_impl->IdentifyFile(
        path,
        MagicPrivate::IdentifyFileOptions::CheckEverything
    );
}

/// @brief Identify a file (noexcept version), delegates to MagicPrivate.
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

/// @brief Identify all files in a directory (throwing implementation).
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

/// @brief Identify all files in a directory (noexcept implementation).
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

/// @brief Identify files from a container (throwing implementation).
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

/// @brief Identify files from a container (noexcept implementation).
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

/// @brief Delegate to MagicPrivate::IsDatabaseLoaded().
bool Magic::IsDatabaseLoaded() const noexcept
{
    return m_impl->IsDatabaseLoaded();
}

/// @brief Delegate to MagicPrivate::IsOpen().
bool Magic::IsOpen() const noexcept
{
    return m_impl->IsOpen();
}

/// @brief Delegate to MagicPrivate::IsValid().
bool Magic::IsValid() const noexcept
{
    return m_impl->IsValid();
}

/// @brief Delegate to MagicPrivate::LoadDatabaseFile() throwing version.
void Magic::LoadDatabaseFile(const std::filesystem::path& database_file)
{
    m_impl->LoadDatabaseFile(database_file);
}

/// @brief Delegate to MagicPrivate::LoadDatabaseFile() noexcept version.
bool Magic::LoadDatabaseFile(
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
{
    return m_impl->LoadDatabaseFile(tag, database_file);
}

/// @brief Delegate to MagicPrivate::Open() with mask (throwing).
void Magic::Open(FlagsMaskT flags_mask)
{
    m_impl->Open(flags_mask);
}

/// @brief Delegate to MagicPrivate::Open() with mask (noexcept).
bool Magic::Open(FlagsMaskT flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->Open(flags_mask, tag);
}

/// @brief Delegate to MagicPrivate::Open() with container (throwing).
void Magic::Open(const FlagsContainerT& flags_container)
{
    m_impl->Open(flags_container);
}

/// @brief Delegate to MagicPrivate::Open() with container (noexcept).
bool Magic::Open(
    const FlagsContainerT& flags_container,
    const std::nothrow_t&  tag
) noexcept
{
    return m_impl->Open(flags_container, tag);
}

/// @brief Delegate to MagicPrivate::SetFlags() with mask (throwing).
void Magic::SetFlags(FlagsMaskT flags_mask)
{
    m_impl->SetFlags(flags_mask);
}

/// @brief Delegate to MagicPrivate::SetFlags() with mask (noexcept).
bool Magic::SetFlags(FlagsMaskT flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->SetFlags(flags_mask, tag);
}

/// @brief Delegate to MagicPrivate::SetFlags() with container (throwing).
void Magic::SetFlags(const FlagsContainerT& flags_container)
{
    m_impl->SetFlags(flags_container);
}

/// @brief Delegate to MagicPrivate::SetFlags() with container (noexcept).
bool Magic::SetFlags(
    const FlagsContainerT& flags_container,
    const std::nothrow_t&  tag
) noexcept
{
    return m_impl->SetFlags(flags_container, tag);
}

/// @brief Delegate to MagicPrivate::SetParameter() throwing version.
void Magic::SetParameter(Parameters parameter, std::size_t value)
{
    m_impl->SetParameter(parameter, value);
}

/// @brief Delegate to MagicPrivate::SetParameter() noexcept version.
bool Magic::SetParameter(
    Parameters            parameter,
    std::size_t           value,
    const std::nothrow_t& tag
) noexcept
{
    return m_impl->SetParameter(parameter, value, tag);
}

/// @brief Delegate to MagicPrivate::SetParameters() throwing version.
void Magic::SetParameters(const ParameterValueMapT& parameters)
{
    m_impl->SetParameters(parameters);
}

/// @brief Delegate to MagicPrivate::SetParameters() noexcept version.
bool Magic::SetParameters(
    const ParameterValueMapT& parameters,
    const std::nothrow_t&     tag
) noexcept
{
    return m_impl->SetParameters(parameters, tag);
}
} /* namespace Recognition */
