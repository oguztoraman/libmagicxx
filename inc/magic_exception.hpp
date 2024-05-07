/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef MAGIC_EXCEPTION_HPP
#define MAGIC_EXCEPTION_HPP

#include <string>
#include <stdexcept>

namespace recognition {

/**
 * @class magic_exception
 *
 * @brief The base class for all exceptions thrown by the magic class.
 */
class magic_exception : public std::runtime_error {
public:

    /**
     * @brief Construct magic_exception with an error message.
     *
     * @param[in] error     The description of the error.
     */
    explicit magic_exception(const std::string& error)
        : std::runtime_error{error}
    { }

    /**
     * @brief Construct magic_exception with an error message
     *        and the name of the funtion where the error occurred.
     *
     * @param[in] function  The name of the funtion.
     * @param[in] error     The description of the error.
     */
    magic_exception(const std::string& function, const std::string& error)
        : std::runtime_error{
            error.empty() ?
            function + " failed." :
            function + " failed with " + error + "."
        }
    { }
};

class empty_path final : public magic_exception {
public:
    empty_path()
        : magic_exception{"path is empty."}
    { }
};

class invalid_path final : public magic_exception {
public:
    invalid_path()
        : magic_exception{"path is invalid."}
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
    magic_load_error(const std::string& error, const std::string& database_file_path)
        : magic_exception{"magic_load(" + database_file_path + ")", error}
    { }
};

class magic_file_error final : public magic_exception {
public:
    magic_file_error(const std::string& error, const std::string& file_path)
        : magic_exception{"magic_file(" + file_path + ")", error}
    { }
};

class magic_set_flags_error final : public magic_exception {
public:
    magic_set_flags_error(const std::string& error, const std::string& flag_names)
        : magic_exception{"magic_set_flags(" + flag_names + ")", error}
    { }
};

class magic_set_param_error final : public magic_exception {
public:
    magic_set_param_error(const std::string& error, const std::string& parameter_name)
        : magic_exception{"magic_set_param(" + parameter_name + ")", error}
    { }
};

} /* namespace recognition */

#endif /* MAGIC_EXCEPTION_HPP */
