/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef MAGIC_HPP
#define MAGIC_HPP

#include <map>
#include <iosfwd>
#include <bitset>
#include <vector>
#include <memory>
#include <expected>

#include <file_concepts.hpp>
#include <magic_exception.hpp>

namespace recognition {

/**
 * @class magic
 *
 * @brief The magic class provides a C++ wrapper over the Magic Number Recognition Library.
 *        The magic class is used to identify the type of a file, if the following steps have been completed;
 *        1. magic must be opened.
 *        2. A magic database file must be loaded.
 */
class magic {
public:

    /**
     * @brief The flags_t typedef.
     */
    using flags_t = std::bitset<30uz>;

    /**
     * @brief The file_type_t typedef.
     */
    using file_type_t = std::string;

    /**
     * @brief The error_message_t typedef.
     */
    using error_message_t = std::string;

    /**
     * @brief The expected_file_type_t typedef.
     */
    using expected_file_type_t = std::expected<file_type_t, error_message_t>;

    /**
     * @brief The types_of_files_t typedef.
     */
    using types_of_files_t = std::map<std::filesystem::path, file_type_t>;

    /**
     * @brief The expected_types_of_files_t typedef.
     */
    using expected_types_of_files_t = std::map<std::filesystem::path, expected_file_type_t>;

    /**
     * @brief The Flag enums are used for configuring the flags of a magic.
     *
     * @note The Flag enums are suitable for bitwise or operations.
     */
    enum Flag : unsigned long long {
        None            = 0ULL,
        Debug           = 1ULL << 0,
        Symlink         = 1ULL << 1,
        Compress        = 1ULL << 2,
        Devices         = 1ULL << 3,
        MimeType        = 1ULL << 4,
        Continue        = 1ULL << 5,
        Check           = 1ULL << 6,
        PreserveAtime   = 1ULL << 7,
        Raw             = 1ULL << 8,
        Error           = 1ULL << 9,
        MimeEncoding    = 1ULL << 10,
        Mime            = 1ULL << 11,
        Apple           = 1ULL << 12,
        Extension       = 1ULL << 13,
        CompressTransp  = 1ULL << 14,
        NoCompressFork  = 1ULL << 15,
        Nodesc          = 1ULL << 16,
        NoCheckCompress = 1ULL << 17,
        NoCheckTar      = 1ULL << 18,
        NoCheckSoft     = 1ULL << 19,
        NoCheckApptype  = 1ULL << 20,
        NoCheckElf      = 1ULL << 21,
        NoCheckText     = 1ULL << 22,
        NoCheckCdf      = 1ULL << 23,
        NoCheckCsv      = 1ULL << 24,
        NoCheckTokens   = 1ULL << 25,
        NoCheckEncoding = 1ULL << 26,
        NoCheckJson     = 1ULL << 27,
        NoCheckSimh     = 1ULL << 28,
        NoCheckBuiltin  = 1ULL << 29
    };

    /**
     * @brief The Parameter enums are used for displaying or
     *        modifying the parameters of a magic.
     */
    enum class Parameter : std::size_t {
        IndirMax    = 0uz,
        NameMax     ,
        ElfPhnumMax ,
        ElfShnumMax ,
        ElfNotesMax ,
        RegexMax    ,
        BytesMax    ,
        EncodingMax ,
        ElfShsizeMax
    };

    /**
     * @brief The Flags typedef.
     */
    using Flags = std::vector<Flag>;

    /**
     * @brief The Parameters typedef.
     */
    using Parameters = std::map<Parameter, std::size_t>;

    /**
     * @brief The path of the default database file.
     */
    static constexpr auto default_database_file = "/usr/share/misc/magic";

    /**
     * @brief Construct magic without opening it.
     */
    magic() noexcept;

    /**
     * @brief Construct magic, open it using the flags and load the magic database file.
     *
     * @param[in] flags             One of the Flag enums or bitwise or of the Flag enums.
     * @param[in] database_file     The path of magic database file, default is /usr/share/misc/magic.
     *
     * @throws magic_open_error     if opening magic fails.
     * @throws invalid_path         if the path of the magic database file is not a file.
     * @throws magic_load_error     if loading the magic database file fails.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    explicit magic(flags_t flags, const std::filesystem::path& database_file = default_database_file);

    /**
     * @brief Move construct magic.
     *
     * @note other is valid as a default constructed magic.
     */
    magic(magic&& other) noexcept;

    /**
     * @brief Deleted copy constructor.
     */
    magic(const magic&) = delete;

    /**
     * @brief Move assign to this magic.
     *
     * @note other is valid as a default constructed magic.
     */
    magic& operator=(magic&& other) noexcept;

    /**
     * @brief Deleted copy assignment.
     */
    magic& operator=(const magic&) = delete;

    /**
     * @brief Destruct magic.
     */
    ~magic();

    /**
     * @brief Used for testing whether magic is open or closed.
     *
     * @returns True if magic is open, false otherwise.
     */
    [[nodiscard]]
    operator bool() const noexcept;

    /**
     * @brief Check the validity of entries in the colon separated database
     *        files passed in as database_file.
     *
     * @param[in] database_file     The file to check, default is /usr/share/misc/magic.
     *
     * @returns True if the database_file has valid entries, false otherwise.
     */
    bool check(const std::filesystem::path& database_file = default_database_file) const noexcept;

    /**
     * @brief Close magic.
     *
     * @note magic is valid as a default constructed magic.
     */
    void close() noexcept;

    /**
     * @brief Compile the colon separated list of database files
     *        passed in as database_file.
     *
     * @param[in] database_file     The file to compile, default is /usr/share/misc/magic.
     *
     * @returns True on success, false otherwise.

     * @note The compiled files created are named from the basename
     *       of each file argument with “.mgc” appended to it.
     */
    bool compile(const std::filesystem::path& database_file = default_database_file) const noexcept;

    /**
     * @brief Get the flags of magic.
     *
     * @returns Flags
     *
     * @throws magic_is_closed      if magic is closed.
     */
    [[nodiscard]]
    Flags get_flags() const;

    /**
     * @brief Get the value of a parameter of magic.
     *
     * @param[in] parameter         One of the Parameter enum.
     *
     * @returns Value of the desired parameter.
     *
     * @throws magic_is_closed      if magic is closed.
     */
    [[nodiscard]]
    std::size_t get_parameter(Parameter parameter) const;

    /**
     * @brief Get the values ​​of all parameters of magic.
     *
     * @returns <Parameter, value> map.
     *
     * @throws magic_is_closed      if magic is closed.
     */
    [[nodiscard]]
    Parameters get_parameters() const;

    /**
     * @brief Get the version of the Magic Number Recognition Library.
     *
     * @returns The version number as a string.
     */
    [[nodiscard]]
    static std::string get_version() noexcept;

    /**
     * @brief Identify the type of a file.
     *
     * @param[in] path              The path of the file.
     *
     * @returns The type of the file as a string.
     *
     * @throws magic_is_closed      if magic is closed.
     * @throws empty_path           if the path of the file is empty.
     * @throws magic_file_error     if identifying the type of the file fails.
     */
    [[nodiscard]]
    file_type_t identify_file(const std::filesystem::path& path) const;

    /**
     * @brief Identify the type of a file, noexcept version.
     *
     * @param[in] path              The path of the file.
     *
     * @returns The type of the file or the error message.
     */
    [[nodiscard]]
    expected_file_type_t
        identify_file(const std::filesystem::path& path, std::nothrow_t) const noexcept;

    /**
     * @brief Identify the types of all files in a directory.
     *
     * @param[in] directory         The path of the directory.
     * @param[in] option            The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     *
     * @throws magic_is_closed      if magic is closed.
     * @throws empty_path           if the path of the file is empty.
     * @throws magic_file_error     if identifying the type of the file fails.
     */
    [[nodiscard]]
    types_of_files_t identify_files(
        const std::filesystem::path& directory,
        std::filesystem::directory_options option = std::filesystem::directory_options::follow_directory_symlink
    ) const
    {
        return identify_files_impl(
            std::filesystem::recursive_directory_iterator{directory, option}
        );
    }

    /**
     * @brief Identify the types of all files in a directory, noexcept version.
     *
     * @param[in] directory         The path of the directory.
     * @param[in] option            The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     */
    [[nodiscard]]
    expected_types_of_files_t identify_files(
        const std::filesystem::path& directory, std::nothrow_t,
        std::filesystem::directory_options option = std::filesystem::directory_options::follow_directory_symlink
    ) const noexcept
    {
        return identify_files_impl(
            std::filesystem::recursive_directory_iterator{directory, option},  std::nothrow
        );
    }

    /**
     * @brief Identify the types of files.
     *
     * @param[in] files             The container that holds the paths of the files.
     *
     * @returns The types of each file as a map.
     *
     * @throws magic_is_closed      if magic is closed.
     * @throws empty_path           if the path of the file is empty.
     * @throws magic_file_error     if identifying the type of the file fails.
     */
    [[nodiscard]]
    types_of_files_t identify_files(const file_concepts::file_container auto& files) const
    {
        return identify_files_impl(files);
    }

    /**
     * @brief Identify the types of files, noexcept version.
     *
     * @param[in] files             The container that holds the paths of the files.
     *
     * @returns The types of each file as a map.
     */
    [[nodiscard]]
    expected_types_of_files_t identify_files(
        const file_concepts::file_container auto& files, std::nothrow_t
    ) const noexcept
    {
        return identify_files_impl(files, std::nothrow);
    }

    /**
     * @brief Used for testing whether magic is open or closed.
     *
     * @returns True if magic is open, false otherwise.
     */
    [[nodiscard]]
    bool is_open() const noexcept;

    /**
     * @brief Load a magic database file.
     *
     * @param[in] database_file     The path of the magic database file, default is /usr/share/misc/magic.
     *
     * @throws magic_is_closed      if magic is closed.
     * @throws empty_path           if the path of the database file is empty.
     * @throws invalid_path         if the path of the database file is not a file.
     * @throws magic_load_error     if loading the database file fails.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    void load_database_file(const std::filesystem::path& database_file = default_database_file);

    /**
     * @brief Open magic using the flags.
     *
     * @param[in] flags             One of the Flag enums or bitwise or of the Flag enums.
     *
     * @throws magic_open_error     if opening magic fails.
     *
     * @note If magic is open, it will be reopened using the flags after closing it.
     */
    void open(flags_t flags);

    /**
     * @brief Set the flags of magic.
     *
     * @param[in] flags               One of the Flag enums or bitwise or of the Flag enums.
     *
     * @throws magic_is_closed        if magic is closed.
     * @throws magic_set_flags_error  if setting the flags of magic fails.
     */
    void set_flags(flags_t flags);

    /**
     * @brief Set the value of a parameter of magic.
     *
     * @param[in] parameter           One of the Parameter enum.
     * @param[in] value               The value of the parameter.
     *
     * @throws magic_is_closed        if magic is closed.
     * @throws magic_set_param_error  if setting the parameter of magic fails.
     */
    void set_parameter(Parameter parameter, std::size_t value);

private:
    class magic_private;
    std::unique_ptr<magic_private> m_impl;

    [[nodiscard]]
    types_of_files_t identify_files_impl(const std::ranges::range auto& files) const
    {
        types_of_files_t types_of_files;
        std::ranges::for_each(files,
            [&](const std::filesystem::path& file){
                types_of_files[file] = identify_file(file);
            }
        );
        return types_of_files;
    }

    [[nodiscard]]
    expected_types_of_files_t identify_files_impl(const std::ranges::range auto& files, std::nothrow_t) const noexcept
    {
        expected_types_of_files_t expected_types_of_files;
        std::ranges::for_each(files,
            [&](const std::filesystem::path& file){
                expected_types_of_files[file] = identify_file(file, std::nothrow);
            }
        );
        return expected_types_of_files;
    }

    friend std::string to_string(Flag);
    friend std::string to_string(Parameter);
};

/**
 * @brief Operator<< for the magic::types_of_files_t.
 *        The format is "The path of a file -> The type of the file".
 *
 * @param[out] os                   The output stream.
 * @param[in]  types_of_files       The types of each file.
 *
 * @returns os.
 */
inline std::ostream& operator<<(std::ostream& os, const magic::types_of_files_t& types_of_files)
{
    std::ranges::for_each(types_of_files,
        [&](const auto& type_of_a_file){
            const auto& file = type_of_a_file.first;
            const auto& file_type = type_of_a_file.second;
            os << file << " -> " << file_type << "\n";
        }
    );
    return os;
}

/**
 * @brief Operator<< for the magic::expected_file_type_t.
 *
 * @param[out] os                       The output stream.
 * @param[in]  expected_file_type       The expected type of the file.
 *
 * @returns os.
 */
inline std::ostream& operator<<(std::ostream& os, const magic::expected_file_type_t& expected_file_type)
{
    auto type_or_error_message = expected_file_type.value_or(expected_file_type.error());
    return os << type_or_error_message;
}

/**
 * @brief Operator<< for the magic::expected_types_of_files_t.
 *        The format is "The path of a file -> The type of the file or the error message".
 *
 * @param[out] os                       The output stream.
 * @param[in]  expected_types_of_files  The types of each file.
 *
 * @returns os.
 */
inline std::ostream& operator<<(std::ostream& os, const magic::expected_types_of_files_t& expected_types_of_files)
{
    std::ranges::for_each(expected_types_of_files,
        [&](const auto& expected_type_of_a_file){
            const auto& file = expected_type_of_a_file.first;
            const auto& expected_type = expected_type_of_a_file.second;
            os << file << " -> " << expected_type << "\n";
        }
    );
    return os;
}

/**
 * @brief Convert the magic::Flag to string.
 * 
 * @param[in] flag                  The flag.
 *
 * @returns The flag as a string.
 */
std::string to_string(magic::Flag flag);

/**
 * @brief Convert the magic::Flags to string.
 * 
 * @param[in] flags                 The flags.
 * @param[in] separator             The separator between the flags, default is ", ".
 *
 * @returns The flags as a string.
 */
std::string to_string(const magic::Flags& flags, const std::string& separator = ", ");

/**
 * @brief Convert the magic::Parameter to string.
 * 
 * @param[in] parameter             The parameter.
 *
 * @returns The parameter as a string.
 */
std::string to_string(magic::Parameter parameter);

/**
 * @brief Convert the magic::Parameters to string.
 * 
 * @param[in] parameters            The parameters with corresponding values.
 * @param[in] value_separator       The separator between the parameter and its value, default is ": ".
 * @param[in] parameter_separator   The separator between the parameters, default is ", ".
 *
 * @returns The parameters as a string.
 */
std::string to_string(
    const magic::Parameters& parameters,
    const std::string& value_separator = ": ",
    const std::string& parameter_separator = ", "
);

} /* namespace recognition */

#endif /* MAGIC_HPP */
