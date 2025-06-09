/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef MAGIC_EXCEPTION_HPP
#define MAGIC_EXCEPTION_HPP

#include <format>
#include <stdexcept>
#include <string>

namespace recognition {

/**
 * @class magic_exception
 *
 * @brief The base class for all exceptions thrown by the magic class.
 */
class magic_exception : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    /**
     * @brief Construct magic_exception with an error message
     *        and the name of the funtion where the error occurred.
     *
     * @param[in] function              The name of the funtion.
     * @param[in] error_message         The description of the error.
     */
    magic_exception(
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
 * @class empty_path
 *
 * @brief Exception thrown when a path is empty.
 */
class empty_path final : public magic_exception {
public:
    /**
     * @brief Construct empty_path.
     */
    empty_path()
      : magic_exception{"path is empty."}
    { }
};

/**
 * @class path_is_not_regular_file
 *
 * @brief Exception thrown from magic::load_database_file
 *        when the database file path is not a regular file.
 */
class path_is_not_regular_file final : public magic_exception {
public:
    /**
     * @brief Construct path_is_not_regular_file with
     *        the database file path that is not a regular file.
     *
     * @param[in] database_file_path    The database file path that is not a regular file.
     */
    explicit path_is_not_regular_file(const std::string& path)
      : magic_exception{std::format("'{}' is not a regular file.", path)}
    { }
};

/**
 * @class path_does_not_exist
 *
 * @brief Exception thrown when a path does not exist.
 */
class path_does_not_exist final : public magic_exception {
public:
    /**
     * @brief Construct path_does_not_exist with the path that does not exist.
     *
     * @param[in] path                  The path that does not exist.
     */
    explicit path_does_not_exist(const std::string& path)
      : magic_exception{std::format("'{}' does not exist.", path)}
    { }
};

/**
 * @class magic_is_closed
 *
 * @brief Exception thrown when magic is closed.
 */
class magic_is_closed final : public magic_exception {
public:
    /**
     * @brief Construct magic_is_closed.
     */
    magic_is_closed()
      : magic_exception{"magic is closed."}
    { }
};

/**
 * @class magic_open_error
 *
 * @brief Exception thrown when magic::open fails.
 */
class magic_open_error final : public magic_exception {
public:
    /**
     * @brief Construct magic_open_error with an error message.
     *
     * @param[in] error_message         The description of the error.
     */
    explicit magic_open_error(const std::string& error_message)
      : magic_exception{"magic_open", error_message}
    { }
};

/**
 * @class magic_load_database_file_error
 *
 * @brief Exception thrown when magic::load_database_file fails.
 */
class magic_load_database_file_error final : public magic_exception {
public:
    /**
     * @brief Construct magic_load_database_file_error with an error message
     *        and the path of the database file.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] database_file_path    The path of the magic database file.
     */
    magic_load_database_file_error(
        const std::string& error_message,
        const std::string& database_file_path
    )
      : magic_exception{
            std::format("magic_load_database_file({})", database_file_path),
            error_message
        }
    { }
};

/**
 * @class magic_database_not_loaded
 *
 * @brief Exception thrown when magic database is not loaded.
 */
class magic_database_not_loaded final : public magic_exception {
public:
    /**
     * @brief Construct magic_database_not_loaded.
     */
    magic_database_not_loaded()
      : magic_exception{"magic database is not loaded."}
    { }
};

/**
 * @class magic_identify_file_error
 *
 * @brief Exception thrown when magic::identify_file(s) fails.
 */
class magic_identify_file_error final : public magic_exception {
public:
    /**
     * @brief Construct magic_identify_file_error with an error message,
     *        and the path of the file.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] file_path             The path of the file.
     */
    magic_identify_file_error(
        const std::string& error_message,
        const std::string& file_path
    )
      : magic_exception{
            std::format("magic_identify_file({})", file_path),
            error_message
        }
    { }
};

/**
 * @class magic_set_flags_error
 *
 * @brief Exception thrown when magic::set_flags fails.
 */
class magic_set_flags_error final : public magic_exception {
public:
    /**
     * @brief Construct magic_set_flags_error with an error message
     *        and the names of the flags.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] flag_names            The names of the flags.
     */
    magic_set_flags_error(
        const std::string& error_message,
        const std::string& flag_names
    )
      : magic_exception{
            std::format("magic_set_flags({})", flag_names),
            error_message
        }
    { }
};

/**
 * @class magic_set_parameter_error
 *
 * @brief Exception thrown when magic::set_parameter(s) fails.
 */
class magic_set_parameter_error final : public magic_exception {
public:
    /**
     * @brief Construct magic_set_parameter_error with an error message,
     *        the name of the parameter and its value.
     *
     * @param[in] error_message         The description of the error.
     * @param[in] parameter_name        The name of the parameter.
     * @param[in] value                 The value of the parameter.
     */
    magic_set_parameter_error(
        const std::string& error_message,
        const std::string& parameter_name,
        std::size_t        value
    )
      : magic_exception{
            std::format("magic_set_parameter({}, {})", parameter_name, value),
            error_message
        }
    { }
};

} /* namespace recognition */

#endif /* MAGIC_EXCEPTION_HPP */
