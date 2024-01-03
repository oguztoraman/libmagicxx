/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef MAGIC_HPP
#define MAGIC_HPP

#include <map>
#include <bitset>
#include <vector>
#include <optional>

#include <file_concepts.hpp>
#include <magic_exception.hpp>

namespace recognition {

/**
 * @brief Namespace alias for the std::filesystem.
 */
namespace std_fs = std::filesystem;

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
    using flags_t = std::bitset<27uz>;

    /**
     * @brief The file_type_t typedef.
     */
    using file_type_t = std::string;

    /**
     * @brief The types_of_files_t typedef.
     */
    using types_of_files_t = std::map<std_fs::path, file_type_t>;

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
        Nodesc          = 1ULL << 15,
        NoCheckCompress = 1ULL << 16,
        NoCheckTar      = 1ULL << 17,
        NoCheckSoft     = 1ULL << 18,
        NoCheckApptype  = 1ULL << 19,
        NoCheckElf      = 1ULL << 20,
        NoCheckText     = 1ULL << 21,
        NoCheckCdf      = 1ULL << 22,
        NoCheckCsv      = 1ULL << 23,
        NoCheckTokens   = 1ULL << 24,
        NoCheckEncoding = 1ULL << 25,
        NoCheckJson     = 1ULL << 26
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
        EncodingMax
    };

    /**
     * @brief Construct magic without opening it.
     */
    magic() noexcept;

    /**
     * @brief Construct magic, open it using the flags and load the magic database file.
     *
     * @param[in] flags         One of the Flag enums or bitwise or of the Flag enums.
     * @param[in] database_file The path of magic database file (default is empty path).
     *
     * @throws magic_open       if opening magic fails.
     * @throws invalid_path     if the path of the magic database file is not a file.
     * @throws magic_load       if loading the magic database file fails.
     *
     * @note If the path of the database file is empty, magic loads the default database file.
     *       The default database file is named by the MAGIC environment variable.
     *       If that variable is not set, the default database file name is /usr/share/misc/magic.
     *       load_database_file() adds “.mgc” to the database filename as appropriate. (v5.44)
     */
    explicit magic(flags_t flags, const std_fs::path& database_file = {});

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
     * @param[in] database_file The file to check, default is an empty path.
     *
     * @returns True if the database_file has valid entries, false otherwise.
     *
     * @throws magic_is_closed  if magic is closed.
     *
     * @note If the database_file is empty, the default database file is used.
     */
    bool check(const std_fs::path& database_file = {}) const;

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
     * @param[in] database_file The file to compile, default is an empty path.
     *
     * @returns True on success, false otherwise.
     *
     * @throws magic_is_closed  if magic is closed.
     *
     * @note If the database_file is empty, the default database is used.
     *       The compiled files created are named from the basename
     *       of each file argument with “.mgc” appended to it.
     */
    bool compile(const std_fs::path& database_file = {}) const;

    /**
     * @brief Get the flags of magic.
     *
     * @returns Flags
     *
     * @throws magic_is_closed  if magic is closed.
     */
    [[nodiscard]]
    std::vector<Flag> get_flags() const;

    /**
     * @brief Get the value of a parameter of magic.
     *
     * @param[in] parameter     One of the Parameter enum.
     *
     * @returns Value of the desired parameter.
     *
     * @throws magic_is_closed  if magic is closed.
     */
    [[nodiscard]]
    std::size_t get_parameter(Parameter parameter) const;

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
     * @param[in] path          The path of the file.
     *
     * @returns The type of the file as a string.
     *
     * @throws magic_is_closed  if magic is closed.
     * @throws empty_path       if the path of the file is empty.
     * @throws magic_file       if identifying the type of the file fails.
     */
    [[nodiscard]]
    file_type_t identify_file(const std_fs::path& path) const;

    /**
     * @brief Identify the type of a file, noexcept version.
     *
     * @param[in] path          The path of the file.
     * @param[out] error        The out parameter for reporting the error message, if any.
     *
     * @returns The type of the file as a string. It does not contain a value on failure.
     */
    [[nodiscard]]
    std::optional<file_type_t>
        identify_file(const std_fs::path& path, std::string& error) const noexcept;

    /**
     * @brief Identify the types of all files in a directory.
     *
     * @param[in] directory     The path of the directory.
     * @param[in] option        The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     *
     * @throws magic_is_closed  if magic is closed.
     * @throws empty_path       if the path of the file is empty.
     * @throws magic_file       if identifying the type of the file fails.
     */
    [[nodiscard]]
    types_of_files_t identify_files(
        const std_fs::path& directory,
        std_fs::directory_options option = std_fs::directory_options::follow_directory_symlink
    ) const
    {
        return identify_files(
            std_fs::recursive_directory_iterator{directory, option}
        );
    }

    /**
     * @brief Identify the types of all files in a directory, noexcept version.
     *
     * @tparam ContainerType    The type of the error container.
     *
     * @param[in] directory     The path of the directory.
     * @param[out] errors       The out parameter for reporting the error messages, if any.
     * @param[in] option        The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     */
    template <error_container ContainerType>
    [[nodiscard]]
    types_of_files_t identify_files(
        const std_fs::path& directory, ContainerType& errors,
        std_fs::directory_options option = std_fs::directory_options::follow_directory_symlink
    ) const noexcept
    {
        return identify_files(
            std_fs::recursive_directory_iterator{directory, option}, errors
        );
    }

    /**
     * @brief Identify the types of files.
     *
     * @tparam ContainerType    The type of the file container.
     *
     * @param[in] files         The container that holds the paths of the files.
     *
     * @returns The types of each file as a map.
     *
     * @throws magic_is_closed  if magic is closed.
     * @throws empty_path       if the path of the file is empty.
     * @throws magic_file       if identifying the type of the file fails.
     */
    template <file_container ContainerType>
    [[nodiscard]]
    types_of_files_t identify_files(const ContainerType& files) const
    {
        return identify_files(files);
    }

    /**
     * @brief Identify the types of files, noexcept version.
     *
     * @tparam FileContainerType   The type of the file container.
     * @tparam ErrorContainerType  The type of the error container.
     *
     * @param[in] files            The container that holds the paths of the files.
     * @param[out] errors          The out parameter for reporting the error messages, if any.
     *
     * @returns The types of each file as a map.
     */
    template <file_container FileContainerType, error_container ErrorContainerType>
    [[nodiscard]]
    types_of_files_t identify_files(
        const FileContainerType& files, ErrorContainerType& errors
    ) const noexcept
    {
        return identify_files(files, errors);
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
     * @param[in] database_file The path of the magic database file, default is an empty path.
     *
     * @throws magic_is_closed  if magic is closed.
     * @throws invalid_path     if the path of the database file is not a file.
     * @throws magic_load       if loading the database file fails.
     *
     * @note If the path of the database file is empty, magic loads the default database file.
     *       The default database file is named by the MAGIC environment variable.
     *       If that variable is not set, the default database file name is /usr/share/misc/magic.
     *       load_database_file() adds “.mgc” to the database filename as appropriate. (v5.44)
     */
    void load_database_file(const std_fs::path& database_file = {});

    /**
     * @brief Open magic using the flags.
     *
     * @param[in] flags         One of the Flag enums or bitwise or of the Flag enums.
     *
     * @throws magic_open       if opening magic fails.
     *
     * @note If magic is open, it will be reopened using the flags after closing it.
     */
    void open(flags_t flags);

    /**
     * @brief Set the flags of magic.
     *
     * @param[in] flags         One of the Flag enums or bitwise or of the Flag enums.
     *
     * @throws magic_is_closed  if magic is closed.
     * @throws magic_set_flags  if setting the flags of magic fails.
     */
    void set_flags(flags_t flags);

    /**
     * @brief Set the value of a parameter of magic.
     *
     * @param[in] parameter     One of the Parameter enum.
     * @param[in] value         The value of the parameter.
     *
     * @throws magic_is_closed  if magic is closed.
     * @throws magic_set_param  if setting the parameter of magic fails.
     */
    void set_parameter(Parameter parameter, std::size_t value);

private:
    class magic_private;
    std::unique_ptr<magic_private> m_impl;

    template <std::ranges::range RangeType>
    [[nodiscard]]
    types_of_files_t identify_files(const RangeType& files) const
    {
        types_of_files_t types_of_files;
        std::ranges::for_each(files,
            [&](const std_fs::path& file){
                types_of_files[file] = identify_file(file);
            }
        );
        return types_of_files;
    }

    template <std::ranges::range RangeType, error_container ErrorContainerType>
    [[nodiscard]]
    types_of_files_t identify_files(const RangeType& files, ErrorContainerType& errors) const noexcept
    {
        errors.clear();
        types_of_files_t types_of_files;
        std::ranges::find_if(files,
            [&](const std_fs::path& file){
                try {
                    types_of_files[file] = identify_file(file);
                    return false; /* continue */
                } catch (const magic_is_closed& e){
                    errors.push_back(file.string() + " -> " + e.what());
                    return true;  /* do not continue */
                } catch (const magic_exception& e){
                    errors.push_back(file.string() + " -> " + e.what());
                    return false; /* continue */
                } catch (const std::exception& e){
                    errors.push_back(e.what());
                    return true; /* do not continue */
                }
            }
        );
        return types_of_files;
    }
};

/**
 * @brief Operator<< for the magic::types_of_files_t.
 *        The format is "The path of a file -> The type of the file".
 *
 * @param[out] os               The output stream.
 * @param[in]  types_of_files   The types of each file.
 *
 * @returns os.
 */
inline std::ostream& operator<<(std::ostream& os, const magic::types_of_files_t& types_of_files)
{
    std::ranges::for_each(types_of_files,
        [&](const auto& type_of_a_file){
            os << type_of_a_file.first << " -> " << type_of_a_file.second << "\n";
        }
    );
    return os;
}

} /* namespace recognition */

#endif /* MAGIC_HPP */
