/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <cmath>

#include <array>
#include <utility>

#include "magic.hpp"

namespace Recognition {
namespace Detail {
#include <magic.h>
} /* namespace Detail */

class Magic::MagicPrivate {
public:
    using IdentifyFileOptionsMaskT = std::bitset<3uz>;

    enum IdentifyFileOptions : unsigned long long {
        CheckNothing    = 0ULL,
        CheckIsValid    = 1ULL << 0,
        CheckPathEmpty  = 1ULL << 1,
        CheckPathExists = 1ULL << 2,
        CheckPath       = CheckPathEmpty | CheckPathExists,
        CheckEverything = CheckIsValid | CheckPath
    };

    MagicPrivate() noexcept = default;

    MagicPrivate(
        FlagsMaskT                   flags_mask,
        const std::filesystem::path& database_file
    )
    {
        Open(flags_mask);
        LoadDatabaseFile(database_file);
    }

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

    MagicPrivate(
        const FlagsContainerT&       flags_container,
        const std::filesystem::path& database_file
    )
    {
        Open(flags_container);
        LoadDatabaseFile(database_file);
    }

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

    MagicPrivate(MagicPrivate&&) noexcept = default;

    MagicPrivate(const MagicPrivate&) = delete;

    MagicPrivate& operator=(MagicPrivate&&) noexcept = default;

    MagicPrivate& operator=(const MagicPrivate&) = delete;

    ~MagicPrivate() = default;

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

    [[nodiscard]] FlagsContainerT GetFlags() const
    {
        MagicPrivate::ThrowExceptionOnFailure<MagicIsClosed>(IsOpen());
        return FlagsConverter(m_flags_mask);
    }

    [[nodiscard]] std::optional<FlagsContainerT> GetFlags(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!IsOpen()) {
            return std::nullopt;
        }
        return {FlagsConverter(m_flags_mask)};
    }

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

    [[nodiscard]] ParameterValueMapT GetParameters() const
    {
        ParameterValueMapT parameter_value_map;
        for (std::size_t i{}; i < LIBMAGIC_PARAMETER_COUNT; ++i) {
            auto parameter                 = static_cast<Parameters>(i);
            parameter_value_map[parameter] = GetParameter(parameter);
        }
        return parameter_value_map;
    }

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

    bool IdentifyContainerPreliminaryChecks(
        [[maybe_unused]] const std::nothrow_t& tag,
        ProgressTrackerT                       progress_tracker
    ) const noexcept
    {
        return IsValid() && progress_tracker;
    }

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

    [[nodiscard]] bool IsDatabaseLoaded() const noexcept
    {
        return m_is_database_loaded;
    }

    [[nodiscard]] bool IsOpen() const noexcept
    {
        return m_cookie != nullptr;
    }

    [[nodiscard]] bool IsValid() const noexcept
    {
        return IsOpen() && IsDatabaseLoaded();
    }

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

    void Open(const FlagsContainerT& flags_container)
    {
        Open(FlagsMaskT{FlagsConverter(flags_container)});
    }

    [[nodiscard]] bool Open(
        const FlagsContainerT&                 flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return Open(FlagsMaskT{FlagsConverter(flags_container)}, std::nothrow);
    }

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

    void SetFlags(const FlagsContainerT& flags_container)
    {
        SetFlags(FlagsMaskT{FlagsConverter(flags_container)});
    }

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
    using CookieT = std::unique_ptr<
        Detail::magic_set,
        decltype([](Detail::magic_t cookie) noexcept {
            Detail::magic_close(cookie);
        })
    >;

    CookieT    m_cookie{nullptr};
    FlagsMaskT m_flags_mask{0};
    bool       m_is_database_loaded{false};

    static constexpr IdentifyFileOptionsMaskT CHECK_IS_VALID_OPTION{
        IdentifyFileOptions::CheckIsValid
    };
    static constexpr IdentifyFileOptionsMaskT CHECK_PATH_EMPTY_OPTION{
        IdentifyFileOptions::CheckPathEmpty
    };
    static constexpr IdentifyFileOptionsMaskT CHECK_PATH_EXISTS_OPTION{
        IdentifyFileOptions::CheckPathExists
    };

    static constexpr auto LIBMAGIC_ERROR           = -1;
    static constexpr auto LIBMAGIC_FLAGS_COUNT     = FlagsMaskT{}.size();
    static constexpr auto LIBMAGIC_PARAMETER_COUNT = 10uz;

    using LibmagicValueT      = int;
    using LibmagicValueNameT  = std::string;
    using LibmagicPairT       = std::pair<LibmagicValueT, const char*>;
    using LibmagicFlagsT      = std::array<LibmagicPairT, LIBMAGIC_FLAGS_COUNT>;
    using LibmagicParametersT = std::array<
        LibmagicPairT,
        LIBMAGIC_PARAMETER_COUNT
    >;

    static constexpr LibmagicPairT LIBMAGIC_FLAG_NONE{
        std::make_pair(MAGIC_NONE, "None")
    };

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

    [[nodiscard]] std::string GetErrorMessage() const noexcept
    {
        const char* magic_error_cstr = Detail::magic_error(m_cookie.get());
        return magic_error_cstr ? magic_error_cstr : "";
    }

    struct FlagsConverter {
        explicit FlagsConverter(const FlagsContainerT& flags_container) noexcept
          : m_flags_mask{std::ranges::fold_left(
                flags_container,
                flags_container.empty() ? Flags::None : flags_container.front(),
                std::bit_or<decltype(1ULL)>{}
            )}
        { }

        explicit FlagsConverter(FlagsMaskT flags_mask) noexcept
          : m_flags_mask{flags_mask}
        { }

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

        operator FlagsMaskT() const
        {
            return m_flags_mask;
        }

        const FlagsMaskT m_flags_mask;
    };

    struct LibmagicPairConverter {
        constexpr explicit LibmagicPairConverter(
            const LibmagicPairT& pair
        ) noexcept
          : m_pair{pair}
        { }

        constexpr operator LibmagicValueNameT() const noexcept
        {
            return std::get<const char*>(m_pair);
        }

        constexpr operator LibmagicValueT() const noexcept
        {
            return std::get<LibmagicValueT>(m_pair);
        }

        const LibmagicPairT& m_pair;
    };

    friend std::string ToString(Flags);
    friend std::string ToString(Parameters);
};

std::string ToString(
    const Magic::FileTypeEntryT& file_type_entry,
    const std::string&           type_separator
)
{
    const auto& file      = file_type_entry.first;
    const auto& file_type = file_type_entry.second;
    return file.string() + type_separator + file_type;
}

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

std::string ToString(const Magic::ExpectedFileTypeT& expected_file_type)
{
    return expected_file_type.value_or(expected_file_type.error());
}

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

std::string ToString(Magic::Flags flag)
{
    if (flag == Magic::Flags::None) {
        return Magic::MagicPrivate::LIBMAGIC_FLAG_NONE.second;
    }
    const auto& flags = Magic::MagicPrivate::LIBMAGIC_FLAGS;
    const auto& flag_name{flags[std::log2(std::to_underlying(flag))].second};
    return flag_name;
}

std::string ToString(
    const Magic::FlagsContainerT& flags,
    const std::string&            separator
)
{
    return Utility::ToString(flags, separator, [](Magic::Flags flag) {
        return ToString(flag);
    });
}

std::string ToString(Magic::Parameters parameter)
{
    const auto& parameters = Magic::MagicPrivate::LIBMAGIC_PARAMETERS;
    const auto& parameter_name{
        parameters[std::to_underlying(parameter)].second
    };
    return parameter_name;
}

std::string ToString(
    const Magic::ParameterValueT& parameter_value,
    const std::string&            value_separator
)
{
    const auto& parameter = parameter_value.first;
    const auto& value     = parameter_value.second;
    return std::format("{}{}{}", ToString(parameter), value_separator, value);
}

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

std::string_view Magic::DEFAULT_DATABASE_FILE{MAGIC_DEFAULT_DATABASE_FILE};

Magic::Magic() noexcept
  : m_impl{std::make_unique<MagicPrivate>()}
{ }

Magic::Magic(FlagsMaskT flags_mask, const std::filesystem::path& database_file)
  : m_impl{std::make_unique<MagicPrivate>(flags_mask, database_file)}
{ }

Magic::Magic(
    FlagsMaskT                   flags_mask,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<MagicPrivate>(flags_mask, tag, database_file)}
{ }

Magic::Magic(
    const FlagsContainerT&       flags_container,
    const std::filesystem::path& database_file
)
  : m_impl{std::make_unique<MagicPrivate>(flags_container, database_file)}
{ }

Magic::Magic(
    const FlagsContainerT&       flags_container,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<MagicPrivate>(flags_container, tag, database_file)}
{ }

Magic::Magic(Magic&& other) noexcept
  : m_impl{std::exchange(other.m_impl, std::make_unique<MagicPrivate>())}
{ }

Magic& Magic::operator=(Magic&& other) noexcept
{
    m_impl = std::exchange(other.m_impl, std::make_unique<MagicPrivate>());
    return *this;
}

Magic::~Magic() = default;

Magic::operator bool() const noexcept
{
    return IsValid();
}

bool Magic::Check(const std::filesystem::path& database_file) noexcept
{
    MagicPrivate magic_for_check{};
    return magic_for_check.Open(Magic::Flags::None, std::nothrow)
        && magic_for_check.Check(database_file);
}

void Magic::Close() noexcept
{
    m_impl = std::make_unique<MagicPrivate>();
}

bool Magic::Compile(const std::filesystem::path& database_file) noexcept
{
    MagicPrivate magic_for_compile{};
    return magic_for_compile.Open(Magic::Flags::None, std::nothrow)
        && magic_for_compile.Compile(database_file);
}

Magic::FlagsContainerT Magic::GetFlags() const
{
    return m_impl->GetFlags();
}

std::optional<Magic::FlagsContainerT> Magic::GetFlags(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetFlags(tag);
}

std::size_t Magic::GetParameter(Magic::Parameters parameter) const
{
    return m_impl->GetParameter(parameter);
}

std::optional<std::size_t> Magic::GetParameter(
    Parameters            parameter,
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetParameter(parameter, tag);
}

Magic::ParameterValueMapT Magic::GetParameters() const
{
    return m_impl->GetParameters();
}

std::optional<Magic::ParameterValueMapT> Magic::GetParameters(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->GetParameters(tag);
}

std::string Magic::GetVersion() noexcept
{
    return std::format("{:2}", Detail::magic_version() / 100.);
}

Magic::FileTypeT Magic::IdentifyFile(const std::filesystem::path& path) const
{
    return m_impl->IdentifyFile(
        path,
        MagicPrivate::IdentifyFileOptions::CheckEverything
    );
}

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

bool Magic::IsDatabaseLoaded() const noexcept
{
    return m_impl->IsDatabaseLoaded();
}

bool Magic::IsOpen() const noexcept
{
    return m_impl->IsOpen();
}

bool Magic::IsValid() const noexcept
{
    return m_impl->IsValid();
}

void Magic::LoadDatabaseFile(const std::filesystem::path& database_file)
{
    m_impl->LoadDatabaseFile(database_file);
}

bool Magic::LoadDatabaseFile(
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
{
    return m_impl->LoadDatabaseFile(tag, database_file);
}

void Magic::Open(FlagsMaskT flags_mask)
{
    m_impl->Open(flags_mask);
}

bool Magic::Open(FlagsMaskT flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->Open(flags_mask, tag);
}

void Magic::Open(const FlagsContainerT& flags_container)
{
    m_impl->Open(flags_container);
}

bool Magic::Open(
    const FlagsContainerT& flags_container,
    const std::nothrow_t&  tag
) noexcept
{
    return m_impl->Open(flags_container, tag);
}

void Magic::SetFlags(FlagsMaskT flags_mask)
{
    m_impl->SetFlags(flags_mask);
}

bool Magic::SetFlags(FlagsMaskT flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->SetFlags(flags_mask, tag);
}

void Magic::SetFlags(const FlagsContainerT& flags_container)
{
    m_impl->SetFlags(flags_container);
}

bool Magic::SetFlags(
    const FlagsContainerT& flags_container,
    const std::nothrow_t&  tag
) noexcept
{
    return m_impl->SetFlags(flags_container, tag);
}

void Magic::SetParameter(Parameters parameter, std::size_t value)
{
    m_impl->SetParameter(parameter, value);
}

bool Magic::SetParameter(
    Parameters            parameter,
    std::size_t           value,
    const std::nothrow_t& tag
) noexcept
{
    return m_impl->SetParameter(parameter, value, tag);
}

void Magic::SetParameters(const ParameterValueMapT& parameters)
{
    m_impl->SetParameters(parameters);
}

bool Magic::SetParameters(
    const ParameterValueMapT& parameters,
    const std::nothrow_t&     tag
) noexcept
{
    return m_impl->SetParameters(parameters, tag);
}
} /* namespace Recognition */
