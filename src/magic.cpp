/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <cmath>
#include <array>
#include <format>
#include <utility>
#include <iostream>

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

    bool check(const std::filesystem::path& database_file) const noexcept
    {
        if (!is_open() || database_file.empty()){
            return false;
        }
        auto result = detail::magic_check(m_cookie.get(), database_file.c_str());
        return result != libmagic_error;
    }

    void close() noexcept
    {
        m_cookie.reset(nullptr);
    }

    bool compile(const std::filesystem::path& database_file) const noexcept
    {
        if (!is_open() || database_file.empty()){
            return false;
        }
        auto result = detail::magic_compile(m_cookie.get(), database_file.c_str());
        return result != libmagic_error;
    }

    [[nodiscard]]
    Flags get_flags() const
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
    Parameters get_parameters() const
    {
        Parameters parameters;
        for (std::size_t i{}; i < libmagic_parameter_count; ++i){
            auto parameter = static_cast<Parameter>(i);
            parameters[parameter] = get_parameter(parameter);
        }
        return parameters;
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
    expected_file_type_t
        identify_file(const std::filesystem::path& path, std::nothrow_t) const noexcept
    {
        if (path.empty()){
            return std::unexpected{empty_path{}.what()};
        }
        if (!is_open()){
            return std::unexpected{magic_is_closed{}.what()};
        }
        auto type_cstr = detail::magic_file(m_cookie.get(), path.c_str());
        if (!type_cstr){
            return std::unexpected{magic_file_error{get_error_message(), path}.what()};
        }
        return {type_cstr};
    }

    [[nodiscard]]
    bool is_open() const noexcept
    {
        return m_cookie != nullptr;
    }

    void load_database_file(const std::filesystem::path& database_file)
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        throw_exception_on_failure<empty_path>(!database_file.empty());
        throw_exception_on_failure<invalid_path>(std::filesystem::is_regular_file(database_file));
        throw_exception_on_failure<magic_load_error>(
            detail::magic_load(m_cookie.get(), database_file.c_str()),
            database_file.c_str()
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
    static constexpr auto libmagic_parameter_count = 9uz;

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
        std::make_pair(MAGIC_NO_COMPRESS_FORK,    "NoCompressFork"),
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
        std::make_pair(MAGIC_NO_CHECK_JSON,       "NoCheckJson"),
        std::make_pair(MAGIC_NO_CHECK_SIMH,       "NoCheckSimh"),
        std::make_pair(MAGIC_NO_CHECK_BUILTIN,    "NoCheckBuiltin")
    };

    static constexpr std::array<libmagic_pair_t, libmagic_parameter_count> libmagic_parameters{
        std::make_pair(MAGIC_PARAM_INDIR_MAX,      "IndirMax"),
        std::make_pair(MAGIC_PARAM_NAME_MAX,       "NameMax"),
        std::make_pair(MAGIC_PARAM_ELF_PHNUM_MAX,  "ElfPhnumMax"),
        std::make_pair(MAGIC_PARAM_ELF_SHNUM_MAX,  "ElfShnumMax"),
        std::make_pair(MAGIC_PARAM_ELF_NOTES_MAX,  "ElfNotesMax"),
        std::make_pair(MAGIC_PARAM_REGEX_MAX,      "RegexMax"),
        std::make_pair(MAGIC_PARAM_BYTES_MAX,      "BytesMax"),
        std::make_pair(MAGIC_PARAM_ENCODING_MAX,   "EncodingMax"),
        std::make_pair(MAGIC_PARAM_ELF_SHSIZE_MAX, "ElfShsizeMax")
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

        operator Flags() const
        {
            if (m_flags.none()){
                libmagic_value_t value = libmagic_pair_converter(libmagic_flag_none);
                return {static_cast<Flag>(value)};
            }
            Flags flags;
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

    friend std::ostream& operator<<(std::ostream& os, Flag flag)
    {
        if (flag == Flag::None){
            return os << libmagic_flag_none.second;
        }
        const auto& flag_name = libmagic_flags[std::log2(static_cast<double>(flag))].second;
        return os << flag_name;
    }

    friend std::ostream& operator<<(std::ostream& os, Parameter parameter)
    {
        const auto& parameter_name = libmagic_parameters[static_cast<std::size_t>(parameter)].second;
        return os << parameter_name;
    }
};

std::ostream& operator<<(std::ostream& os, const magic::Flags& flags)
{
    std::ranges::for_each(flags,
        [&](const auto& flag){
            os << flag << "\n";
        }
    );
    return os;
}

std::ostream& operator<<(std::ostream& os, const magic::Parameters& parameters)
{
    std::ranges::for_each(parameters,
        [&](const auto& parameter){
            const auto& parameter_name = parameter.first;
            const auto& parameter_value = parameter.second;
            os << parameter_name << ": " << parameter_value << "\n";
        }
    );
    return os;
}

magic::magic() noexcept
    : m_impl{std::make_unique<magic_private>()}
{ }

magic::magic(flags_t flags, const std::filesystem::path& database_file)
    : m_impl{std::make_unique<magic_private>(flags, database_file)}
{ }

magic::magic(magic&& other) noexcept
    : m_impl{std::exchange(other.m_impl, std::make_unique<magic_private>())}
{ }

magic& magic::operator=(magic&& other) noexcept
{
    m_impl = std::exchange(other.m_impl, std::make_unique<magic_private>());
    return *this;
}

magic::~magic() = default;

[[nodiscard]]
magic::operator bool() const noexcept
{
    return is_open();
}

bool magic::check(const std::filesystem::path& database_file) const noexcept
{
    return m_impl->check(database_file);
}

void magic::close() noexcept
{
    m_impl->close();
}

bool magic::compile(const std::filesystem::path& database_file) const noexcept
{
    return m_impl->compile(database_file);
}

[[nodiscard]]
magic::Flags magic::get_flags() const
{
    return m_impl->get_flags();
}

[[nodiscard]]
std::size_t magic::get_parameter(magic::Parameter parameter) const
{
    return m_impl->get_parameter(parameter);
}

[[nodiscard]]
magic::Parameters magic::get_parameters() const
{
    return m_impl->get_parameters();
}

[[nodiscard]]
std::string magic::get_version() noexcept
{
    return std::format("{:2}", detail::magic_version() / 100.);
}

[[nodiscard]]
magic::file_type_t magic::identify_file(const std::filesystem::path& path) const
{
    return m_impl->identify_file(path);
}

[[nodiscard]]
magic::expected_file_type_t
    magic::identify_file(const std::filesystem::path& path, std::nothrow_t) const noexcept
{
    return m_impl->identify_file(path, std::nothrow);
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
