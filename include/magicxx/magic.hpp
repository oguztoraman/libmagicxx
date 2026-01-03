/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
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

#include "magic_exception.hpp"
#include "progress_tracker.hpp"
#include "utility.hpp"

namespace Recognition {
/**
 * @class Magic
 *
 * @brief The Magic class provides a C++ wrapper over the Magic Number Recognition Library.
 *
 * @note  To use the Magic class for file type identification, ensure the following:
 *        1. The Magic instance must be opened.
 *        2. A magic database file must be successfully loaded.
 *        Only after these steps is the instance considered valid for identifying file types.
 */
class Magic {
public:
    /**
     * @typedef FlagsMaskT
     *
     * @brief Bitmask type representing a set of Magic::Flags used to configure the Magic instance.
     */
    using FlagsMaskT = std::bitset<30uz>;

    /**
     * @typedef FileTypeT
     *
     * @brief String type representing the detected type of a file.
     */
    using FileTypeT = std::string;

    /**
     * @typedef ErrorMessageT
     *
     * @brief String type representing an error message from file identification.
     */
    using ErrorMessageT = std::string;

    /**
     * @typedef ExpectedFileTypeT
     *
     * @brief Result type for file identification, containing either a file type or an error message.
     */
    using ExpectedFileTypeT = std::expected<FileTypeT, ErrorMessageT>;

    /**
     * @typedef TypesOfFilesT
     *
     * @brief Map from file paths to their detected types.
     */
    using TypesOfFilesT = std::map<std::filesystem::path, FileTypeT>;

    /**
     * @typedef TypeOfAFileT
     *
     * @brief Key-value pair representing a single file and its detected type.
     */
    using TypeOfAFileT = TypesOfFilesT::value_type;

    /**
     * @typedef ExpectedTypesOfFilesT
     *
     * @brief Map from file paths to expected file type results (success or error).
     */
    using ExpectedTypesOfFilesT = std::map<
        std::filesystem::path,
        ExpectedFileTypeT
    >;

    /**
     * @typedef ExpectedTypeOfAFileT
     *
     * @brief Key-value pair representing a single file and its expected file type result.
     */
    using ExpectedTypeOfAFileT = ExpectedTypesOfFilesT::value_type;

    /**
     * @typedef TrackerT
     *
     * @brief Alias for a shared pointer to a progress tracker used for monitoring file identification progress.
     *
     * @see Utility::ProgressTracker
     */
    using TrackerT = Utility::SharedProgressTrackerT;

    /**
     * @brief The Flags enums are used for configuring the flags of a Magic.
     *
     * @note The Flags enums are suitable for bitwise or operations.
     */
    enum Flags : unsigned long long {
        /* clang-format off */
        None             = 0ULL,       /**< No special handling. */
        Debug            = 1ULL << 0,  /**< Print debugging messages to stderr. */
        Symlink          = 1ULL << 1,  /**< If the file queried is a symlink, follow it. */
        Compress         = 1ULL << 2,  /**< If the file is compressed, unpack it and look at the  contents. */
        Devices          = 1ULL << 3,  /**< If the file is a block or character special device, then open the device and try to look in its contents. */
        MimeType         = 1ULL << 4,  /**< Return a MIME type string, instead of a  textual  description. */
        ContinueSearch   = 1ULL << 5,  /**< Return all matches, not just the first. */
        CheckDatabase    = 1ULL << 6,  /**< Check the magic database for consistency and print warnings to stderr. */
        PreserveAtime    = 1ULL << 7,  /**< On systems that support utime(3) or utimes(2), attempt to preserve the access time of files analysed. */
        Raw              = 1ULL << 8,  /**< Don't translate unprintable characters to a \ooo octal representation. */
        Error            = 1ULL << 9,  /**< Treat operating system errors while trying to open files and follow symlinks as real errors, instead of printing them in the magic buffer. */
        MimeEncoding     = 1ULL << 10, /**< Return a MIME encoding, instead of a textual description. */
        Mime             = 1ULL << 11, /**< A shorthand for MimeType|MimeEncoding. */
        Apple            = 1ULL << 12, /**< Return the Apple creator and type. */
        Extension        = 1ULL << 13, /**< Return a slash-separated list of extensions for this file type. */
        CompressTransp   = 1ULL << 14, /**< Don't report on compression, only report about the uncompressed data. */
        NoCompressFork   = 1ULL << 15, /**< Don't allow decompressors that use fork. */
        Nodesc           = 1ULL << 16, /**< A shorthand for Extension|Mime|Apple */
        NoCheckCompress  = 1ULL << 17, /**< Don't look inside compressed files. */
        NoCheckTar       = 1ULL << 18, /**< Don't examine tar files. */
        NoCheckSoft      = 1ULL << 19, /**< Don't consult magic files. */
        NoCheckApptype   = 1ULL << 20, /**< Don't check for EMX application type (only on EMX). */
        NoCheckElf       = 1ULL << 21, /**< Don't print ELF details. */
        NoCheckText      = 1ULL << 22, /**< Don't check for various types of text files. */
        NoCheckCdf       = 1ULL << 23, /**< Don't get extra information on MS Composite Document Files. */
        NoCheckCsv       = 1ULL << 24, /**< Don't examine CSV files. */
        NoCheckTokens    = 1ULL << 25, /**< Don't look for known tokens inside ascii files. */
        NoCheckEncoding  = 1ULL << 26, /**< Don't check text encodings. */
        NoCheckJson      = 1ULL << 27, /**< Don't examine JSON files. */
        NoCheckSimh      = 1ULL << 28, /**< Don't examine SIMH tape files. */
        NoCheckBuiltin   = 1ULL << 29  /**< No built-in tests; only consult the magic file. */
        /* clang-format on */
    };

    /**
     * @brief The Parameters enums are used for displaying or
     *        modifying the parameters of a Magic.
     */
    enum class Parameters : std::size_t {
        /* clang-format off */
        IndirMax     = 0uz, /**< Recursion limit for indirection (default is 15). */
        NameMax      = 1uz, /**< Use limit for name/use magic (default is 30). */
        ElfPhnumMax  = 2uz, /**< Max ELF program sections processed (default is 128). */
        ElfShnumMax  = 3uz, /**< Max ELF sections processed (default is 32768). */
        ElfNotesMax  = 4uz, /**< Max ELF notes processed (default is 256). */
        RegexMax     = 5uz, /**< Max length limit for REGEX searches (default is 8192). */
        BytesMax     = 6uz, /**< Max bytes to look inside file (default is 7340032). */
        EncodingMax  = 7uz, /**< Max bytes to scan for encoding (default is 1048576). */
        ElfShsizeMax = 8uz, /**< Max ELF section size (default is 134217728). */
        MagWarnMax   = 9uz  /**< Max warnings to tolerate in a magic file (default is 64). */
        /* clang-format on */
    };

    /**
     * @typedef FlagsContainerT
     *
     * @brief Container type holding a collection of Magic::Flags.
     */
    using FlagsContainerT = std::vector<Flags>;

    /**
     * @typedef ParameterValueMapT
     *
     * @brief Map from Magic::Parameters to their corresponding values.
     */
    using ParameterValueMapT = std::map<Parameters, std::size_t>;

    /**
     * @typedef ParameterValueT
     *
     * @brief Key-value pair representing a single parameter and its value.
     */
    using ParameterValueT = ParameterValueMapT::value_type;

    /**
     * @brief The path of the default database file.
     */
    static std::string_view DEFAULT_DATABASE_FILE;

    /**
     * @brief Construct Magic without opening it.
     * 
     * @note This Magic is not valid for identifying file types
     *       until it is opened and a magic database file is loaded.
     */
    Magic() noexcept;

    /**
     * @brief Construct Magic, open it using the flags and load the magic database file.
     *
     * @param[in] flags_mask        One of the Flags enums or bitwise or of the Flags enums.
     * @param[in] database_file     The path of magic database file, default is DEFAULT_DATABASE_FILE.
     *
     * @throws MagicOpenError                 if opening Magic fails.
     * @throws EmptyPath                      if the path of the database file is empty.
     * @throws PathDoesNotExist               if the path of the database file does not exist.
     * @throws PathIsNotRegularFile           if the path of the magic database file is not a file.
     * @throws MagicLoadDatabaseFileError     if loading the magic database file fails.
     *
     * @note LoadDatabaseFile() adds ".mgc" to the database filename as appropriate.
     */
    explicit Magic(
        FlagsMaskT                   flags_mask,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    );

    /**
     * @brief Construct Magic, open it using the flags and load the magic database file, noexcept version.
     *
     * @param[in] flags_mask        One of the Flags enums or bitwise or of the Flags enums.
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] database_file     The path of magic database file, default is DEFAULT_DATABASE_FILE.
     *
     * @note LoadDatabaseFile() adds ".mgc" to the database filename as appropriate.
     */
    Magic(
        FlagsMaskT                   flags_mask,
        const std::nothrow_t&        tag,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /**
     * @brief Construct Magic, open it using the flags and load the magic database file.
     *
     * @param[in] flags_container   Flags.
     * @param[in] database_file     The path of magic database file, default is DEFAULT_DATABASE_FILE.
     *
     * @throws MagicOpenError                 if opening Magic fails.
     * @throws EmptyPath                      if the path of the database file is empty.
     * @throws PathDoesNotExist               if the path of the database file does not exist.
     * @throws PathIsNotRegularFile           if the path of the magic database file is not a file.
     * @throws MagicLoadDatabaseFileError     if loading the magic database file fails.
     *
     * @note LoadDatabaseFile() adds ".mgc" to the database filename as appropriate.
     */
    explicit Magic(
        const FlagsContainerT&       flags_container,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    );

    /**
     * @brief Construct Magic, open it using the flags and load the magic database file, noexcept version.
     *
     * @param[in] flags_container   Flags.
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] database_file     The path of magic database file, default is DEFAULT_DATABASE_FILE.
     *
     * @note LoadDatabaseFile() adds ".mgc" to the database filename as appropriate.
     */
    Magic(
        const FlagsContainerT&       flags_container,
        const std::nothrow_t&        tag,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /**
     * @brief Move construct Magic.
     *
     * @note After move construction, the moved-from object (other) is closed.
     *       It cannot be used for identifying file types until 
     *       it is reopened and a magic database file is loaded.
     */
    Magic(Magic&& other) noexcept;

    /**
     * @brief Deleted copy constructor.
     */
    Magic(const Magic&) = delete;

    /**
     * @brief Move assign to this Magic.
     *
     * @note After move construction, the moved-from object (other) is closed.
     *       It cannot be used for identifying file types until
     *       it is reopened and a magic database file is loaded.
     */
    Magic& operator=(Magic&& other) noexcept;

    /**
     * @brief Deleted copy assignment.
     */
    Magic& operator=(const Magic&) = delete;

    /**
     * @brief Destruct Magic.
     */
    ~Magic();

    /**
     * @brief Used for testing whether Magic is valid
     *        for identifying file types or not.
     *
     * @returns True if the Magic is valid, i.e.,
     *          it is open and a magic database file is loaded;
     *          false otherwise.
     */
    [[nodiscard]] operator bool() const noexcept;

    /**
     * @brief Check the validity of entries in the colon separated database
     *        files passed in as database_file.
     *
     * @param[in] database_file     The file to check, default is DEFAULT_DATABASE_FILE.
     *
     * @returns True if the database_file has valid entries, false otherwise.
     */
    [[nodiscard]] static bool Check(
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /**
     * @brief Close Magic.
     *
     * @note Magic cannot be used for identifying file types until
     *       it is reopened and a magic database file is loaded.
     */
    void Close() noexcept;

    /**
     * @brief Compile the colon separated list of database files
     *        passed in as database_file.
     *
     * @param[in] database_file     The file to compile, default is DEFAULT_DATABASE_FILE.
     *
     * @returns True on success, false otherwise.

     * @note The compiled files created are named from the basename
     *       of each file argument with ".mgc" appended to it.
     */
    [[nodiscard]] static bool Compile(
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /**
     * @brief Get the flags of Magic.
     *
     * @returns FlagsContainerT
     *
     * @throws MagicIsClosed      if Magic is closed.
     */
    [[nodiscard]] FlagsContainerT GetFlags() const;

    /**
     * @brief Get the flags of Magic, noexcept version.
     *
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns FlagsContainerT or std::nullopt if Magic is closed.
     */
    [[nodiscard]] std::optional<FlagsContainerT> GetFlags(
        const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get the value of a parameter of Magic.
     *
     * @param[in] parameter         One of the Parameters enum.
     *
     * @returns Value of the desired parameter.
     *
     * @throws MagicIsClosed      if Magic is closed.
     */
    [[nodiscard]] std::size_t GetParameter(Parameters parameter) const;

    /**
     * @brief Get the value of a parameter of Magic, noexcept version.
     *
     * @param[in] parameter         One of the Parameters enum.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns Value of the desired parameter or std::nullopt if Magic is closed.
     */
    [[nodiscard]] std::optional<std::size_t> GetParameter(
        Parameters            parameter,
        const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get the values ​​of all parameters of Magic.
     *
     * @returns <parameter, value> map.
     *
     * @throws MagicIsClosed      if Magic is closed.
     */
    [[nodiscard]] ParameterValueMapT GetParameters() const;

    /**
     * @brief Get the values ​​of all parameters of Magic, noexcept version.
     *
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns <parameter, value> map or std::nullopt if Magic is closed.
     */
    [[nodiscard]] std::optional<ParameterValueMapT> GetParameters(
        const std::nothrow_t& tag
    ) const noexcept;

    /**
     * @brief Get the version of the Magic Number Recognition Library.
     *
     * @returns The version number as a string.
     */
    [[nodiscard]] static std::string GetVersion() noexcept;

    /**
     * @brief Identify the type of a file.
     *
     * @param[in] path              The path of the file.
     *
     * @returns The type of the file as a string.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicDatabaseNotLoaded     if the magic database is not loaded.
     * @throws EmptyPath                  if the path of the file is empty.
     * @throws PathDoesNotExist           if the path of the file does not exist.
     * @throws MagicIdentifyFileError     if identifying the type of the file fails.
     */
    [[nodiscard]] FileTypeT IdentifyFile(
        const std::filesystem::path& path
    ) const;

    /**
     * @brief Identify the type of a file, noexcept version.
     *
     * @param[in] path              The path of the file.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns The type of the file or the error message.
     */
    [[nodiscard]] ExpectedFileTypeT IdentifyFile(
        const std::filesystem::path& path,
        const std::nothrow_t&        tag
    ) const noexcept;

    /**
     * @brief Identify the types of all files in a directory.
     *
     * @param[in] directory         The path of the directory.
     * @param[in] option            The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicDatabaseNotLoaded     if the magic database is not loaded.
     * @throws EmptyPath                  if the path of the directory is empty.
     * @throws PathDoesNotExist           if the path of the directory does not exist.
     * @throws PathIsNotDirectory         if the path of the directory is not a directory.
     * @throws FilesystemError            if the underlying std::filesystem OS API fails.
     * @throws MagicIdentifyFileError     if identifying the type of the file fails.
     */
    [[nodiscard]] TypesOfFilesT IdentifyFiles(
        const std::filesystem::path&       directory,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const
    {
        return IdentifyDirectoryImpl(directory, option);
    }

    /**
     * @brief Identify the types of all files in a directory with progress tracking.
     *
     * @param[in]  directory        The path of the directory.
     * @param[out] tracker          The progress tracker to track the progress of the identification. Must not be null.
     * @param[in]  option           The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicDatabaseNotLoaded     if the magic database is not loaded.
     * @throws EmptyPath                  if the path of the directory is empty.
     * @throws PathDoesNotExist           if the path of the directory does not exist.
     * @throws PathIsNotDirectory         if the path of the directory is not a directory.
     * @throws NullTracker                if the tracker is null.
     * @throws FilesystemError            if the underlying std::filesystem OS API fails.
     * @throws MagicIdentifyFileError     if identifying the type of the file fails.
     */
    [[nodiscard]] TypesOfFilesT IdentifyFiles(
        const std::filesystem::path&       directory,
        TrackerT                           tracker,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const
    {
        return IdentifyDirectoryImpl(directory, option, tracker);
    }

    /**
     * @brief Identify the types of all files in a directory, noexcept version.
     *
     * @param[in] directory         The path of the directory.
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] option            The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map or an empty map on failure.
     */
    [[nodiscard]] ExpectedTypesOfFilesT IdentifyFiles(
        const std::filesystem::path&       directory,
        const std::nothrow_t&              tag,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const noexcept
    {
        return IdentifyDirectoryImpl(directory, tag, option);
    }

    /**
     * @brief Identify the types of all files in a directory with progress tracking, noexcept version.
     *
     * @param[in]  directory        The path of the directory.
     * @param[in]  tag              Tag for non-throwing overload.
     * @param[out] tracker          The progress tracker to track the progress of the identification. Must not be null.
     * @param[in]  option           The directory iteration option, default is follow_directory_symlink.
     *
     * @returns The types of each file as a map or an empty map on failure.
     */
    [[nodiscard]] ExpectedTypesOfFilesT IdentifyFiles(
        const std::filesystem::path&       directory,
        const std::nothrow_t&              tag,
        TrackerT                           tracker,
        std::filesystem::directory_options option = std::filesystem::
            directory_options::follow_directory_symlink
    ) const noexcept
    {
        return IdentifyDirectoryImpl(directory, tag, option, tracker);
    }

    /**
     * @brief Identify the types of files.
     *
     * @param[in] files             The container that holds the paths of the files.
     *
     * @returns The types of each file as a map.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicDatabaseNotLoaded     if the magic database is not loaded.
     * @throws EmptyPath                  if the path of the file is empty.
     * @throws PathDoesNotExist           if the path of the file does not exist.
     * @throws MagicIdentifyFileError     if identifying the type of the file fails.
     */
    [[nodiscard]] TypesOfFilesT IdentifyFiles(
        const Utility::FileContainer auto& files
    ) const
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)}
        );
    }

    /**
     * @brief Identify the types of files with progress tracking.
     *
     * @param[in]  files            The container that holds the paths of the files.
     * @param[out] tracker          The progress tracker to track the progress of the identification. 
     *
     * @returns The types of each file as a map.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicDatabaseNotLoaded     if the magic database is not loaded.
     * @throws EmptyPath                  if the path of the file is empty.
     * @throws PathDoesNotExist           if the path of the file does not exist.
     * @throws NullTracker                if the tracker is null.
     * @throws MagicIdentifyFileError     if identifying the type of the file fails.
     */
    [[nodiscard]] TypesOfFilesT IdentifyFiles(
        const Utility::FileContainer auto& files,
        TrackerT                           tracker
    ) const
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)},
            tracker
        );
    }

    /**
     * @brief Identify the types of files, noexcept version.
     *
     * @param[in] files             The container that holds the paths of the files.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns The types of each file as a map or an empty map on failure.
     */
    [[nodiscard]] ExpectedTypesOfFilesT IdentifyFiles(
        const Utility::FileContainer auto& files,
        const std::nothrow_t&              tag
    ) const noexcept
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)},
            tag
        );
    }

    /**
     * @brief Identify the types of files with progress tracking, noexcept version.
     *
     * @param[in]  files            The container that holds the paths of the files.
     * @param[in]  tag              Tag for non-throwing overload.
     * @param[out] tracker          The progress tracker to track the progress of the identification. 
     *
     * @returns The types of each file as a map or an empty map on failure.
     */
    [[nodiscard]] ExpectedTypesOfFilesT IdentifyFiles(
        const Utility::FileContainer auto& files,
        const std::nothrow_t&              tag,
        TrackerT                           tracker
    ) const noexcept
    {
        return IdentifyContainerImpl(
            {std::ranges::begin(files), std::ranges::end(files)},
            tag,
            tracker
        );
    }

    /**
     * @brief Used for testing whether a magic database is loaded or not.
     *
     * @returns True if a magic database is loaded, false otherwise.
     */
    [[nodiscard]] bool IsDatabaseLoaded() const noexcept;

    /**
     * @brief Used for testing whether Magic is open or closed.
     *
     * @returns True if Magic is open, false otherwise.
     */
    [[nodiscard]] bool IsOpen() const noexcept;

    /**
     * @brief Used for testing whether Magic is valid
     *        for identifying file types or not.
     *
     * @returns True if the Magic is valid, i.e.,
     *          it is open and a magic database file is loaded;
     *          false otherwise.
     */
    [[nodiscard]] bool IsValid() const noexcept;

    /**
     * @brief Load a magic database file.
     *
     * @param[in] database_file     The path of the magic database file, default is DEFAULT_DATABASE_FILE.
     *
     * @throws MagicIsClosed                  if Magic is closed.
     * @throws EmptyPath                      if the path of the database file is empty.
     * @throws PathDoesNotExist               if the path of the database file does not exist.
     * @throws PathIsNotRegularFile           if the path of the database file is not a file.
     * @throws MagicLoadDatabaseFileError     if loading the database file fails.
     *
     * @note LoadDatabaseFile() adds ".mgc" to the database filename as appropriate.
     */
    void LoadDatabaseFile(
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    );

    /**
     * @brief Load a magic database file, noexcept version.
     *
     * @param[in] tag               Tag for non-throwing overload.
     * @param[in] database_file     The path of the magic database file, default is DEFAULT_DATABASE_FILE.
     *
     * @returns True on success, false otherwise.
     *
     * @note LoadDatabaseFile() adds ".mgc" to the database filename as appropriate.
     */
    [[nodiscard]] bool LoadDatabaseFile(
        const std::nothrow_t&        tag,
        const std::filesystem::path& database_file = DEFAULT_DATABASE_FILE
    ) noexcept;

    /**
     * @brief Open Magic using the flags.
     *
     * @param[in] flags_mask        One of the Flags enums or bitwise or of the Flags enums.
     *
     * @throws MagicOpenError     if opening Magic fails.
     *
     * @note If Magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening Magic.
     */
    void Open(FlagsMaskT flags_mask);

    /**
     * @brief Open Magic using the flags, noexcept version.
     *
     * @param[in] flags_mask        One of the Flags enums or bitwise or of the Flags enums.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     *
     * @note If Magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening Magic.
     */
    [[nodiscard]] bool Open(
        FlagsMaskT            flags_mask,
        const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Open Magic using the flags.
     *
     * @param[in] flags_container   Flags.
     *
     * @throws MagicOpenError     if opening Magic fails.
     *
     * @note If Magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening Magic.
     */
    void Open(const FlagsContainerT& flags_container);

    /**
     * @brief Open Magic using the flags, noexcept version.
     *
     * @param[in] flags_container   Flags.
     * @param[in] tag               Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     *
     * @note If Magic is open, it will be reopened using the flags after closing it.
     * @note A magic database file must be loaded after opening Magic.
     */
    [[nodiscard]] bool Open(
        const FlagsContainerT& flags_container,
        const std::nothrow_t&  tag
    ) noexcept;

    /**
     * @brief Set the flags of Magic.
     *
     * @param[in] flags_mask          One of the Flags enums or bitwise or of the Flags enums.
     *
     * @throws MagicIsClosed        if Magic is closed.
     * @throws MagicSetFlagsError   if setting the flags of Magic fails.
     */
    void SetFlags(FlagsMaskT flags_mask);

    /**
     * @brief Set the flags of Magic, noexcept version.
     *
     * @param[in] flags_mask          One of the Flags enums or bitwise or of the Flags enums.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    [[nodiscard]] bool SetFlags(
        FlagsMaskT            flags_mask,
        const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set the flags of Magic.
     *
     * @param[in] flags_container     Flags.
     *
     * @throws MagicIsClosed        if Magic is closed.
     * @throws MagicSetFlagsError   if setting the flags of Magic fails.
     */
    void SetFlags(const FlagsContainerT& flags_container);

    /**
     * @brief Set the flags of Magic, noexcept version.
     *
     * @param[in] flags_container     Flags.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    [[nodiscard]] bool SetFlags(
        const FlagsContainerT& flags_container,
        const std::nothrow_t&  tag
    ) noexcept;

    /**
     * @brief Set the value of a parameter of Magic.
     *
     * @param[in] parameter                 One of the Parameters enum.
     * @param[in] value                     The value of the parameter.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicSetParameterError     if setting the parameter of Magic fails.
     */
    void SetParameter(Parameters parameter, std::size_t value);

    /**
     * @brief Set the value of a parameter of Magic, noexcept version.
     *
     * @param[in] parameter           One of the Parameters enum.
     * @param[in] value               The value of the parameter.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    [[nodiscard]] bool SetParameter(
        Parameters            parameter,
        std::size_t           value,
        const std::nothrow_t& tag
    ) noexcept;

    /**
     * @brief Set the values of the parameters of Magic.
     *
     * @param[in] parameters                Parameters with corresponding values.
     *
     * @throws MagicIsClosed              if Magic is closed.
     * @throws MagicSetParameterError     if setting the parameter of Magic fails.
     */
    void SetParameters(const ParameterValueMapT& parameters);

    /**
     * @brief Set the values of the parameters of Magic, noexcept version.
     *
     * @param[in] parameters          Parameters with corresponding values.
     * @param[in] tag                 Tag for non-throwing overload.
     *
     * @returns True on success, false otherwise.
     */
    [[nodiscard]] bool SetParameters(
        const ParameterValueMapT& parameters,
        const std::nothrow_t&     tag
    ) noexcept;

private:
    class MagicPrivate;
    std::unique_ptr<MagicPrivate> m_impl;

    using DefaultFileContainerT = std::vector<std::filesystem::path>;

    [[nodiscard]] TypesOfFilesT IdentifyDirectoryImpl(
        const std::filesystem::path&       directory,
        std::filesystem::directory_options option,
        TrackerT tracker = Utility::MakeSharedProgressTracker()
    ) const;

    [[nodiscard]] ExpectedTypesOfFilesT IdentifyDirectoryImpl(
        const std::filesystem::path&       directory,
        const std::nothrow_t&              tag,
        std::filesystem::directory_options option,
        TrackerT tracker = Utility::MakeSharedProgressTracker()
    ) const noexcept;

    [[nodiscard]] TypesOfFilesT IdentifyContainerImpl(
        const DefaultFileContainerT& files,
        TrackerT tracker = Utility::MakeSharedProgressTracker()
    ) const;

    [[nodiscard]] ExpectedTypesOfFilesT IdentifyContainerImpl(
        const DefaultFileContainerT& files,
        const std::nothrow_t&        tag,
        TrackerT tracker = Utility::MakeSharedProgressTracker()
    ) const noexcept;

    friend std::string ToString(Flags);
    friend std::string ToString(Parameters);
};

/**
 * @brief Convert the Magic::TypeOfAFileT to string.
 *
 * @param[in] type_of_a_file            The type of a file.
 * @param[in] type_separator            The separator between the file and its type, default is " -> ".
 *
 * @returns The type_of_a_file as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::TypeOfAFileT& type_of_a_file,
    const std::string&         type_separator = " -> "
);

/**
 * @brief Convert the Magic::TypesOfFilesT to string.
 *
 * @param[in] types_of_files            The types of each file.
 * @param[in] type_separator            The separator between the file and its type, default is " -> ".
 * @param[in] file_separator            The separator between the files, default is "\n".
 *
 * @returns The types_of_files as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::TypesOfFilesT& types_of_files,
    const std::string&          type_separator = " -> ",
    const std::string&          file_separator = "\n"
);

/**
 * @brief Convert the Magic::ExpectedFileTypeT to string.
 *
 * @param[in] expected_file_type        The expected type of the file.
 *
 * @returns The expected_file_type as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::ExpectedFileTypeT& expected_file_type
);

/**
 * @brief Convert the Magic::ExpectedTypeOfAFileT to string.
 *
 * @param[in] expected_type_of_a_file   The expected type of a file.
 * @param[in] type_separator            The separator between the file and its expected type, default is " -> ".
 *
 * @returns The expected_type_of_a_file as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::ExpectedTypeOfAFileT& expected_type_of_a_file,
    const std::string&                 type_separator = " -> "
);

/**
 * @brief Convert the Magic::ExpectedTypesOfFilesT to string.
 *
 * @param[in] expected_types_of_files   The expected types of each file.
 * @param[in] type_separator            The separator between the file and its expected type, default is " -> ".
 * @param[in] file_separator            The separator between the files, default is "\n".
 *
 * @returns The expected_types_of_files as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::ExpectedTypesOfFilesT& expected_types_of_files,
    const std::string&                  type_separator = " -> ",
    const std::string&                  file_separator = "\n"
);

/**
 * @brief Convert the Magic::Flags to string.
 * 
 * @param[in] flag                  The flag.
 *
 * @returns The flag as a string.
 */
[[nodiscard]] std::string ToString(Magic::Flags flag);

/**
 * @brief Convert the Magic::FlagsContainerT to string.
 * 
 * @param[in] flags                 The flags.
 * @param[in] separator             The separator between the flags, default is ", ".
 *
 * @returns The flags as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::FlagsContainerT& flags,
    const std::string&            separator = ", "
);

/**
 * @brief Convert the Magic::Parameters to string.
 * 
 * @param[in] parameter             The parameter.
 *
 * @returns The parameter as a string.
 */
[[nodiscard]] std::string ToString(Magic::Parameters parameter);

/**
 * @brief Convert the Magic::ParameterValueT to string.
 * 
 * @param[in] parameter_value       The parameter with corresponding value.
 * @param[in] value_separator       The separator between the parameter and its value, default is ": ".
 *
 * @returns The parameter_value as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::ParameterValueT& parameter_value,
    const std::string&            value_separator = ": "
);

/**
 * @brief Convert the Magic::ParameterValueMapT to string.
 * 
 * @param[in] parameters            The parameters with corresponding values.
 * @param[in] value_separator       The separator between the parameter and its value, default is ": ".
 * @param[in] parameter_separator   The separator between the parameters, default is ", ".
 *
 * @returns The parameters as a string.
 */
[[nodiscard]] std::string ToString(
    const Magic::ParameterValueMapT& parameters,
    const std::string&               value_separator     = ": ",
    const std::string&               parameter_separator = ", "
);
} /* namespace Recognition */

#endif /* MAGIC_HPP */
