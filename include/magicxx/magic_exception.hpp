/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_exception.hpp
 * @brief Exception hierarchy for the Magic file identification library.
 * @ingroup magic_exceptions
 *
 * This file defines the exception classes used by the Magic class to report
 * errors. All exceptions derive from MagicException, which itself derives
 * from std::runtime_error.
 *
 * @section exception_hierarchy Exception Hierarchy
 *
 * ```
 * std::runtime_error
 *  └── MagicException (base class for all Magic errors)
 *       ├── NullTracker              - Progress tracker is null
 *       ├── EmptyPath                - Path is empty
 *       ├── PathIsNotRegularFile     - Path is not a regular file
 *       ├── PathIsNotDirectory       - Path is not a directory
 *       ├── PathDoesNotExist         - Path does not exist
 *       ├── FilesystemError          - Filesystem operation failed
 *       ├── MagicIsClosed            - Magic instance is closed
 *       ├── MagicOpenError           - Failed to open Magic
 *       ├── MagicLoadDatabaseFileError - Failed to load database
 *       ├── MagicDatabaseNotLoaded   - Database not loaded
 *       ├── MagicIdentifyFileError   - Failed to identify file
 *       ├── MagicFlagsError          - Flag operation failed (base, set or get)
 *       │    ├── MagicSetFlagsError  - Magic::SetFlags() failed
 *       │    └── MagicGetFlagsError  - Magic::GetFlags() failed
 *       └── MagicParametersError     - Parameter operation failed (base, set or get)
 *            ├── MagicSetParametersError - Magic::SetParameter(s)() failed
 *            └── MagicGetParametersError - Magic::GetParameter(s)() failed
 * ```
 *
 * @section exception_handling Exception Handling Patterns
 *
 * Catch specific exceptions for targeted error handling:
 * @code{.cpp}
 * try {
 *     auto type = magic.IdentifyFile("/nonexistent/file.txt");
 * } catch (const Recognition::PathDoesNotExist& e) {
 *     std::cerr << "File not found: " << e.what() << '\n';
 * } catch (const Recognition::MagicIdentifyFileError& e) {
 *     std::cerr << "Identification failed: " << e.what() << '\n';
 * } catch (const Recognition::MagicException& e) {
 *     std::cerr << "Magic error: " << e.what() << '\n';
 * }
 * @endcode
 *
 * Or use noexcept overloads to avoid exceptions entirely:
 * @code{.cpp}
 * auto result = magic.IdentifyFile("/path/to/file", std::nothrow);
 * if (!result) {
 *     std::cerr << "Error: " << result.error() << '\n';
 * }
 * @endcode
 *
 * @see Magic
 */

#ifndef MAGIC_EXCEPTION_HPP
#define MAGIC_EXCEPTION_HPP

#include <format>
#include <stdexcept>
#include <string>

#include "magic_flags.hpp"
#include "magic_parameters.hpp"

namespace Recognition {
/**
 * @defgroup magic_exceptions Exception Classes
 * @ingroup magic_core
 * @brief Exception hierarchy for Magic class error handling.
 *
 * All exceptions thrown by the Magic class derive from MagicException,
 * allowing catch-all handling while also supporting specific error handling.
 *
 * @{
 */

/**
 * @class MagicException
 * @ingroup magic_exceptions
 *
 * @brief Base exception class for all Magic-related errors.
 *
 * MagicException inherits from std::runtime_error and provides a common
 * base class for all exceptions thrown by the Magic class. This allows
 * users to catch all Magic errors with a single catch block.
 *
 * @section magic_exception_example Example
 *
 * @code{.cpp}
 * try {
 *     magic.LoadDatabaseFile("/invalid/path");
 * } catch (const Recognition::MagicException& e) {
 *     std::cerr << "Magic operation failed: " << e.what() << '\n';
 * }
 * @endcode
 *
 * @see std::runtime_error
 *
 * @since 10.0.0
 */
class MagicException : public std::runtime_error {
public:
    /** @brief Inherit constructors from std::runtime_error. */
    using std::runtime_error::runtime_error;

    /**
     * @brief Construct MagicException with function name and error message.
     *
     * Creates an exception message in the format:
     * - "function failed." (if error_message is empty)
     * - "function failed with error_message." (otherwise)
     *
     * @param[in] function      The name of the function where the error occurred.
     * @param[in] error_message Description of the error (optional).
     *
     * @since 10.0.0
     */
    MagicException(
        const std::string& function,
        const std::string& error_message
    )
      : std::runtime_error{
            error_message.empty()
                ? function + " failed."
                : function + " failed with " + error_message + "."
        }
    { }
};

/**
 * @class NullTracker
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a shared ProgressTracker is null.
 *
 * This exception is thrown when a method that requires a progress tracker
 * receives a null shared pointer.
 *
 * @see ProgressTracker
 * @see Magic::IdentifyFiles()
 *
 * @since 10.0.0
 */
class NullTracker final : public MagicException {
public:
    /**
     * @brief Construct NullTracker exception.
     *
     * @since 10.0.0
     */
    NullTracker()
      : MagicException{"shared ProgressTracker is null."}
    { }
};

/**
 * @class EmptyPath
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a path argument is empty.
 *
 * This exception is thrown when a file or directory path is required
 * but an empty path is provided.
 *
 * @since 10.0.0
 */
class EmptyPath final : public MagicException {
public:
    /**
     * @brief Construct EmptyPath exception.
     *
     * @since 10.0.0
     */
    EmptyPath()
      : MagicException{"path is empty."}
    { }
};

/**
 * @class PathIsNotRegularFile
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a path is expected to be a regular file but is not.
 *
 * This exception is thrown by Magic::LoadDatabaseFile() when the specified
 * database file path exists but is not a regular file (e.g., it's a directory).
 *
 * @see Magic::LoadDatabaseFile()
 *
 * @since 10.0.0
 */
class PathIsNotRegularFile final : public MagicException {
public:
    /**
     * @brief Construct PathIsNotRegularFile with the invalid path.
     *
     * @param[in] path The path that is not a regular file.
     *
     * @since 10.0.0
     */
    explicit PathIsNotRegularFile(const std::string& path)
      : MagicException{std::format("'{}' is not a regular file.", path)}
    { }
};

/**
 * @class PathIsNotDirectory
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a path is expected to be a directory but is not.
 *
 * This exception is thrown by Magic::IdentifyFiles() when the specified
 * directory path exists but is not a directory (e.g., it's a regular file).
 *
 * @see Magic::IdentifyFiles()
 *
 * @since 10.0.0
 */
class PathIsNotDirectory final : public MagicException {
public:
    /**
     * @brief Construct PathIsNotDirectory with the invalid path.
     *
     * @param[in] path The path that is not a directory.
     *
     * @since 10.0.0
     */
    explicit PathIsNotDirectory(const std::string& path)
      : MagicException{std::format("'{}' is not a directory.", path)}
    { }
};

/**
 * @class PathDoesNotExist
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a specified path does not exist.
 *
 * This exception is thrown when any operation requires a path that
 * must exist (file or directory) but the path cannot be found.
 *
 * @since 10.0.0
 */
class PathDoesNotExist final : public MagicException {
public:
    /**
     * @brief Construct PathDoesNotExist with the missing path.
     *
     * @param[in] path The path that does not exist.
     *
     * @since 10.0.0
     */
    explicit PathDoesNotExist(const std::string& path)
      : MagicException{std::format("'{}' does not exist.", path)}
    { }
};

/**
 * @class FilesystemError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a filesystem operation fails.
 *
 * This exception wraps errors from std::filesystem operations that
 * fail due to OS-level issues (permissions, I/O errors, etc.).
 *
 * @since 10.0.0
 */
class FilesystemError final : public MagicException {
public:
    /**
     * @brief Construct FilesystemError with path and error description.
     *
     * @param[in] path          The path related to the error.
     * @param[in] error_message Description of the filesystem error.
     *
     * @since 10.0.0
     */
    FilesystemError(const std::string& path, const std::string& error_message)
      : MagicException{std::format("'{}': {}.", path, error_message)}
    { }
};

/**
 * @class MagicIsClosed
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when an operation is attempted on a closed Magic instance.
 *
 * This exception is thrown when methods like IdentifyFile(), SetFlags(),
 * or LoadDatabaseFile() are called on a Magic instance that has been closed.
 *
 * @code{.cpp}
 * Magic magic;
 * magic.Close();
 * magic.IdentifyFile("file.txt");  // Throws MagicIsClosed
 * @endcode
 *
 * @see Magic::Close()
 * @see Magic::IsOpen()
 *
 * @since 10.0.0
 */
class MagicIsClosed final : public MagicException {
public:
    /**
     * @brief Construct MagicIsClosed exception.
     *
     * @since 10.0.0
     */
    MagicIsClosed()
      : MagicException{"Magic is closed."}
    { }
};

/**
 * @class MagicOpenError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when Magic::Open() fails.
 *
 * This exception indicates that the underlying magic library could not
 * be initialized, typically due to memory allocation failures.
 *
 * @see Magic::Open()
 *
 * @since 10.0.0
 */
class MagicOpenError final : public MagicException {
public:
    /**
     * @brief Construct MagicOpenError with error description.
     *
     * @param[in] error_message Description of why opening failed.
     *
     * @since 10.0.0
     */
    explicit MagicOpenError(const std::string& error_message)
      : MagicException{"magic_open", error_message}
    { }
};

/**
 * @class MagicLoadDatabaseFileError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when Magic::LoadDatabaseFile() fails.
 *
 * This exception indicates that the magic database file could not be loaded.
 * Common causes include invalid or corrupted database files.
 *
 * @see Magic::LoadDatabaseFile()
 *
 * @since 10.0.0
 */
class MagicLoadDatabaseFileError final : public MagicException {
public:
    /**
     * @brief Construct MagicLoadDatabaseFileError with details.
     *
     * @param[in] error_message      Description of why loading failed.
     * @param[in] database_file_path Path to the database file that failed to load.
     *
     * @since 10.0.0
     */
    MagicLoadDatabaseFileError(
        const std::string& error_message,
        const std::string& database_file_path
    )
      : MagicException{
            std::format("Magic::LoadDatabaseFile({})", database_file_path),
            error_message
        }
    { }
};

/**
 * @class MagicDatabaseNotLoaded
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when file identification is attempted without a loaded database.
 *
 * This exception is thrown when IdentifyFile() or IdentifyFiles() is called
 * but no magic database has been loaded via LoadDatabaseFile().
 *
 * @code{.cpp}
 * Magic magic;
 * magic.Open(Magic::FlagsT::Mime);
 * // Forgot to call LoadDatabaseFile()!
 * magic.IdentifyFile("file.txt");  // Throws MagicDatabaseNotLoaded
 * @endcode
 *
 * @see Magic::LoadDatabaseFile()
 * @see Magic::IsDatabaseLoaded()
 *
 * @since 10.0.0
 */
class MagicDatabaseNotLoaded final : public MagicException {
public:
    /**
     * @brief Construct MagicDatabaseNotLoaded exception.
     *
     * @since 10.0.0
     */
    MagicDatabaseNotLoaded()
      : MagicException{"magic database is not loaded."}
    { }
};

/**
 * @class MagicIdentifyFileError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when file type identification fails.
 *
 * This exception is thrown when the underlying magic library cannot
 * determine the type of a file. The error message includes details
 * from the magic library.
 *
 * @see Magic::IdentifyFile()
 * @see Magic::IdentifyFiles()
 *
 * @since 10.0.0
 */
class MagicIdentifyFileError final : public MagicException {
public:
    /**
     * @brief Construct MagicIdentifyFileError with details.
     *
     * @param[in] error_message Description of why identification failed.
     * @param[in] file_path     Path to the file that could not be identified.
     *
     * @since 10.0.0
     */
    MagicIdentifyFileError(
        const std::string& error_message,
        const std::string& file_path
    )
      : MagicException{
            std::format("Magic::IdentifyFile({})", file_path),
            error_message
        }
    { }
};

/**
 * @class MagicFlagsError
 * @ingroup magic_exceptions
 *
 * @brief Base exception for all Magic flag operations.
 *
 * MagicFlagsError is the common base for the exceptions reported by the
 * flag operations of the Magic class: MagicSetFlagsError (writing flags
 * via Magic::SetFlags()) and MagicGetFlagsError (reading flags via
 * Magic::GetFlags()). Catching MagicFlagsError handles every flag
 * operation failure with a single catch block. Failures include unknown
 * flag bits and libmagic errors.
 *
 * @see MagicSetFlagsError
 * @see MagicGetFlagsError
 * @see Magic::SetFlags()
 * @see Magic::GetFlags()
 * @see Magic::FlagsT
 *
 * @since 11.0.0
 */
class MagicFlagsError : public MagicException {
public:
    /**
     * @brief Construct MagicFlagsError with function context and details.
     *
     * @param[in] function      Name of the flag operation that failed.
     * @param[in] error_message Description of why the operation failed.
     *
     * @since 11.0.0
     */
    MagicFlagsError(
        const std::string& function,
        const std::string& error_message
    )
      : MagicException{function, error_message}
    { }
};

/**
 * @class MagicSetFlagsError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a Magic flag write operation fails.
 *
 * This exception is reported by Magic::SetFlags() when the specified
 * flags could not be applied, for example because the value contains
 * unknown flag bits or because the underlying libmagic call failed.
 *
 * @see MagicFlagsError
 * @see Magic::SetFlags()
 *
 * @since 11.0.0
 */
class MagicSetFlagsError final : public MagicFlagsError {
public:
    /**
     * @brief Construct MagicSetFlagsError with the flags that were set.
     *
     * @param[in] error_message Description of why setting flags failed.
     * @param[in] flags         Flag value set that could not be applied.
     *
     * @since 11.0.0
     */
    MagicSetFlagsError(
        const std::string& error_message,
        const MagicFlags&  flags
    )
      : MagicFlagsError{
            std::format("Magic::SetFlags({})", flags.ToString()),
            error_message
        }
    { }
};

/**
 * @class MagicGetFlagsError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a Magic flag read operation fails.
 *
 * This exception is reported by Magic::GetFlags(). It carries only an
 * error message because Magic::GetFlags() takes no arguments.
 *
 * @see MagicFlagsError
 * @see Magic::GetFlags()
 *
 * @since 11.0.0
 */
class MagicGetFlagsError final : public MagicFlagsError {
public:
    /**
     * @brief Construct MagicGetFlagsError with details.
     *
     * @param[in] error_message Description of why reading flags failed.
     *
     * @since 11.0.0
     */
    explicit MagicGetFlagsError(const std::string& error_message)
      : MagicFlagsError{"Magic::GetFlags()", error_message}
    { }
};

/**
 * @class MagicParametersError
 * @ingroup magic_exceptions
 *
 * @brief Base exception for all Magic parameter operations.
 *
 * MagicParametersError is the common base for the exceptions reported by
 * the parameter operations of the Magic class: MagicSetParametersError
 * (writing via Magic::SetParameter() / Magic::SetParameters()) and
 * MagicGetParametersError (reading via Magic::GetParameter() /
 * Magic::GetParameters()). Catching MagicParametersError handles every
 * parameter operation failure with a single catch block. Failures
 * include out-of-range parameter values and libmagic errors.
 *
 * @see MagicSetParametersError
 * @see MagicGetParametersError
 * @see Magic::SetParameter()
 * @see Magic::GetParameter()
 * @see Magic::ParametersT
 *
 * @since 11.0.0
 */
class MagicParametersError : public MagicException {
public:
    /**
     * @brief Construct MagicParametersError with function context and details.
     *
     * @param[in] function      Name of the parameter operation that failed.
     * @param[in] error_message Description of why the operation failed.
     *
     * @since 11.0.0
     */
    MagicParametersError(
        const std::string& function,
        const std::string& error_message
    )
      : MagicException{function, error_message}
    { }
};

/**
 * @class MagicSetParametersError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a Magic parameter write operation fails.
 *
 * This exception is reported by Magic::SetParameter() and
 * Magic::SetParameters() when the specified parameter could not be set,
 * for example because the parameter value is out of range or because the
 * underlying libmagic call failed.
 *
 * @see MagicParametersError
 * @see Magic::SetParameter()
 * @see Magic::SetParameters()
 *
 * @since 11.0.0
 */
class MagicSetParametersError final : public MagicParametersError {
public:
    /**
     * @brief Construct MagicSetParametersError with the attempted values.
     *
     * MagicParameters holds both the parameters and their values, so no
     * separate value argument is needed.
     *
     * @param[in] error_message Description of why setting failed.
     * @param[in] parameters    Parameter-value snapshot of the attempt.
     *
     * @since 11.0.0
     */
    MagicSetParametersError(
        const std::string&     error_message,
        const MagicParameters& parameters
    )
      : MagicParametersError{
            std::format("Magic::SetParameter({})", parameters.ToString()),
            error_message
        }
    { }
};

/**
 * @class MagicGetParametersError
 * @ingroup magic_exceptions
 *
 * @brief Exception thrown when a Magic parameter read operation fails.
 *
 * This exception is reported by Magic::GetParameter() and
 * Magic::GetParameters() when the specified parameter could not be read,
 * for example because the parameter value is out of range or because the
 * underlying libmagic call failed. Read operations carry no value, so
 * only the parameter name is reported.
 *
 * @see MagicParametersError
 * @see Magic::GetParameter()
 * @see Magic::GetParameters()
 *
 * @since 11.0.0
 */
class MagicGetParametersError final : public MagicParametersError {
public:
    /**
     * @brief Construct MagicGetParametersError with the parameter read.
     *
     * @param[in] error_message Description of why reading failed.
     * @param[in] parameter     The parameter that could not be read.
     *
     * @since 11.0.0
     */
    MagicGetParametersError(
        const std::string&          error_message,
        MagicParameters::Parameters parameter
    )
      : MagicParametersError{
            std::format(
                "Magic::GetParameter({})",
                MagicParameters{parameter, 0UZ}.ToString(
                    MagicParameters::StringFormat::Names
                )
            ),
            error_message
        }
    { }
};

/** @} magic_exceptions */
} /* namespace Recognition */

#endif /* MAGIC_EXCEPTION_HPP */
