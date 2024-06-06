/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <cmath>
#include <array>
#include <format>
#include <utility>

#include <magic.hpp>

namespace recognition {

namespace detail {
#include <magic.h>
} /* namespace detail */

class magic::magic_private {
public:
    magic_private() noexcept = default;

    magic_private(flags_mask_t flags_mask, const std::filesystem::path& database_file)
    {
        open(flags_mask);
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
    flags_container_t get_flags() const
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        return flag_converter(m_flags_mask);
    }

    [[nodiscard]]
    std::size_t get_parameter(parameters parameter) const
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
    parameter_value_map_t get_parameters() const
    {
        parameter_value_map_t parameter_value_map;
        for (std::size_t i{}; i < libmagic_parameter_count; ++i){
            auto parameter = static_cast<parameters>(i);
            parameter_value_map[parameter] = get_parameter(parameter);
        }
        return parameter_value_map;
    }

    [[nodiscard]]
    file_type_t identify_file(const std::filesystem::path& path) const
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        throw_exception_on_failure<empty_path>(!path.empty());
        auto type_cstr = detail::magic_file(m_cookie.get(), path.c_str());
        throw_exception_on_failure<magic_file_error>(type_cstr != nullptr, path);
        return type_cstr;
    }

    [[nodiscard]]
    expected_file_type_t
        identify_file(const std::filesystem::path& path, std::nothrow_t) const noexcept
    {
        if (!is_open()){
            return std::unexpected{magic_is_closed{}.what()};
        }
        if (path.empty()){
            return std::unexpected{empty_path{}.what()};
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

    void open(flags_mask_t flags_mask)
    {
        m_cookie.reset(detail::magic_open(flag_converter(flags_mask)));
        throw_exception_on_failure<magic_open_error>(is_open());
        m_flags_mask = flags_mask;
    }

    void set_flags(flags_mask_t flags_mask)
    {
        throw_exception_on_failure<magic_is_closed>(is_open());
        throw_exception_on_failure<magic_set_flags_error>(
            detail::magic_setflags(m_cookie.get(), flag_converter(flags_mask)),
            flag_converter(flags_mask)
        );
        m_flags_mask = flags_mask;
    }

    void set_parameter(parameters parameter, std::size_t value)
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
            libmagic_pair_converter(libmagic_parameter), value
        );
    }

private:
    using cookie_t = std::unique_ptr<detail::magic_set, decltype(
        [](detail::magic_t cookie) noexcept {
            detail::magic_close(cookie);
        }
    )>;

    cookie_t m_cookie{nullptr};
    flags_mask_t m_flags_mask{0};

    static constexpr auto libmagic_error           = -1;
    static constexpr auto libmagic_flags_count     = flags_mask_t{}.size();
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
        std::make_pair(MAGIC_PARAM_INDIR_MAX,      "indir_max"),
        std::make_pair(MAGIC_PARAM_NAME_MAX,       "name_max"),
        std::make_pair(MAGIC_PARAM_ELF_PHNUM_MAX,  "elf_phnum_max"),
        std::make_pair(MAGIC_PARAM_ELF_SHNUM_MAX,  "elf_shnum_max"),
        std::make_pair(MAGIC_PARAM_ELF_NOTES_MAX,  "elf_notes_max"),
        std::make_pair(MAGIC_PARAM_REGEX_MAX,      "regex_max"),
        std::make_pair(MAGIC_PARAM_BYTES_MAX,      "bytes_max"),
        std::make_pair(MAGIC_PARAM_ENCODING_MAX,   "encoding_max"),
        std::make_pair(MAGIC_PARAM_ELF_SHSIZE_MAX, "elf_shsize_max")
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
        explicit flag_converter(flags_mask_t flags_mask) noexcept
            : m_flags_mask{flags_mask}
        { }

        operator libmagic_value_t() const noexcept
        {
            libmagic_value_t flags = libmagic_pair_converter(libmagic_flag_none);
            for (std::size_t i{}; i < m_flags_mask.size(); ++i){
                if (m_flags_mask[i]){
                    flags |= libmagic_pair_converter(libmagic_flags[i]);
                }
            }
            return flags;
        }

        operator flags_container_t() const
        {
            if (m_flags_mask.none()){
                libmagic_value_t value = libmagic_pair_converter(libmagic_flag_none);
                return {static_cast<flags>(value)};
            }
            flags_container_t flags_container;
            for (std::size_t i{}; i < m_flags_mask.size(); ++i){
                if (m_flags_mask[i]){
                    flags_container.push_back(static_cast<flags>(1ULL << i));
                }
            }
            return flags_container;
        }

        operator libmagic_value_name_t() const
        {
            if (m_flags_mask.none()){
                return libmagic_pair_converter(libmagic_flag_none);
            }
            libmagic_value_name_t flags;
            for (std::size_t i{}; i < m_flags_mask.size(); ++i){
                if (m_flags_mask[i]){
                    flags.append(libmagic_pair_converter(libmagic_flags[i])) += ",";
                }
            }
            flags.erase(flags.find_last_of(','));
            return flags;
        }

        const flags_mask_t m_flags_mask;
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

    friend std::string to_string(flags);
    friend std::string to_string(parameters);
};

std::string to_string(
    const magic::types_of_files_t& types_of_files,
    const std::string& type_separator, const std::string& file_separator)
{
    return utility::to_string(types_of_files, file_separator,
        [&](const magic::types_of_files_t::value_type& type_of_a_file){
            const auto& file = type_of_a_file.first;
            const auto& file_type = type_of_a_file.second;
            return file.string() + type_separator + file_type;
        }
    );
}

std::string to_string(const magic::expected_file_type_t& expected_file_type)
{
    return expected_file_type.value_or(expected_file_type.error());
}

std::string to_string(
    const magic::expected_types_of_files_t& expected_types_of_files,
    const std::string& type_separator, const std::string& file_separator)
{
    return utility::to_string(expected_types_of_files, file_separator,
        [&](const magic::expected_types_of_files_t::value_type& type_of_a_file){
            const auto& file = type_of_a_file.first;
            const auto& expected_file_type = type_of_a_file.second;
            return file.string() + type_separator + to_string(expected_file_type);
        }
    );
}

std::string to_string(magic::flags flag)
{
    if (flag == magic::flags::None){
        return magic::magic_private::libmagic_flag_none.second;
    }
    const auto& flag_name{
        magic::magic_private::libmagic_flags[std::log2(std::to_underlying(flag))].second
    };
    return flag_name;
}

std::string to_string(const magic::flags_container_t& flags, const std::string& separator)
{
    return utility::to_string(flags, separator,
        [](magic::flags flag){
            return to_string(flag);
        }
    );
}

std::string to_string(magic::parameters parameter)
{
    const auto& parameter_name{
        magic::magic_private::libmagic_parameters[std::to_underlying(parameter)].second
    };
    return parameter_name;
}

std::string to_string(
    const magic::parameter_value_map_t& parameters,
    const std::string& value_separator, const std::string& parameter_separator)
{
    return utility::to_string(parameters, parameter_separator,
        [&](const magic::parameter_value_map_t::value_type& parameter_with_value) -> std::string {
            const auto& parameter = parameter_with_value.first;
            const auto& value = parameter_with_value.second;
            return to_string(parameter) + value_separator + std::to_string(value);
        }
    );
}

magic::magic() noexcept
    : m_impl{std::make_unique<magic_private>()}
{ }

magic::magic(flags_mask_t flags_mask, const std::filesystem::path& database_file)
    : m_impl{std::make_unique<magic_private>(flags_mask, database_file)}
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
magic::flags_container_t magic::get_flags() const
{
    return m_impl->get_flags();
}

[[nodiscard]]
std::size_t magic::get_parameter(magic::parameters parameter) const
{
    return m_impl->get_parameter(parameter);
}

[[nodiscard]]
magic::parameter_value_map_t magic::get_parameters() const
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

void magic::open(flags_mask_t flags_mask)
{
    m_impl->open(flags_mask);
}

void magic::set_flags(flags_mask_t flags_mask)
{
    m_impl->set_flags(flags_mask);
}

void magic::set_parameter(magic::parameters parameter, std::size_t value)
{
    m_impl->set_parameter(parameter, value);
}

} /* namespace recognition */
