/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef MAGIC_EXCEPTION_HPP
#define MAGIC_EXCEPTION_HPP

#include <format>
#include <stdexcept>
#include <string>

namespace Recognition {
/**
 * @class MagicException
 *
 * @brief The base class for all exceptions thrown by the Magic class.
 */
class MagicException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    /**
     * @brief Construct MagicException with an error message
     *        and the name of the funtion where the error occurred.
     *
     * @param[in] function              The name of the funtion.
     * @param[in] error_message         The description of the error.
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
 *
 * @brief Exception thrown when a shared ProgressTracker is null.
 */
class NullTracker final : public MagicException {
public:
    /**
     * @brief Construct NullTracker.
     */
    NullTracker()
      : MagicException{"shared ProgressTracker is null."}
    { }
};

/**
 * @class EmptyPath
 *
 * @brief Exception thrown when a path is empty.
 */
class EmptyPath final : public MagicException {
public:
    /**
     * @brief Construct EmptyPath.
     */
    EmptyPath()
      : MagicException{"path is empty."}
    { }
};

/**
 * @class PathIsNotRegularFile
 *
 * @brief Exception thrown from Magic::LoadDatabaseFile
 *        when the database file path is not a regular file.
 */
class PathIsNotRegularFile final : public MagicException {
public:
    /**
     * @brief Construct PathIsNotRegularFile with
     *        the database file path that is not a regular file.
     *
     * @param[in] database_file_path    The database file path that is not a regular file.
     */
    explicit PathIsNotRegularFile(const std::string& path)
      : MagicException{std::format("'{}' is not a regular file.", path)}
    { }
};

/**
 * @class PathIsNotDirectory
 *
 * @brief Exception thrown from Magic::IdentifyFiles(directory)
 *        when the path is not a directory.
 */
class PathIsNotDirectory final : public MagicException {
public:
    /**
     * @brief Construct PathIsNotDirectory with
     *        the path that is not a directory.
     *
     * @param[in] path                  The path that is not a regular file.
     */
    explicit PathIsNotDirectory(const std::string& path)
      : MagicException{std::format("'{}' is not a directory.", path)}
    { }
};

/**
 * @class PathDoesNotExist
 *
 * @brief Exception thrown when a path does not exist.
 */
class PathDoesNotExist final : public MagicException {
public:
    /**
     * @brief Construct PathDoesNotExist with the path that does not exist.
     *
     * @param[in] path                  The path that does not exist.
     */
    explicit PathDoesNotExist(const std::string& path)
      : MagicException{std::format("'{}' does not exist.", path)}
    { }
};

/**
 * @class FilesystemError
 *
 * @brief Exception thrown when the underlying std::filesystem OS API fails.
 */
class FilesystemError final : public MagicException {
public:
    /**
     * @brief Construct FilesystemError with a path and an error message.
     *
     * @param[in] path                  The path related to the error.
     * @param[in] error_message         The description of the error.
     */
    FilesystemError(const std::string& path, const std::string& error_message)
      : MagicException{std::format("'{}': {}.", path, error_message)}
    { }
};

/**
 * @class MagicIsClosed
 *
 * @brief Exception thrown when Magic is closed.
 */
class MagicIsClosed final : public MagicException {
public:
    /**
     * @brief Construct MagicIsClosed.
     */
    MagicIsClosed()
      : MagicException{"Magic is closed."}
    { }
};

/**
 * @class MagicOpenError
 *
 * @brief Exception thrown when Magic::Open fails.
 */
class MagicOpenError final : public MagicException {
public:
    /**
     * @brief Construct MagicOpenError with an error message.
     *
     * @param[in] error_message         The description of the error.
     */
    explicit MagicOpenError(const std::string& error_message)
      : MagicException{"magic_open", error_message}
    { }
};

/**
 * @class MagicLoadDatabaseFileError
 *
 * @brief Exception thrown when Magic::LoadDatabaseFile fails.
 */
class MagicLoadDatabaseFileError final : public MagicException {
public:
    /**
     * @brief Construct MagicLoadDatabaseFileError with an error message
     *        and the path of the database file.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] database_file_path    The path of the magic database file.
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
 *
 * @brief Exception thrown when magic database is not loaded.
 */
class MagicDatabaseNotLoaded final : public MagicException {
public:
    /**
     * @brief Construct MagicDatabaseNotLoaded.
     */
    MagicDatabaseNotLoaded()
      : MagicException{"magic database is not loaded."}
    { }
};

/**
 * @class MagicIdentifyFileError
 *
 * @brief Exception thrown when Magic::IdentifyFile(s) fails.
 */
class MagicIdentifyFileError final : public MagicException {
public:
    /**
     * @brief Construct MagicIdentifyFileError with an error message,
     *        and the path of the file.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] file_path             The path of the file.
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
 * @class MagicSetFlagsError
 *
 * @brief Exception thrown when Magic::SetFlags fails.
 */
class MagicSetFlagsError final : public MagicException {
public:
    /**
     * @brief Construct MagicSetFlagsError with an error message
     *        and the names of the flags.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] flag_names            The names of the flags.
     */
    MagicSetFlagsError(
        const std::string& error_message,
        const std::string& flag_names
    )
      : MagicException{
            std::format("Magic::SetFlags({})", flag_names),
            error_message
        }
    { }
};

/**
 * @class MagicSetParameterError
 *
 * @brief Exception thrown when Magic::SetParameter(s) fails.
 */
class MagicSetParameterError final : public MagicException {
public:
    /**
     * @brief Construct MagicSetParameterError with an error message,
     *        the name of the parameter and its value.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] parameter_name        The name of the parameter.
     * @param[in] value                 The value of the parameter.
     */
    MagicSetParameterError(
        const std::string& error_message,
        const std::string& parameter_name,
        std::size_t        value
    )
      : MagicException{
            std::format("Magic::SetParameter({}, {})", parameter_name, value),
            error_message
        }
    { }
};
} /* namespace Recognition */

#endif /* MAGIC_EXCEPTION_HPP */
