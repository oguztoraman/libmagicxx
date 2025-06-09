/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef MAGIC_HPP
#define MAGIC_HPP

#include <bitset>
#include <expected>
#include <map>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "file_concepts.hpp"
#include "magic_exception.hpp"

namespace recognition {

/**
 * @class magic
 *
 * @brief The magic class provides a C++ wrapper over the Magic Number Recognition Library.
 *
 * @note  To use the magic class for file type identification, ensure the following:
 *        1. The magic instance must be opened.
 *        2. A magic database file must be successfully loaded.
 *        Only after these steps is the instance considered valid for identifying file types.
 */
class magic {
public:
    /**
     * @brief The flags_mask_t typedef.
     */
    using flags_mask_t = std::bitset<30uz>;

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
    using expected_types_of_files_t = std::map<
        std::filesystem::path,
        expected_file_type_t>;

    /**
     * @brief The flags enums are used for configuring the flags of a magic.
     *
     * @note The flags enums are suitable for bitwise or operations.
     */
    enum flags : unsigned long long {
        /* clang-format off */
        none              = 0ULL,       /**< No special handling. */
        debug             = 1ULL << 0,  /**< Print debugging messages to stderr. */
        symlink           = 1ULL << 1,  /**< If the file queried is a symlink, follow it. */
        compress          = 1ULL << 2,  /**< If the file is compressed, unpack it and look at the  contents. */
        devices           = 1ULL << 3,  /**< If the file is a block or character special device, then open the device and try to look in its contents. */
        mime_type         = 1ULL << 4,  /**< Return a MIME type string, instead of a  textual  description. */
        continue_search   = 1ULL << 5,  /**< Return all matches, not just the first. */
        check_database    = 1ULL << 6,  /**< Check the magic database for consistency and print warnings to stderr. */
        preserve_atime    = 1ULL << 7,  /**< On systems that support utime(3) or utimes(2), attempt to preserve the access time of files analysed. */
        raw               = 1ULL << 8,  /**< Don't translate unprintable characters to a \ooo octal representation. */
        error             = 1ULL << 9,  /**< Treat operating system errors while trying to open files and follow symlinks as real errors, instead of printing them in the magic buffer. */
        mime_encoding     = 1ULL << 10, /**< Return a MIME encoding, instead of a textual description. */
        mime              = 1ULL << 11, /**< A shorthand for mime_type|mime_encoding. */
        apple             = 1ULL << 12, /**< Return the Apple creator and type. */
        extension         = 1ULL << 13, /**< Return a slash-separated list of extensions for this file type. */
        compress_transp   = 1ULL << 14, /**< Don't report on compression, only report about the uncompressed data. */
        no_compress_fork  = 1ULL << 15, /**< Don't allow decompressors that use fork. */
        nodesc            = 1ULL << 16, /**< A shorthand for extension|mime|apple */
        no_check_compress = 1ULL << 17, /**< Don't look inside compressed files. */
        no_check_tar      = 1ULL << 18, /**< Don't examine tar files. */
        no_check_soft     = 1ULL << 19, /**< Don't consult magic files. */
        no_check_apptype  = 1ULL << 20, /**< Don't check for EMX application type (only on EMX). */
        no_check_elf      = 1ULL << 21, /**< Don't print ELF details. */
        no_check_text     = 1ULL << 22, /**< Don't check for various types of text files. */
        no_check_cdf      = 1ULL << 23, /**< Don't get extra information on MS Composite Document Files. */
        no_check_csv      = 1ULL << 24, /**< Don't examine CSV files. */
        no_check_tokens   = 1ULL << 25, /**< Don't look for known tokens inside ascii files. */
        no_check_encoding = 1ULL << 26, /**< Don't check text encodings. */
        no_check_json     = 1ULL << 27, /**< Don't examine JSON files. */
        no_check_simh     = 1ULL << 28, /**< Don't examine SIMH tape files. */
        no_check_builtin  = 1ULL << 29  /**< No built-in tests; only consult the magic file. */
        /* clang-format on */
    };

    /**
     * @brief The parameters enums are used for displaying or
     *        modifying the parameters of a magic.
     */
    enum class parameters : std::size_t {
        /* clang-format off */
        indir_max      = 0uz, /**< Recursion limit for indirection (default is 15). */
        name_max       = 1uz, /**< Use limit for name/use magic (default is 30). */
        elf_phnum_max  = 2uz, /**< Max ELF program sections processed (default is 128). */
        elf_shnum_max  = 3uz, /**< Max ELF sections processed (default is 32768). */
        elf_notes_max  = 4uz, /**< Max ELF notes processed (default is 256). */
        regex_max      = 5uz, /**< Max length limit for REGEX searches (default is 8192). */
        bytes_max      = 6uz, /**< Max bytes to look inside file (default is 7340032). */
        encoding_max   = 7uz, /**< Max bytes to scan for encoding (default is 1048576). */
        elf_shsize_max = 8uz, /**< Max ELF section size (default is 134217728). */
        mag_warn_max   = 9uz  /**< Max warnings to tolerate in a magic file (default is 64). */
        /* clang-format on */
    };

    /**
     * @brief The flags_container_t typedef.
     */
    using flags_container_t = std::vector<flags>;

    /**
     * @brief The parameter_value_map_t typedef.
     */
    using parameter_value_map_t = std::map<parameters, std::size_t>;

    /**
     * @brief The path of the default database file.
     */
    static std::string_view default_database_file;

    /**
     * @brief Construct magic without opening it.
     * 
     * @note This magic is not valid for identifying file types
     *       until it is opened and a magic database file is loaded.
     */
    magic() noexcept;

    /**
     * @brief Construct magic, open it using the flags and load the magic database file.
     *
     * @param[in] flags_mask        One of the flags enums or bitwise or of the flags enums.
     * @param[in] database_file     The path of magic database file, default is default_database_file.
     *
     * @throws magic_open_error                 if opening magic fails.
     * @throws empty_path                       if the path of the database file is empty.
     * @throws path_does_not_exist              if the path of the database file does not exist.
     * @throws path_is_not_regular_file         if the path of the magic database file is not a file.
     * @throws magic_load_database_file_error   if loading the magic database file fails.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    explicit magic(
        flags_mask_t                 flags_mask,
        const std::filesystem::path& database_file = default_database_file
    );

    /**
     * @brief Construct magic, open it using the flags and load the magic database file, noexcept version.
     *
     * @param[in] flags_mask        One of the flags enums or bitwise or of the flags enums.
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] database_file     The path of magic database file, default is default_database_file.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    magic(
        flags_mask_t                           flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path& database_file = default_database_file
    ) noexcept;

    /**
     * @brief Construct magic, open it using the flags and load the magic database file.
     *
     * @param[in] flags_container   Flags.
     * @param[in] database_file     The path of magic database file, default is default_database_file.
     *
     * @throws magic_open_error                 if opening magic fails.
     * @throws empty_path                       if the path of the database file is empty.
     * @throws path_does_not_exist              if the path of the database file does not exist.
     * @throws path_is_not_regular_file         if the path of the magic database file is not a file.
     * @throws magic_load_database_file_error   if loading the magic database file fails.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    explicit magic(
        const flags_container_t&     flags_container,
        const std::filesystem::path& database_file = default_database_file
    );

    /**
     * @brief Construct magic, open it using the flags and load the magic database file, noexcept version.
     *
     * @param[in] flags_container   Flags.
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] database_file     The path of magic database file, default is default_database_file.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    magic(
        const flags_container_t&               flags_container,
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path& database_file = default_database_file
    ) noexcept;

    /**
     * @brief Move construct magic.
     *
     * @note After move construction, the moved-from object (other) is closed.
     *       It cannot be used for identifying file types until 
     *       it is reopened and a magic database file is loaded.
     */
    magic(magic&& other) noexcept;

    /**
     * @brief Deleted copy constructor.
     */
    magic(const magic&) = delete;

    /**
     * @brief Move assign to this magic.
     *
     * @note After move construction, the moved-from object (other) is closed.
     *       It cannot be used for identifying file types until
     *       it is reopened and a magic database file is loaded.
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
     * @brief Used for testing whether magic is valid
     *        for identifying file types or not.
     *
     * @returns True if the magic is valid, i.e.,
     *          it is open and a magic database file is loaded;
     *          false otherwise.
     */
    [[nodiscard]] operator bool() const noexcept;

    /**
     * @brief Check the validity of entries in the colon separated database
     *        files passed in as database_file.
     *
     * @param[in] database_file     The file to check, default is default_database_file.
     *
     * @returns True if the database_file has valid entries, false otherwise.
     */
    [[nodiscard]] static bool check(
        const std::filesystem::path& database_file = default_database_file
    ) noexcept;

    /**
     * @brief Close magic.
     *
     * @note magic cannot be used for identifying file types until
     *       it is reopened and a magic database file is loaded.
     */
    void close() noexcept;

    /**
     * @brief Compile the colon separated list of database files
     *        passed in as database_file.
     *
     * @param[in] database_file     The file to compile, default is default_database_file.
     *
     * @returns True on success, false otherwise.

     * @note The compiled files created are named from the basename
     *       of each file argument with “.mgc” appended to it.
     */
    bool compile(
        const std::filesystem::path& database_file = default_database_file
    ) const noexcept;

    /**
     * @brief Get the flags of magic.
     *
     * @returns flags_container_t
     *
     * @throws magic_is_closed      if magic is closed.
     */
    [[nodiscard]] flags_container_t get_flags() const;

    /**
     * @brief Get the flags of magic, noexcept version.
     *
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns flags_container_t or std::nullopt if magic is closed.
     */
    [[nodiscard]] std::optional<flags_container_t> get_flags(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get the value of a parameter of magic.
     *
     * @param[in] parameter         One of the parameters enum.
     *
     * @returns Value of the desired parameter.
     *
     * @throws magic_is_closed      if magic is closed.
     */
    [[nodiscard]] std::size_t get_parameter(parameters parameter) const;

    /**
     * @brief Get the value of a parameter of magic, noexcept version.
     *
     * @param[in] parameter         One of the parameters enum.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns Value of the desired parameter or std::nullopt if magic is closed.
     */
    [[nodiscard]] std::optional<std::size_t> get_parameter(
        parameters                             parameter,
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get the values ​​of all parameters of magic.
     *
     * @returns <parameter, value> map.
     *
     * @throws magic_is_closed      if magic is closed.
     */
    [[nodiscard]] parameter_value_map_t get_parameters() const;

    /**
     * @brief Get the values ​​of all parameters of magic, noexcept version.
     *
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns <parameter, value> map or std::nullopt if magic is closed.
     */
    [[nodiscard]] std::optional<parameter_value_map_t> get_parameters(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get the version of the Magic Number Recognition Library.
     *
     * @returns The version number as a string.
     */
    [[nodiscard]] static std::string get_version() noexcept;

    /**
     * @brief Identify the type of a file.
     *
     * @param[in] path              The path of the file.
     *
     * @returns The type of the file as a string.
     *
     * @throws magic_is_closed              if magic is closed.
     * @throws magic_database_not_loaded    if the magic database is not loaded.
     * @throws empty_path                   if the path of the file is empty.
     * @throws path_does_not_exist          if the path of the file does not exist.
     * @throws magic_identify_file_error    if identifying the type of the file fails.
     */
    [[nodiscard]] file_type_t identify_file(const std::filesystem::path& path
    ) const;

    /**
     * @brief Identify the type of a file, noexcept version.
     *
     * @param[in] path              The path of the file.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns The type of the file or the error message.
     */
    [[nodiscard]] expected_file_type_t identify_file(
        const std::filesystem::path&           path,
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Identify the types of all files in a directory.
     *
     * @param[in] directory         The path of the directory.
     * @param[in] option            The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     *
     * @throws magic_is_closed              if magic is closed.
     * @throws magic_database_not_loaded    if the magic database is not loaded.
     * @throws empty_path                   if the path of the file is empty.
     * @throws path_does_not_exist          if the path of the file does not exist.
     * @throws magic_identify_file_error    if identifying the type of the file fails.
     */
    [[nodiscard]] types_of_files_t identify_files(
        const std::filesystem::path&       directory,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const;

    /**
     * @brief Identify the types of all files in a directory, noexcept version.
     *
     * @param[in] directory         The path of the directory.
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] option            The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     */
    [[nodiscard]] expected_types_of_files_t identify_files(
        const std::filesystem::path&           directory,
        [[maybe_unused]] const std::nothrow_t& tag,
        std::filesystem::directory_options     option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const noexcept;

    /**
     * @brief Identify the types of files.
     *
     * @param[in] files             The container that holds the paths of the files.
     *
     * @returns The types of each file as a map.
     *
     * @throws magic_is_closed              if magic is closed.
     * @throws magic_database_not_loaded    if the magic database is not loaded.
     * @throws empty_path                   if the path of the file is empty.
     * @throws path_does_not_exist          if the path of the file does not exist.
     * @throws magic_identify_file_error    if identifying the type of the file fails.
     */
    [[nodiscard]] types_of_files_t identify_files(
        const file_concepts::file_container auto& files
    ) const;

    /**
     * @brief Identify the types of files, noexcept version.
     *
     * @param[in] files             The container that holds the paths of the files.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns The types of each file as a map.
     */
    [[nodiscard]] expected_types_of_files_t identify_files(
        const file_concepts::file_container auto& files,
        [[maybe_unused]] const std::nothrow_t&    tag
    ) const noexcept;

    /**
     * @brief Used for testing whether magic is open or closed.
     *
     * @returns True if magic is open, false otherwise.
     */
    [[nodiscard]] bool is_open() const noexcept;

    /**
     * @brief Used for testing whether magic is valid
     *        for identifying file types or not.
     *
     * @returns True if the magic is valid, i.e.,
     *          it is open and a magic database file is loaded;
     *          false otherwise.
     */
    [[nodiscard]] bool is_valid() const noexcept;

    /**
     * @brief Load a magic database file.
     *
     * @param[in] database_file     The path of the magic database file, default is default_database_file.
     *
     * @throws magic_is_closed                  if magic is closed.
     * @throws empty_path                       if the path of the database file is empty.
     * @throws path_does_not_exist              if the path of the database file does not exist.
     * @throws path_is_not_regular_file         if the path of the database file is not a file.
     * @throws magic_load_database_file_error   if loading the database file fails.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    void load_database_file(
        const std::filesystem::path& database_file = default_database_file
    );

    /**
     * @brief Load a magic database file, noexcept version.
     *
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] database_file     The path of the magic database file, default is default_database_file.
     *
     * @returns True on success, false otherwise.
     *
     * @note load_database_file() adds “.mgc” to the database filename as appropriate.
     */
    bool load_database_file(
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path& database_file = default_database_file
    ) noexcept;

    /**
     * @brief Open magic using the flags.
     *
     * @param[in] flags_mask        One of the flags enums or bitwise or of the flags enums.
     *
     * @throws magic_open_error     if opening magic fails.
     *
     * @note If magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening magic.
     */
    void open(flags_mask_t flags_mask);

    /**
     * @brief Open magic using the flags, noexcept version.
     *
     * @param[in] flags_mask        One of the flags enums or bitwise or of the flags enums.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     *
     * @note If magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening magic.
     */
    bool open(
        flags_mask_t                           flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Open magic using the flags.
     *
     * @param[in] flags_container   Flags.
     *
     * @throws magic_open_error     if opening magic fails.
     *
     * @note If magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening magic.
     */
    void open(const flags_container_t& flags_container);

    /**
     * @brief Open magic using the flags, noexcept version.
     *
     * @param[in] flags_container   Flags.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     *
     * @note If magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening magic.
     */
    bool open(
        const flags_container_t&               flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set the flags of magic.
     *
     * @param[in] flags_mask          One of the flags enums or bitwise or of the flags enums.
     *
     * @throws magic_is_closed        if magic is closed.
     * @throws magic_set_flags_error  if setting the flags of magic fails.
     */
    void set_flags(flags_mask_t flags_mask);

    /**
     * @brief Set the flags of magicü noexcept version.
     *
     * @param[in] flags_mask          One of the flags enums or bitwise or of the flags enums.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    bool set_flags(
        flags_mask_t                           flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set the flags of magic.
     *
     * @param[in] flags_container     Flags.
     *
     * @throws magic_is_closed        if magic is closed.
     * @throws magic_set_flags_error  if setting the flags of magic fails.
     */
    void set_flags(const flags_container_t& flags_container);

    /**
     * @brief Set the flags of magic, noexcept version.
     *
     * @param[in] flags_container     Flags.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    bool set_flags(
        const flags_container_t&               flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set the value of a parameter of magic.
     *
     * @param[in] parameter                 One of the parameters enum.
     * @param[in] value                     The value of the parameter.
     *
     * @throws magic_is_closed              if magic is closed.
     * @throws magic_set_parameter_error    if setting the parameter of magic fails.
     */
    void set_parameter(parameters parameter, std::size_t value);

    /**
     * @brief Set the value of a parameter of magic, noexcept version.
     *
     * @param[in] parameter           One of the parameters enum.
     * @param[in] value               The value of the parameter.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    bool set_parameter(
        parameters                             parameter,
        std::size_t                            value,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set the values of the parameters of magic.
     *
     * @param[in] parameters                Parameters with corresponding values.
     *
     * @throws magic_is_closed              if magic is closed.
     * @throws magic_set_parameter_error    if setting the parameter of magic fails.
     */
    void set_parameters(const parameter_value_map_t& parameters);

    /**
     * @brief Set the values of the parameters of magic, noexcept version.
     *
     * @param[in] parameters          Parameters with corresponding values.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    bool set_parameters(
        const parameter_value_map_t&           parameters,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept;

private:
    class magic_private;
    std::unique_ptr<magic_private> m_impl;

    friend std::string to_string(flags);
    friend std::string to_string(parameters);
};

/**
 * @brief Convert the magic::types_of_files_t to string.
 *
 * @param[in] types_of_files            The types of each file.
 * @param[in] type_separator            The separator between the file and its type, default is " -> ".
 * @param[in] file_separator            The separator between the files, default is "\n".
 *
 * @returns The types_of_files as a string.
 */
[[nodiscard]] std::string to_string(
    const magic::types_of_files_t& types_of_files,
    const std::string&             type_separator = " -> ",
    const std::string&             file_separator = "\n"
);

/**
 * @brief Convert the magic::expected_file_type_t to string.
 *
 * @param[in] expected_file_type        The expected type of the file.
 *
 * @returns The expected_file_type as a string.
 */
[[nodiscard]] std::string to_string(
    const magic::expected_file_type_t& expected_file_type
);

/**
 * @brief Convert the magic::expected_types_of_files_t to string.
 *
 * @param[in] expected_types_of_files   The expected types of each file.
 * @param[in] type_separator            The separator between the file and its expected type, default is " -> ".
 * @param[in] file_separator            The separator between the files, default is "\n".
 *
 * @returns The expected_types_of_files as a string.
 */
[[nodiscard]] std::string to_string(
    const magic::expected_types_of_files_t& expected_types_of_files,
    const std::string&                      type_separator = " -> ",
    const std::string&                      file_separator = "\n"
);

/**
 * @brief Convert the magic::flags to string.
 * 
 * @param[in] flag                  The flag.
 *
 * @returns The flag as a string.
 */
[[nodiscard]] std::string to_string(magic::flags flag);

/**
 * @brief Convert the magic::flags_container_t to string.
 * 
 * @param[in] flags                 The flags.
 * @param[in] separator             The separator between the flags, default is ", ".
 *
 * @returns The flags as a string.
 */
[[nodiscard]] std::string to_string(
    const magic::flags_container_t& flags,
    const std::string&              separator = ", "
);

/**
 * @brief Convert the magic::parameters to string.
 * 
 * @param[in] parameter             The parameter.
 *
 * @returns The parameter as a string.
 */
[[nodiscard]] std::string to_string(magic::parameters parameter);

/**
 * @brief Convert the magic::parameter_value_map_t to string.
 * 
 * @param[in] parameters            The parameters with corresponding values.
 * @param[in] value_separator       The separator between the parameter and its value, default is ": ".
 * @param[in] parameter_separator   The separator between the parameters, default is ", ".
 *
 * @returns The parameters as a string.
 */
[[nodiscard]] std::string to_string(
    const magic::parameter_value_map_t& parameters,
    const std::string&                  value_separator     = ": ",
    const std::string&                  parameter_separator = ", "
);

} /* namespace recognition */

#endif /* MAGIC_HPP */
