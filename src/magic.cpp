/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: GPL-3.0-only */

#include <array>
#include <utility>

#include <magic.hpp>

namespace recognition {

namespace detail {
#include <magic.h>
} /* namespace detail */

class magic::magic_private {
public:
    magic_private() noexcept = default;

    magic_private(flags_t flags, const std::filesystem::path& database_file)
    {
        open(flags);
        load_database_file(database_file);
    }

    magic_private(magic_private&&) noexcept = default;

    magic_private(const magic_private&) = delete;

    magic_private& operator=(magic_private&&) noexcept = default;

    magic_private& operator=(const magic_private&) = delete;

    ~magic_private() = default;

    [[nodiscard]]
    operator bool() const noexcept
    {
        return is_open();
    }

    bool check(const std::filesystem::path& database_file) const
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        auto file_path_cstr{
            database_file.empty() ? nullptr : database_file.c_str()
        };
        auto result = detail::magic_check(m_cookie.get(), file_path_cstr);
        return result != libmagic_error;
    }

    void close() noexcept
    {
        m_cookie.reset(nullptr);
    }

    bool compile(const std::filesystem::path& database_file) const
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        auto file_path_cstr{
            database_file.empty() ? nullptr : database_file.c_str()
        };
        auto result = detail::magic_compile(m_cookie.get(), file_path_cstr);
        return result != libmagic_error;
    }

    [[nodiscard]]
    std::vector<Flag> get_flags() const
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        return flag_converter(m_flags);
    }

    [[nodiscard]]
    std::size_t get_parameter(Parameter parameter) const
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        std::size_t value{};
        detail::magic_getparam(
            m_cookie.get(),
            libmagic_pair_converter(
                libmagic_parameters[std::to_underlying(parameter)]
            ),
            &value
        );
        return value;
    }

    [[nodiscard]]
    file_type_t identify_file(const std::filesystem::path& path) const
    {
        throw_exception_on_failure<empty_path>(!path.empty());
        throw_exception_on_failure<magic_is_closed>(is_open());
        auto type_cstr = detail::magic_file(m_cookie.get(), path.c_str());
        throw_exception_on_failure<magic_file_error>(type_cstr != nullptr, path);
        return type_cstr;
    }

    [[nodiscard]]
    std::optional<file_type_t>
        identify_file(const std::filesystem::path& path, std::string& error) const noexcept
    {
        error.erase();
        try {
            return identify_file(path);
        } catch (const std::exception& e){
            error = e.what();
            return {};
        }
    }

    [[nodiscard]]
    bool is_open() const noexcept
    {
        return m_cookie != nullptr;
    }

    void load_database_file(const std::filesystem::path& database_file)
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        const char* database_file_cstr{nullptr};
        if (!database_file.empty()){
            throw_exception_on_failure<invalid_path>(std::filesystem::is_regular_file(database_file));
            database_file_cstr = database_file.c_str();
        }
        throw_exception_on_failure<magic_load_error>(
            detail::magic_load(m_cookie.get(), database_file_cstr),
            database_file_cstr ? database_file_cstr : "default_database_file"
        );
    }

    void open(flags_t flags)
    {
        m_cookie.reset(detail::magic_open(flag_converter(flags)));
        throw_exception_on_failure<magic_open_error>(is_open());
        m_flags = flags;
    }

    void set_flags(flags_t flags)
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        throw_exception_on_failure<magic_set_flags_error>(
            detail::magic_setflags(m_cookie.get(), flag_converter(flags)),
            flag_converter(flags)
        );
        m_flags = flags;
    }

    void set_parameter(Parameter parameter, std::size_t value)
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        const auto& libmagic_parameter{
            libmagic_parameters[std::to_underlying(parameter)]
        };
        throw_exception_on_failure<magic_set_param_error>(
            detail::magic_setparam(
                m_cookie.get(),
                libmagic_pair_converter(libmagic_parameter),
                &value
            ),
            libmagic_pair_converter(libmagic_parameter)
        );
    }

private:
    using cookie_t = std::unique_ptr<detail::magic_set, decltype(
        [](detail::magic_t cookie) noexcept {
            detail::magic_close(cookie);
        }
    )>;

    cookie_t m_cookie{nullptr};
    flags_t m_flags{0};

    static constexpr auto libmagic_error           = -1;
    static constexpr auto libmagic_flags_count     = flags_t{}.size();
    static constexpr auto libmagic_parameter_count = 8uz;

    using libmagic_value_t = int;
    using libmagic_value_name_t  = std::string;
    using libmagic_pair_t  = std::pair<libmagic_value_t, libmagic_value_name_t>;

    static constexpr libmagic_pair_t libmagic_flag_none{
        std::make_pair(MAGIC_NONE,                "None")
    };

    static constexpr std::array<libmagic_pair_t, libmagic_flags_count> libmagic_flags{
        std::make_pair(MAGIC_DEBUG,               "Debug"),
        std::make_pair(MAGIC_SYMLINK,             "Symlink"),
        std::make_pair(MAGIC_COMPRESS,            "Compress"),
        std::make_pair(MAGIC_DEVICES,             "Devices"),
        std::make_pair(MAGIC_MIME_TYPE,           "MimeType"),
        std::make_pair(MAGIC_CONTINUE,            "Continue"),
        std::make_pair(MAGIC_CHECK,               "Check"),
        std::make_pair(MAGIC_PRESERVE_ATIME,      "PreserveAtime"),
        std::make_pair(MAGIC_RAW,                 "Raw"),
        std::make_pair(MAGIC_ERROR,               "Error"),
        std::make_pair(MAGIC_MIME_ENCODING,       "MimeEncoding"),
        std::make_pair(MAGIC_MIME,                "Mime"),
        std::make_pair(MAGIC_APPLE,               "Apple"),
        std::make_pair(MAGIC_EXTENSION,           "Extension"),
        std::make_pair(MAGIC_COMPRESS_TRANSP,     "CompressTransp"),
        std::make_pair(MAGIC_NODESC,              "Nodesc"),
        std::make_pair(MAGIC_NO_CHECK_COMPRESS,   "NoCheckCompress"),
        std::make_pair(MAGIC_NO_CHECK_TAR,        "NoCheckTar"),
        std::make_pair(MAGIC_NO_CHECK_SOFT,       "NoCheckSoft"),
        std::make_pair(MAGIC_NO_CHECK_APPTYPE,    "NoCheckApptype"),
        std::make_pair(MAGIC_NO_CHECK_ELF,        "NoCheckElf"),
        std::make_pair(MAGIC_NO_CHECK_TEXT,       "NoCheckText"),
        std::make_pair(MAGIC_NO_CHECK_CDF,        "NoCheckCdf"),
        std::make_pair(MAGIC_NO_CHECK_CSV,        "NoCheckCsv"),
        std::make_pair(MAGIC_NO_CHECK_TOKENS,     "NoCheckTokens"),
        std::make_pair(MAGIC_NO_CHECK_ENCODING,   "NoCheckEncoding"),
        std::make_pair(MAGIC_NO_CHECK_JSON,       "NoCheckJson")
    };

    static constexpr std::array<libmagic_pair_t, libmagic_parameter_count> libmagic_parameters{
        std::make_pair(MAGIC_PARAM_INDIR_MAX,     "IndirMax"),
        std::make_pair(MAGIC_PARAM_NAME_MAX,      "NameMax"),
        std::make_pair(MAGIC_PARAM_ELF_PHNUM_MAX, "ElfPhnumMax"),
        std::make_pair(MAGIC_PARAM_ELF_SHNUM_MAX, "ElfShnumMax"),
        std::make_pair(MAGIC_PARAM_ELF_NOTES_MAX, "ElfNotesMax"),
        std::make_pair(MAGIC_PARAM_REGEX_MAX,     "RegexMax"),
        std::make_pair(MAGIC_PARAM_BYTES_MAX,     "BytesMax"),
        std::make_pair(MAGIC_PARAM_ENCODING_MAX,  "EncodingMax")
    };

    /**
     * @brief Throws an ExceptionType exception with ExceptionArgs
     *        if the result is false or equal to libmagic_error.
     */
    template <std::derived_from<magic_exception> ExceptionType,
             typename ResultType, typename ...ExceptionArgs>
    requires std::same_as<ResultType, int> || std::same_as<ResultType, bool>
    void throw_exception_on_failure(ResultType result, ExceptionArgs&& ...args) const
    {
        bool throw_exception = false;
        if constexpr (std::same_as<int, ResultType>){
            throw_exception = result == libmagic_error;
        } else if constexpr (std::same_as<ResultType, bool>){
            throw_exception = !result;
        }
        if (!throw_exception){
            return;
        }
        if constexpr (std::default_initializable<ExceptionType>){
            throw ExceptionType{};
        } else {
            throw ExceptionType{get_error_message(), std::forward<ExceptionArgs>(args)...};
        }
    }

    [[nodiscard]]
    std::string get_error_message() const noexcept
    {
        const char* magic_error_cstr = detail::magic_error(m_cookie.get());
        return magic_error_cstr ? magic_error_cstr : "";
    }

    struct flag_converter {
        explicit flag_converter(flags_t flags) noexcept
            : m_flags{flags}
        { }

        operator libmagic_value_t() const noexcept
        {
            libmagic_value_t flags = libmagic_pair_converter(libmagic_flag_none);
            for (std::size_t i{}; i < m_flags.size(); ++i){
                if (m_flags[i]){
                    flags |= libmagic_pair_converter(libmagic_flags[i]);
                }
            }
            return flags;
        }

        operator std::vector<Flag>() const
        {
            if (m_flags.none()){
                libmagic_value_t value = libmagic_pair_converter(libmagic_flag_none);
                return {static_cast<Flag>(value)};
            }
            std::vector<Flag> flags;
            for (std::size_t i{}; i < m_flags.size(); ++i){
                if (m_flags[i]){
                    flags.push_back(static_cast<Flag>(1ULL << i));
                }
            }
            return flags;
        }

        operator libmagic_value_name_t() const
        {
            if (m_flags.none()){
                return libmagic_pair_converter(libmagic_flag_none);
            }
            libmagic_value_name_t flags;
            for (std::size_t i{}; i < m_flags.size(); ++i){
                if (m_flags[i]){
                    flags.append(libmagic_pair_converter(libmagic_flags[i])) += ",";
                }
            }
            flags.erase(flags.find_last_of(','));
            return flags;
        }

        const flags_t m_flags;
    };

    struct libmagic_pair_converter {
        constexpr explicit libmagic_pair_converter(const libmagic_pair_t& pair) noexcept
            : m_pair{pair}
        { }

        constexpr operator libmagic_value_name_t() const noexcept
        {
            return std::get<libmagic_value_name_t>(m_pair);
        }

        constexpr operator libmagic_value_t() const noexcept
        {
            return std::get<libmagic_value_t>(m_pair);
        }

        const libmagic_pair_t& m_pair;
    };
};

magic::magic() noexcept = default;

magic::magic(flags_t flags, const std::filesystem::path& database_file)
    : m_impl{std::make_unique<magic_private>(flags, database_file)}
{ }

magic::magic(magic&&) noexcept = default;

magic& magic::operator=(magic&&) noexcept = default;

magic::~magic() = default;

[[nodiscard]]
magic::operator bool() const noexcept
{
    return m_impl->operator bool();
}

bool magic::check(const std::filesystem::path& database_file) const
{
    return m_impl->check(database_file);
}

void magic::close() noexcept
{
    m_impl->close();
}

bool magic::compile(const std::filesystem::path& database_file) const
{
    return m_impl->compile(database_file);
}

[[nodiscard]]
std::vector<magic::Flag> magic::get_flags() const
{
    return m_impl->get_flags();
}

[[nodiscard]]
std::size_t magic::get_parameter(magic::Parameter parameter) const
{
    return m_impl->get_parameter(parameter);
}

[[nodiscard]]
std::string magic::get_version() noexcept
{
    auto version_str{std::to_string(detail::magic_version())};
    if (version_str.size() >= 3uz){
        version_str.insert(std::end(version_str) - 2uz, '.');
    }
    return version_str;
}

[[nodiscard]]
magic::file_type_t magic::identify_file(const std::filesystem::path& path) const
{
    return m_impl->identify_file(path);
}

[[nodiscard]]
std::optional<magic::file_type_t>
    magic::identify_file(const std::filesystem::path& path, std::string& error) const noexcept
{
    return m_impl->identify_file(path, error);
}

[[nodiscard]]
bool magic::is_open() const noexcept
{
    return m_impl->is_open();
}

void magic::load_database_file(const std::filesystem::path& database_file)
{
    m_impl->load_database_file(database_file);
}

void magic::open(flags_t flags)
{
    m_impl->open(flags);
}

void magic::set_flags(flags_t flags)
{
    m_impl->set_flags(flags);
}

void magic::set_parameter(magic::Parameter parameter, std::size_t value)
{
    m_impl->set_parameter(parameter, value);
}

} /* namespace recognition */
