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
     * @param[in] function  The name of the funtion.
     * @param[in] error     The description of the error.
     */
    magic_exception(const std::string& function, const std::string& error)
      : std::runtime_error{
            error.empty() ? function + " failed."
                          : function + " failed with " + error + "."
        }
    { }
};

class empty_path final : public magic_exception {
public:
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

class magic_is_closed final : public magic_exception {
public:
    magic_is_closed()
      : magic_exception{"magic is closed."}
    { }
};

class magic_open_error final : public magic_exception {
public:
    explicit magic_open_error(const std::string& error)
      : magic_exception{"magic_open", error}
    { }
};

class magic_load_error final : public magic_exception {
public:
    magic_load_error(
        const std::string& error,
        const std::string& database_file_path
    )
      : magic_exception{
            std::format("magic_load({})", database_file_path),
            error
        }
    { }
};

class magic_database_not_loaded final : public magic_exception {
public:
    magic_database_not_loaded()
      : magic_exception{"magic database is not loaded."}
    { }
};

class magic_file_error final : public magic_exception {
public:
    magic_file_error(const std::string& error, const std::string& file_path)
      : magic_exception{std::format("magic_file({})", file_path), error}
    { }
};

class magic_set_flags_error final : public magic_exception {
public:
    magic_set_flags_error(
        const std::string& error,
        const std::string& flag_names
    )
      : magic_exception{std::format("magic_set_flags({})", flag_names), error}
    { }
};

class magic_set_param_error final : public magic_exception {
public:
    magic_set_param_error(
        const std::string& error,
        const std::string& parameter_name,
        std::size_t        value
    )
      : magic_exception{
            std::format("magic_set_param({}, {})", parameter_name, value),
            error
        }
    { }
};

} /* namespace recognition */

#endif /* MAGIC_EXCEPTION_HPP */
