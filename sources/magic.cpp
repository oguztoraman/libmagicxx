/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <cmath>

#include <array>
#include <utility>

#include "magic.hpp"

namespace recognition {

namespace detail {
#include <magic.h>
} /* namespace detail */

class magic::magic_private {
public:
    using identify_file_options_mask_t = std::bitset<3uz>;

    enum identify_file_options : unsigned long long {
        check_nothing     = 0ULL,
        check_is_valid    = 1ULL << 0,
        check_path_empty  = 1ULL << 1,
        check_path_exists = 1ULL << 2,
        check_path        = check_path_empty | check_path_exists,
        check_everything  = check_is_valid | check_path
    };

    magic_private() noexcept = default;

    magic_private(
        flags_mask_t                 flags_mask,
        const std::filesystem::path& database_file
    )
    {
        open(flags_mask);
        load_database_file(database_file);
    }

    magic_private(
        flags_mask_t                           flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path&           database_file
    ) noexcept
    {
        static_cast<void>(
            open(flags_mask, std::nothrow)
            && load_database_file(std::nothrow, database_file)
        );
    }

    magic_private(
        const flags_container_t&     flags_container,
        const std::filesystem::path& database_file
    )
    {
        open(flags_container);
        load_database_file(database_file);
    }

    magic_private(
        const flags_container_t&               flags_container,
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path&           database_file
    ) noexcept
    {
        static_cast<void>(
            open(flags_container, std::nothrow)
            && load_database_file(std::nothrow, database_file)
        );
    }

    magic_private(magic_private&&) noexcept = default;

    magic_private(const magic_private&) = delete;

    magic_private& operator=(magic_private&&) noexcept = default;

    magic_private& operator=(const magic_private&) = delete;

    ~magic_private() = default;

    [[nodiscard]] bool check(const std::filesystem::path& database_file
    ) const noexcept
    {
        if (!is_open() || database_file.empty()) {
            return false;
        }
        using detail::magic_check;
        auto result = magic_check(
            m_cookie.get(),
            database_file.string().c_str()
        );
        return result != libmagic_error;
    }

    [[nodiscard]] bool compile(const std::filesystem::path& database_file
    ) const noexcept
    {
        if (!is_open() || database_file.empty()) {
            return false;
        }
        using detail::magic_compile;
        auto result = magic_compile(
            m_cookie.get(),
            database_file.string().c_str()
        );
        return result != libmagic_error;
    }

    [[nodiscard]] flags_container_t get_flags() const
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
        return flags_converter(m_flags_mask);
    }

    [[nodiscard]] std::optional<flags_container_t> get_flags(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!is_open()) {
            return std::nullopt;
        }
        return {flags_converter(m_flags_mask)};
    }

    [[nodiscard]] std::size_t get_parameter(parameters parameter) const
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
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

    [[nodiscard]] std::optional<std::size_t> get_parameter(
        parameters                             parameter,
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!is_open()) {
            return std::nullopt;
        }
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

    [[nodiscard]] parameter_value_map_t get_parameters() const
    {
        parameter_value_map_t parameter_value_map;
        for (std::size_t i{}; i < libmagic_parameter_count; ++i) {
            auto parameter                 = static_cast<parameters>(i);
            parameter_value_map[parameter] = get_parameter(parameter);
        }
        return parameter_value_map;
    }

    [[nodiscard]] std::optional<parameter_value_map_t> get_parameters(
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if (!is_open()) {
            return std::nullopt;
        }
        parameter_value_map_t parameter_value_map;
        for (std::size_t i{}; i < libmagic_parameter_count; ++i) {
            auto parameter                 = static_cast<parameters>(i);
            parameter_value_map[parameter] = get_parameter(parameter);
        }
        return parameter_value_map;
    }

    [[nodiscard]] file_type_t identify_file(
        const std::filesystem::path& path,
        identify_file_options_mask_t options
    ) const
    {
        if ((options & check_is_valid_option) == check_is_valid_option) {
            magic_private::throw_exception_on_failure<magic_is_closed>(is_open()
            );
            magic_private::throw_exception_on_failure<
                magic_database_not_loaded>(is_database_loaded());
        }
        if ((options & check_path_empty_option) == check_path_empty_option) {
            magic_private::throw_exception_on_failure<empty_path>(!path.empty()
            );
        }
        if ((options & check_path_exists_option) == check_path_exists_option) {
            std::error_code error_code{};
            magic_private::throw_exception_on_failure<path_does_not_exist>(
                std::filesystem::exists(path, error_code),
                path.string()
            );
        }
        auto type_cstr = detail::magic_file(
            m_cookie.get(),
            path.string().c_str()
        );
        magic_private::throw_exception_on_failure<magic_identify_file_error>(
            type_cstr != nullptr,
            get_error_message(),
            path.string()
        );
        return type_cstr;
    }

    [[nodiscard]] expected_file_type_t identify_file(
        const std::filesystem::path&           path,
        identify_file_options_mask_t           options,
        [[maybe_unused]] const std::nothrow_t& tag
    ) const noexcept
    {
        if ((options & check_is_valid_option) == check_is_valid_option) {
            if (!is_open()) {
                return std::unexpected{magic_is_closed{}.what()};
            }
            if (!is_database_loaded()) {
                return std::unexpected{magic_database_not_loaded{}.what()};
            }
        }
        if ((options & check_path_empty_option) == check_path_empty_option) {
            if (path.empty()) {
                return std::unexpected{empty_path{}.what()};
            }
        }
        if ((options & check_path_exists_option) == check_path_exists_option) {
            std::error_code error_code{};
            if (!std::filesystem::exists(path, error_code)) {
                return std::unexpected{path_does_not_exist{path.string()}.what()
                };
            }
        }
        auto type_cstr = detail::magic_file(
            m_cookie.get(),
            path.string().c_str()
        );
        if (!type_cstr) {
            return std::unexpected{
                magic_identify_file_error{get_error_message(), path.string()}
                    .what()
            };
        }
        return {type_cstr};
    }

    [[nodiscard]] default_file_container_t
        identify_directory_preliminary_checks(
            const std::filesystem::path&       directory,
            std::filesystem::directory_options option,
            tracker_t                          tracker
        ) const
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
        magic_private::throw_exception_on_failure<magic_database_not_loaded>(
            is_database_loaded()
        );
        magic_private::throw_exception_on_failure<empty_path>(!directory.empty()
        );
        std::error_code error_code{};
        magic_private::throw_exception_on_failure<path_does_not_exist>(
            std::filesystem::exists(directory, error_code),
            directory.string()
        );
        magic_private::throw_exception_on_failure<path_is_not_directory>(
            std::filesystem::is_directory(directory, error_code),
            directory.string()
        );
        magic_private::throw_exception_on_failure<null_tracker>(
            tracker != nullptr
        );
        auto files = std::filesystem::recursive_directory_iterator{
            directory,
            option,
            error_code
        };
        magic_private::throw_exception_on_failure<filesystem_error>(
            !error_code,
            directory.string(),
            error_code.message()
        );
        return {std::ranges::begin(files), std::ranges::end(files)};
    }

    [[nodiscard]] std::optional<default_file_container_t>
        identify_directory_preliminary_checks(
            const std::filesystem::path&           directory,
            [[maybe_unused]] const std::nothrow_t& tag,
            std::filesystem::directory_options     option,
            tracker_t                              tracker
        ) const noexcept
    {
        if (!is_valid()) {
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
        if (!tracker) {
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

    void identify_container_preliminary_checks(tracker_t tracker) const
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
        magic_private::throw_exception_on_failure<magic_database_not_loaded>(
            is_database_loaded()
        );
        magic_private::throw_exception_on_failure<null_tracker>(
            tracker != nullptr
        );
    }

    bool identify_container_preliminary_checks(
        [[maybe_unused]] const std::nothrow_t& tag,
        tracker_t                              tracker
    ) const noexcept
    {
        return is_valid() && tracker;
    }

    [[nodiscard]] types_of_files_t identify_files(
        const default_file_container_t& files,
        identify_file_options_mask_t    option,
        tracker_t                       tracker
    ) const
    {
        types_of_files_t types_of_files;
        tracker->reset(std::ranges::distance(files));
        std::ranges::for_each(files, [&](const auto& file) {
            utility::advance_tracker advance{tracker};
            types_of_files[file] = identify_file(file, option);
        });
        tracker->mark_as_completed();
        return types_of_files;
    }

    [[nodiscard]] expected_types_of_files_t identify_files(
        const default_file_container_t&        files,
        [[maybe_unused]] const std::nothrow_t& tag,
        identify_file_options_mask_t           option,
        tracker_t                              tracker
    ) const noexcept
    {
        expected_types_of_files_t expected_types_of_files;
        option &= ~check_path_empty_option;
        tracker->reset(std::ranges::distance(files));
        std::ranges::for_each(files, [&](const auto& file) {
            utility::advance_tracker advance{tracker};
            if (file.empty()) {
                return;
            }
            expected_types_of_files[file] = identify_file(
                file,
                option,
                std::nothrow
            );
        });
        return expected_types_of_files;
    }

    [[nodiscard]] bool is_database_loaded() const noexcept
    {
        return m_is_database_loaded;
    }

    [[nodiscard]] bool is_open() const noexcept
    {
        return m_cookie != nullptr;
    }

    [[nodiscard]] bool is_valid() const noexcept
    {
        return is_open() && is_database_loaded();
    }

    void load_database_file(const std::filesystem::path& database_file)
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
        magic_private::throw_exception_on_failure<empty_path>(
            !database_file.empty()
        );
        std::error_code error_code{};
        magic_private::throw_exception_on_failure<path_does_not_exist>(
            std::filesystem::exists(database_file, error_code),
            database_file.string()
        );
        magic_private::throw_exception_on_failure<path_is_not_regular_file>(
            std::filesystem::is_regular_file(database_file, error_code),
            database_file.string()
        );
        m_is_database_loaded = false;
        magic_private::throw_exception_on_failure<
            magic_load_database_file_error>(
            detail::magic_load(m_cookie.get(), database_file.string().c_str()),
            get_error_message(),
            database_file.string()
        );
        m_is_database_loaded = true;
    }

    [[nodiscard]] bool load_database_file(
        [[maybe_unused]] const std::nothrow_t& tag,
        const std::filesystem::path&           database_file
    ) noexcept
    {
        if (!is_open() || database_file.empty()) {
            return false;
        }
        std::error_code error_code{};
        if (!std::filesystem::exists(database_file, error_code)) {
            return false;
        }
        if (!std::filesystem::is_regular_file(database_file, error_code)) {
            return false;
        }
        m_is_database_loaded = detail::magic_load(
                                   m_cookie.get(),
                                   database_file.string().c_str()
                               )
                            != libmagic_error;
        return is_database_loaded();
    }

    void open(flags_mask_t flags_mask)
    {
        m_is_database_loaded = false;
        m_cookie.reset(detail::magic_open(flags_converter(flags_mask)));
        magic_private::throw_exception_on_failure<magic_open_error>(
            is_open(),
            get_error_message()
        );
        m_flags_mask = flags_mask;
    }

    [[nodiscard]] bool open(
        flags_mask_t                           flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        m_is_database_loaded = false;
        m_cookie.reset(detail::magic_open(flags_converter(flags_mask)));
        if (!is_open()) {
            return false;
        }
        m_flags_mask = flags_mask;
        return true;
    }

    void open(const flags_container_t& flags_container)
    {
        open(flags_mask_t{flags_converter(flags_container)});
    }

    [[nodiscard]] bool open(
        const flags_container_t&               flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return open(
            flags_mask_t{flags_converter(flags_container)},
            std::nothrow
        );
    }

    void set_flags(flags_mask_t flags_mask)
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
        magic_private::throw_exception_on_failure<magic_set_flags_error>(
            detail::magic_setflags(m_cookie.get(), flags_converter(flags_mask)),
            get_error_message(),
            flags_converter(flags_mask)
        );
        m_flags_mask = flags_mask;
    }

    [[nodiscard]] bool set_flags(
        flags_mask_t                           flags_mask,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        if (!is_open()) {
            return false;
        }
        auto result = detail::magic_setflags(
                          m_cookie.get(),
                          flags_converter(flags_mask)
                      )
                   != libmagic_error;
        if (!result) {
            return false;
        }
        m_flags_mask = flags_mask;
        return true;
    }

    void set_flags(const flags_container_t& flags_container)
    {
        set_flags(flags_mask_t{flags_converter(flags_container)});
    }

    [[nodiscard]] bool set_flags(
        const flags_container_t&               flags_container,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return set_flags(
            flags_mask_t{flags_converter(flags_container)},
            std::nothrow
        );
    }

    void set_parameter(parameters parameter, std::size_t value)
    {
        magic_private::throw_exception_on_failure<magic_is_closed>(is_open());
        const auto& libmagic_parameter{
            libmagic_parameters[std::to_underlying(parameter)]
        };
        magic_private::throw_exception_on_failure<magic_set_parameter_error>(
            detail::magic_setparam(
                m_cookie.get(),
                libmagic_pair_converter(libmagic_parameter),
                &value
            ),
            get_error_message(),
            libmagic_pair_converter(libmagic_parameter),
            value
        );
    }

    [[nodiscard]] bool set_parameter(
        parameters                             parameter,
        std::size_t                            value,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        if (!is_open()) {
            return false;
        }
        const auto& libmagic_parameter{
            libmagic_parameters[std::to_underlying(parameter)]
        };
        return detail::magic_setparam(
                   m_cookie.get(),
                   libmagic_pair_converter(libmagic_parameter),
                   &value
               )
            != libmagic_error;
    }

    void set_parameters(const parameter_value_map_t& parameters)
    {
        std::ranges::for_each(
            parameters,
            [&](const auto& parameter_value_pair) {
                const auto& parameter = parameter_value_pair.first;
                const auto& value     = parameter_value_pair.second;
                set_parameter(parameter, value);
            }
        );
    }

    [[nodiscard]] bool set_parameters(
        const parameter_value_map_t&           parameters,
        [[maybe_unused]] const std::nothrow_t& tag
    ) noexcept
    {
        return std::ranges::find_if_not(
                   parameters,
                   [&](const auto& parameter_value_pair) {
                       const auto& parameter = parameter_value_pair.first;
                       const auto& value     = parameter_value_pair.second;
                       return set_parameter(parameter, value, std::nothrow);
                   }
               )
            == parameters.end();
    }

    /**
     * @brief Throws an ExceptionType exception with ExceptionArgs
     *        if the result is false or equal to libmagic_error.
     */
    template <
        std::derived_from<magic_exception> ExceptionType,
        typename ResultType,
        typename... ExceptionArgs>
    requires std::same_as<ResultType, int> || std::same_as<ResultType, bool>
    static void throw_exception_on_failure(
        ResultType result,
        ExceptionArgs&&... args
    )
    {
        bool throw_exception = false;
        if constexpr (std::same_as<int, ResultType>) {
            throw_exception = result == libmagic_error;
        } else if constexpr (std::same_as<ResultType, bool>) {
            throw_exception = !result;
        }
        if (!throw_exception) {
            return;
        }
        throw ExceptionType{std::forward<ExceptionArgs>(args)...};
    }

private:
    using cookie_t = std::unique_ptr<
        detail::magic_set,
        decltype([](detail::magic_t cookie) noexcept {
            detail::magic_close(cookie);
        })>;

    cookie_t     m_cookie{nullptr};
    flags_mask_t m_flags_mask{0};
    bool         m_is_database_loaded{false};

    static constexpr identify_file_options_mask_t check_is_valid_option{
        identify_file_options::check_is_valid
    };
    static constexpr identify_file_options_mask_t check_path_empty_option{
        identify_file_options::check_path_empty
    };
    static constexpr identify_file_options_mask_t check_path_exists_option{
        identify_file_options::check_path_exists
    };

    static constexpr auto libmagic_error           = -1;
    static constexpr auto libmagic_flags_count     = flags_mask_t{}.size();
    static constexpr auto libmagic_parameter_count = 10uz;

    using libmagic_value_t      = int;
    using libmagic_value_name_t = std::string;
    using libmagic_pair_t       = std::pair<libmagic_value_t, const char*>;
    using libmagic_flags_t = std::array<libmagic_pair_t, libmagic_flags_count>;
    using libmagic_parameters_t = std::array<
        libmagic_pair_t,
        libmagic_parameter_count>;

    static constexpr libmagic_pair_t libmagic_flag_none{
        std::make_pair(MAGIC_NONE, "none")
    };

    static constexpr libmagic_flags_t libmagic_flags{
        std::make_pair(MAGIC_DEBUG, "debug"),
        std::make_pair(MAGIC_SYMLINK, "symlink"),
        std::make_pair(MAGIC_COMPRESS, "compress"),
        std::make_pair(MAGIC_DEVICES, "devices"),
        std::make_pair(MAGIC_MIME_TYPE, "mime_type"),
        std::make_pair(MAGIC_CONTINUE, "continue_search"),
        std::make_pair(MAGIC_CHECK, "check_database"),
        std::make_pair(MAGIC_PRESERVE_ATIME, "preserve_atime"),
        std::make_pair(MAGIC_RAW, "raw"),
        std::make_pair(MAGIC_ERROR, "error"),
        std::make_pair(MAGIC_MIME_ENCODING, "mime_encoding"),
        std::make_pair(MAGIC_MIME, "mime"),
        std::make_pair(MAGIC_APPLE, "apple"),
        std::make_pair(MAGIC_EXTENSION, "extension"),
        std::make_pair(MAGIC_COMPRESS_TRANSP, "compress_transp"),
        std::make_pair(MAGIC_NO_COMPRESS_FORK, "no_compress_fork"),
        std::make_pair(MAGIC_NODESC, "nodesc"),
        std::make_pair(MAGIC_NO_CHECK_COMPRESS, "no_check_compress"),
        std::make_pair(MAGIC_NO_CHECK_TAR, "no_check_tar"),
        std::make_pair(MAGIC_NO_CHECK_SOFT, "no_check_soft"),
        std::make_pair(MAGIC_NO_CHECK_APPTYPE, "no_check_apptype"),
        std::make_pair(MAGIC_NO_CHECK_ELF, "no_check_elf"),
        std::make_pair(MAGIC_NO_CHECK_TEXT, "no_check_text"),
        std::make_pair(MAGIC_NO_CHECK_CDF, "no_check_cdf"),
        std::make_pair(MAGIC_NO_CHECK_CSV, "no_check_csv"),
        std::make_pair(MAGIC_NO_CHECK_TOKENS, "no_check_tokens"),
        std::make_pair(MAGIC_NO_CHECK_ENCODING, "no_check_encoding"),
        std::make_pair(MAGIC_NO_CHECK_JSON, "no_check_json"),
        std::make_pair(MAGIC_NO_CHECK_SIMH, "no_check_simh"),
        std::make_pair(MAGIC_NO_CHECK_BUILTIN, "no_check_builtin")
    };

    static constexpr libmagic_parameters_t libmagic_parameters{
        std::make_pair(MAGIC_PARAM_INDIR_MAX, "indir_max"),
        std::make_pair(MAGIC_PARAM_NAME_MAX, "name_max"),
        std::make_pair(MAGIC_PARAM_ELF_PHNUM_MAX, "elf_phnum_max"),
        std::make_pair(MAGIC_PARAM_ELF_SHNUM_MAX, "elf_shnum_max"),
        std::make_pair(MAGIC_PARAM_ELF_NOTES_MAX, "elf_notes_max"),
        std::make_pair(MAGIC_PARAM_REGEX_MAX, "regex_max"),
        std::make_pair(MAGIC_PARAM_BYTES_MAX, "bytes_max"),
        std::make_pair(MAGIC_PARAM_ENCODING_MAX, "encoding_max"),
        std::make_pair(MAGIC_PARAM_ELF_SHSIZE_MAX, "elf_shsize_max"),
        std::make_pair(MAGIC_PARAM_MAGWARN_MAX, "mag_warn_max")
    };

    [[nodiscard]] std::string get_error_message() const noexcept
    {
        const char* magic_error_cstr = detail::magic_error(m_cookie.get());
        return magic_error_cstr ? magic_error_cstr : "";
    }

    struct flags_converter {
        explicit flags_converter(const flags_container_t& flags_container
        ) noexcept
          : m_flags_mask{std::ranges::fold_left(
                flags_container,
                flags_container.empty() ? flags::none : flags_container.front(),
                std::bit_or<decltype(1ULL)>{}
            )}
        { }

        explicit flags_converter(flags_mask_t flags_mask) noexcept
          : m_flags_mask{flags_mask}
        { }

        operator libmagic_value_t() const noexcept
        {
            libmagic_value_t flags = libmagic_pair_converter(libmagic_flag_none
            );
            for (std::size_t i{}; i < m_flags_mask.size(); ++i) {
                if (m_flags_mask[i]) {
                    flags |= libmagic_pair_converter(libmagic_flags[i]);
                }
            }
            return flags;
        }

        operator flags_container_t() const
        {
            if (m_flags_mask.none()) {
                libmagic_value_t value = libmagic_pair_converter(
                    libmagic_flag_none
                );
                return {static_cast<flags>(value)};
            }
            flags_container_t flags_container;
            for (std::size_t i{}; i < m_flags_mask.size(); ++i) {
                if (m_flags_mask[i]) {
                    flags_container.push_back(static_cast<flags>(1ULL << i));
                }
            }
            return flags_container;
        }

        operator libmagic_value_name_t() const
        {
            if (m_flags_mask.none()) {
                return libmagic_pair_converter(libmagic_flag_none);
            }
            libmagic_value_name_t flags;
            for (std::size_t i{}; i < m_flags_mask.size(); ++i) {
                if (m_flags_mask[i]) {
                    flags.append(libmagic_pair_converter(libmagic_flags[i])
                    ) += ",";
                }
            }
            flags.erase(flags.find_last_of(','));
            return flags;
        }

        operator flags_mask_t() const
        {
            return m_flags_mask;
        }

        const flags_mask_t m_flags_mask;
    };

    struct libmagic_pair_converter {
        constexpr explicit libmagic_pair_converter(const libmagic_pair_t& pair
        ) noexcept
          : m_pair{pair}
        { }

        constexpr operator libmagic_value_name_t() const noexcept
        {
            return std::get<const char*>(m_pair);
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
    const magic::type_of_a_file_t& type_of_a_file,
    const std::string&             type_separator
)
{
    const auto& file      = type_of_a_file.first;
    const auto& file_type = type_of_a_file.second;
    return file.string() + type_separator + file_type;
}

std::string to_string(
    const magic::types_of_files_t& types_of_files,
    const std::string&             type_separator,
    const std::string&             file_separator
)
{
    return utility::to_string(
        types_of_files,
        file_separator,
        [&type_separator](const magic::type_of_a_file_t& type_of_a_file) {
            return to_string(type_of_a_file, type_separator);
        }
    );
}

std::string to_string(const magic::expected_file_type_t& expected_file_type)
{
    return expected_file_type.value_or(expected_file_type.error());
}

std::string to_string(
    const magic::expected_type_of_a_file_t& expected_type_of_a_file,
    const std::string&                      type_separator
)
{
    const auto& file               = expected_type_of_a_file.first;
    const auto& expected_file_type = expected_type_of_a_file.second;
    return std::format(
        "{}{}{}",
        file.string(),
        type_separator,
        to_string(expected_file_type)
    );
}

std::string to_string(
    const magic::expected_types_of_files_t& expected_types_of_files,
    const std::string&                      type_separator,
    const std::string&                      file_separator
)
{
    return utility::to_string(
        expected_types_of_files,
        file_separator,
        [&type_separator](const magic::expected_type_of_a_file_t& type_of_a_file
        ) {
            return to_string(type_of_a_file, type_separator);
        }
    );
}

std::string to_string(magic::flags flag)
{
    if (flag == magic::flags::none) {
        return magic::magic_private::libmagic_flag_none.second;
    }
    const auto& flags = magic::magic_private::libmagic_flags;
    const auto& flag_name{flags[std::log2(std::to_underlying(flag))].second};
    return flag_name;
}

std::string to_string(
    const magic::flags_container_t& flags,
    const std::string&              separator
)
{
    return utility::to_string(flags, separator, [](magic::flags flag) {
        return to_string(flag);
    });
}

std::string to_string(magic::parameters parameter)
{
    const auto& parameters = magic::magic_private::libmagic_parameters;
    const auto& parameter_name{parameters[std::to_underlying(parameter)].second
    };
    return parameter_name;
}

std::string to_string(
    const magic::parameter_value_t& parameter_value,
    const std::string&              value_separator
)
{
    const auto& parameter = parameter_value.first;
    const auto& value     = parameter_value.second;
    return std::format("{}{}{}", to_string(parameter), value_separator, value);
}

std::string to_string(
    const magic::parameter_value_map_t& parameters,
    const std::string&                  value_separator,
    const std::string&                  parameter_separator
)
{
    return utility::to_string(
        parameters,
        parameter_separator,
        [&value_separator](const magic::parameter_value_t& parameter_value) {
            return to_string(parameter_value, value_separator);
        }
    );
}

std::string_view magic::default_database_file{DEFAULT_DATABASE_FILE};

magic::magic() noexcept
  : m_impl{std::make_unique<magic_private>()}
{ }

magic::magic(
    flags_mask_t                 flags_mask,
    const std::filesystem::path& database_file
)
  : m_impl{std::make_unique<magic_private>(flags_mask, database_file)}
{ }

magic::magic(
    flags_mask_t                 flags_mask,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<magic_private>(flags_mask, tag, database_file)}
{ }

magic::magic(
    const flags_container_t&     flags_container,
    const std::filesystem::path& database_file
)
  : m_impl{std::make_unique<magic_private>(flags_container, database_file)}
{ }

magic::magic(
    const flags_container_t&     flags_container,
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
  : m_impl{std::make_unique<magic_private>(flags_container, tag, database_file)}
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

magic::operator bool() const noexcept
{
    return is_valid();
}

bool magic::check(const std::filesystem::path& database_file) noexcept
{
    magic_private magic_for_check{};
    return magic_for_check.open(magic::flags::none, std::nothrow)
        && magic_for_check.check(database_file);
}

void magic::close() noexcept
{
    m_impl = std::make_unique<magic_private>();
}

bool magic::compile(const std::filesystem::path& database_file) noexcept
{
    magic_private magic_for_compile{};
    return magic_for_compile.open(magic::flags::none, std::nothrow)
        && magic_for_compile.compile(database_file);
}

magic::flags_container_t magic::get_flags() const
{
    return m_impl->get_flags();
}

std::optional<magic::flags_container_t> magic::get_flags(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->get_flags(tag);
}

std::size_t magic::get_parameter(magic::parameters parameter) const
{
    return m_impl->get_parameter(parameter);
}

std::optional<std::size_t> magic::get_parameter(
    parameters            parameter,
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->get_parameter(parameter, tag);
}

magic::parameter_value_map_t magic::get_parameters() const
{
    return m_impl->get_parameters();
}

std::optional<magic::parameter_value_map_t> magic::get_parameters(
    const std::nothrow_t& tag
) const noexcept
{
    return m_impl->get_parameters(tag);
}

std::string magic::get_version() noexcept
{
    return std::format("{:2}", detail::magic_version() / 100.);
}

magic::file_type_t magic::identify_file(const std::filesystem::path& path) const
{
    return m_impl->identify_file(
        path,
        magic_private::identify_file_options::check_everything
    );
}

magic::expected_file_type_t magic::identify_file(
    const std::filesystem::path& path,
    const std::nothrow_t&        tag
) const noexcept
{
    return m_impl->identify_file(
        path,
        magic_private::identify_file_options::check_everything,
        tag
    );
}

magic::types_of_files_t magic::identify_directory_impl(
    const std::filesystem::path&       directory,
    std::filesystem::directory_options option,
    tracker_t                          tracker
) const
{
    return m_impl->identify_files(
        m_impl
            ->identify_directory_preliminary_checks(directory, option, tracker),
        magic_private::identify_file_options::check_nothing,
        tracker
    );
}

magic::expected_types_of_files_t magic::identify_directory_impl(
    const std::filesystem::path&       directory,
    const std::nothrow_t&              tag,
    std::filesystem::directory_options option,
    tracker_t                          tracker
) const noexcept
{
    auto files = m_impl->identify_directory_preliminary_checks(
        directory,
        tag,
        option,
        tracker
    );
    utility::mark_tracker_as_completed marker{tracker};
    if (!files) {
        return {};
    }
    return m_impl->identify_files(
        files.value(),
        tag,
        magic_private::identify_file_options::check_nothing,
        tracker
    );
}

magic::types_of_files_t magic::identify_container_impl(
    const default_file_container_t& files,
    tracker_t                       tracker
) const
{
    m_impl->identify_container_preliminary_checks(tracker);
    return m_impl->identify_files(
        files,
        magic_private::identify_file_options::check_path,
        tracker
    );
}

magic::expected_types_of_files_t magic::identify_container_impl(
    const default_file_container_t& files,
    const std::nothrow_t&           tag,
    tracker_t                       tracker
) const noexcept
{
    utility::mark_tracker_as_completed marker{tracker};
    if (!m_impl->identify_container_preliminary_checks(tag, tracker)) {
        return {};
    }
    return m_impl->identify_files(
        files,
        tag,
        magic_private::identify_file_options::check_path_exists,
        tracker
    );
}

bool magic::is_database_loaded() const noexcept
{
    return m_impl->is_database_loaded();
}

bool magic::is_open() const noexcept
{
    return m_impl->is_open();
}

bool magic::is_valid() const noexcept
{
    return m_impl->is_valid();
}

void magic::load_database_file(const std::filesystem::path& database_file)
{
    m_impl->load_database_file(database_file);
}

bool magic::load_database_file(
    const std::nothrow_t&        tag,
    const std::filesystem::path& database_file
) noexcept
{
    return m_impl->load_database_file(tag, database_file);
}

void magic::open(flags_mask_t flags_mask)
{
    m_impl->open(flags_mask);
}

bool magic::open(flags_mask_t flags_mask, const std::nothrow_t& tag) noexcept
{
    return m_impl->open(flags_mask, tag);
}

void magic::open(const flags_container_t& flags_container)
{
    m_impl->open(flags_container);
}

bool magic::open(
    const flags_container_t& flags_container,
    const std::nothrow_t&    tag
) noexcept
{
    return m_impl->open(flags_container, tag);
}

void magic::set_flags(flags_mask_t flags_mask)
{
    m_impl->set_flags(flags_mask);
}

bool magic::set_flags(
    flags_mask_t          flags_mask,
    const std::nothrow_t& tag
) noexcept
{
    return m_impl->set_flags(flags_mask, tag);
}

void magic::set_flags(const flags_container_t& flags_container)
{
    m_impl->set_flags(flags_container);
}

bool magic::set_flags(
    const flags_container_t& flags_container,
    const std::nothrow_t&    tag
) noexcept
{
    return m_impl->set_flags(flags_container, tag);
}

void magic::set_parameter(parameters parameter, std::size_t value)
{
    m_impl->set_parameter(parameter, value);
}

bool magic::set_parameter(
    parameters            parameter,
    std::size_t           value,
    const std::nothrow_t& tag
) noexcept
{
    return m_impl->set_parameter(parameter, value, tag);
}

void magic::set_parameters(const parameter_value_map_t& parameters)
{
    m_impl->set_parameters(parameters);
}

bool magic::set_parameters(
    const parameter_value_map_t& parameters,
    const std::nothrow_t&        tag
) noexcept
{
    return m_impl->set_parameters(parameters, tag);
}

} /* namespace recognition */
