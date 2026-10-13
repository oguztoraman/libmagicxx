/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters.hpp
 * @brief Header file for the MagicParameters class.
 *
 * This file contains the MagicParameters class, a C++23 value type that
 * owns the libmagicxx tuning parameters: the `Parameters` enum and the
 * operations that convert parameters to human-readable strings. Every
 * operation is allocation-free and `noexcept`; the authoritative values
 * live in libmagic and are reached through Magic::GetParameter() and
 * Magic::SetParameter().
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 *
 * @see https://github.com/oguztoraman/libmagicxx
 * @see https://github.com/file/file (underlying libmagic)
 */

#ifndef MAGIC_PARAMETERS_HPP
#define MAGIC_PARAMETERS_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <optional>
#include <ranges>
#include <string_view>

#include "static_string.hpp"

/**
 * @namespace Recognition
 * @brief Root namespace for the libmagicxx library.
 *
 * @since 10.0.0
 */
namespace Recognition {
/**
 * @class MagicParameters
 * @ingroup magic_core
 *
 * @brief A modern C++23 value class rendering Magic tuning parameters.
 *
 * MagicParameters owns the `Parameters` enum and stores a snapshot of
 * parameter values in a bounded inline container, converting them to
 * human-readable strings. The authoritative values are kept by libmagic
 * itself and reached through Magic::GetParameter() and
 * Magic::SetParameter().
 *
 * ### Key Features
 *
 * - **Value Semantics**: Trivially copyable and movable parameter snapshot.
 * - **Exception Safety**: Every operation is `noexcept` and performs no
 *   heap allocation.
 * - **Explicit Conversions**: `ToString()` named member function instead of
 *   implicit conversion operators.
 * - **Comparison**: Snapshots are comparable with `operator==` and
 *   `operator<=>`.
 *
 * ### Usage Examples
 *
 * @code{.cpp}
 * #include <magic.hpp>
 * #include <print>
 *
 * using namespace Recognition;
 *
 * // Name of a single parameter
 * std::println("{}", MagicParameters{MagicParameters::Parameters::RegexMax,
 *     1UZ}
 *     .ToString(MagicParameters::StringFormat::Names)); // "RegexMax"
 *
 * // Snapshot of all current values of a Magic instance
 * Magic magic{Magic::FlagsT::Mime};
 * MagicParameters parameters{magic.GetParameters()};
 * std::println("{}", parameters.ToString());  // "IndirMax: 15, NameMax: 30,..."
 * @endcode
 *
 * @see Magic for the file type identification class using these parameters.
 *
 * @since 11.0.0
 */
class MagicParameters {
public:
    /**
     * @brief Parameters for tuning Magic behavior limits.
     * @ingroup magic_core
     *
     * The Parameters enum provides access to various internal limits that
     * control how deeply Magic analyzes files. Adjusting these can help
     * balance between thoroughness and performance.
     *
     * ### Common Parameter Usage
     *
     * @code{.cpp}
     * Magic magic{Magic::FlagsT::Mime};
     *
     * // Limit bytes scanned for faster performance
     * magic.SetParameter(Magic::ParametersT::BytesMax, 1024 * 1024);
     *
     * // Get current value
     * auto bytes = magic.GetParameter(Magic::ParametersT::BytesMax);
     *
     * // Get all parameters as "name: value" entries
     * MagicParameters parameters{magic.GetParameters()};
     * std::println("{}", parameters.ToString());
     * @endcode
     *
     * @see Magic::SetParameter() to modify a single parameter
     * @see Magic::SetParameters() to modify multiple parameters
     * @see Magic::GetParameter() to retrieve a single parameter value
     * @see Magic::GetParameters() to retrieve all parameter values
     *
     * @since 11.0.0
     */
    enum class Parameters : std::uint8_t {
        /* clang-format off */
        IndirMax     = 0UZ, /**< Maximum recursion depth for indirect magic (default: 15). */
        NameMax      = 1UZ, /**< Maximum use count for name/use magic entries (default: 30). */
        ElfPhnumMax  = 2UZ, /**< Maximum ELF program headers to process (default: 128). */
        ElfShnumMax  = 3UZ, /**< Maximum ELF section headers to process (default: 32768). */
        ElfNotesMax  = 4UZ, /**< Maximum ELF notes to process (default: 256). */
        RegexMax     = 5UZ, /**< Maximum regex search length in bytes (default: 8192). */
        BytesMax     = 6UZ, /**< Maximum bytes to read from file (default: 7340032 = 7MB). */
        EncodingMax  = 7UZ, /**< Maximum bytes to scan for encoding detection (default: 1048576 = 1MB). */
        ElfShsizeMax = 8UZ, /**< Maximum ELF section size to process (default: 134217728 = 128MB). */
        MagWarnMax   = 9UZ  /**< Maximum warnings to tolerate from magic file (default: 64). */
        /* clang-format on */
    };

    /**
     * @brief Format selector for MagicParameters::ToString().
     * @ingroup magic_core
     *
     * Chooses whether the string contains only parameter names or names
     * paired with their values.
     *
     * @code{.cpp}
     * MagicParameters parameters{map};
     *
     * // "RegexMax, BytesMax"
     * parameters.ToString(StringFormat::Names);
     *
     * // "RegexMax: 8192, BytesMax: 1048576"
     * parameters.ToString(StringFormat::NamesAndValues);
     * @endcode
     *
     * @see MagicParameters::ToString()
     *
     * @since 11.0.0
     */
    enum class StringFormat : std::uint8_t {
        Names,         /**< Render only the parameter names. */
        NamesAndValues /**< Render each name followed by its value. */
    };

    /**
     * @typedef ParameterValueMapT
     *
     * @brief Map from MagicParameters::Parameters to their corresponding
     *        values.
     *
     * @since 11.0.0
     */
    using ParameterValueMapT = std::map<Parameters, std::size_t>;

    /**
     * @typedef ParameterValueT
     *
     * @brief Key-value pair representing a single parameter and its value.
     *
     * @since 11.0.0
     */
    using ParameterValueT = ParameterValueMapT::value_type;

    /**
     * @brief Default constructor. Stores no parameter values.
     *
     * @code{.cpp}
     * MagicParameters parameters;  // No values stored
     * @endcode
     *
     * @since 11.0.0
     */
    MagicParameters() noexcept = default;

    /**
     * @brief Construct from a single parameter and its value.
     *
     * The resulting snapshot holds exactly the given parameter-value entry.
     * A parameter's name alone renders with `StringFormat::Names`, which
     * ignores the value. Use Magic::GetParameter() to read the parameter's
     * actual libmagic value.
     *
     * @note A `parameter` that is not a valid ordinal is silently
     *       discarded, leaving an empty snapshot.
     *
     * @param[in] parameter The parameter to store.
     * @param[in] value     The value for the parameter.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     MagicParameters::Parameters::BytesMax, 1048576U
     * };
     * std::println("{}", parameters.ToString());  // "BytesMax: 1048576"
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicParameters(Parameters parameter, std::size_t value) noexcept
    {
        Store(parameter, value);
    }

    /**
     * @brief Construct from a map of parameter values.
     *
     * The resulting snapshot holds exactly the entries of the given map.
     *
     * @note Map entries whose key is not a valid ordinal are silently
     *       discarded; a map of only invalid keys leaves an empty
     *       snapshot.
     *
     * @param[in] parameter_value_map Map from Parameters to their values.
     *
     * @code{.cpp}
     * Magic magic{Magic::FlagsT::Mime};
     * MagicParameters parameters{magic.GetParameters()};
     * @endcode
     *
     * @since 11.0.0
     */
    explicit MagicParameters(
        const ParameterValueMapT& parameter_value_map
    ) noexcept
    {
        for (const auto& [parameter, value] : parameter_value_map) {
            Store(parameter, value);
        }
    }

    /**
     * @brief Copy constructor. Defaulted; the inline storage is trivially
     *        copyable.
     *
     * @param[in] other The MagicParameters instance to copy.
     *
     * @since 11.0.0
     */
    MagicParameters(const MagicParameters& other) noexcept = default;

    /**
     * @brief Move constructor.
     *
     * @param[in,out] other The MagicParameters instance to move from.
     *
     * @since 11.0.0
     */
    MagicParameters(MagicParameters&& other) noexcept = default;

    /**
     * @brief Destructor.
     *
     * @since 11.0.0
     */
    ~MagicParameters() = default;

    /**
     * @brief Copy assignment operator.
     *
     * @param[in] other The MagicParameters instance to copy.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    MagicParameters& operator=(const MagicParameters& other) noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * @param[in,out] other The MagicParameters instance to move from.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    MagicParameters& operator=(MagicParameters&& other) noexcept = default;

    /**
     * @brief Compare two parameter snapshots for equality.
     *
     * @param[in] lhs Left-hand side snapshot.
     * @param[in] rhs Right-hand side snapshot.
     *
     * @returns `true` if both snapshots store identical entries.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr bool operator==(
        const MagicParameters& lhs,
        const MagicParameters& rhs
    ) noexcept
    {
        return lhs.m_values == rhs.m_values;
    }

    /**
     * @brief Compare two parameter snapshots entry by entry.
     *
     * @param[in] lhs Left-hand side snapshot.
     * @param[in] rhs Right-hand side snapshot.
     *
     * @returns A `std::strong_ordering` result comparing the stored entries
     *          in parameter ordinal order.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(
        const MagicParameters& lhs,
        const MagicParameters& rhs
    ) noexcept
    {
        return lhs.m_values <=> rhs.m_values;
    }

    /**
     * @brief Default inline capacity of the string returned by ToString().
     *
     * The smallest capacity that fits every parameter with its maximum
     * value and the default separators.
     *
     * @since 11.0.0
     */
    static constexpr std::size_t TO_STRING_CAPACITY{337UZ};

    /**
     * @brief Convert the stored parameter values to a string.
     *
     * The result is a fixed-capacity Utility::StaticString, so rendering
     * never allocates and never throws; text beyond `CapacityV` characters
     * is truncated (see StaticString::IsTruncated()).
     *
     * @tparam CapacityV Inline capacity, in characters, of the returned
     *                   StaticString (default: `TO_STRING_CAPACITY`).
     *
     * @param[in] format              Whether to render only the parameter
     *                                names or the names with their values
     *                                (default: `StringFormat::NamesAndValues`).
     * @param[in] value_separator     String separating each name and value
     *                                (default: `: `). Used only with
     *                                `StringFormat::NamesAndValues`.
     * @param[in] parameter_separator String separating the entries
     *                                (default: `, `).
     *
     * @returns The stored entries in parameter order — e.g.
     *          "RegexMax: 8192, BytesMax: 1048576" with
     *          `StringFormat::NamesAndValues`, or "RegexMax, BytesMax" with
     *          `StringFormat::Names`. Empty when no value is stored.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     {{MagicParameters::Parameters::BytesMax, 1048576U},
     *      {MagicParameters::Parameters::RegexMax, 8192U}}
     * };
     * auto text = parameters.ToString();  // "RegexMax: 8192, BytesMax: 1048576"
     * auto names = parameters.ToString(
     *     MagicParameters::StringFormat::Names);  // "RegexMax, BytesMax"
     * @endcode
     *
     * @since 11.0.0
     */
    template <std::size_t CapacityV = TO_STRING_CAPACITY>
    [[nodiscard]] Utility::StaticString<CapacityV> ToString(
        StringFormat format = StringFormat::NamesAndValues,
        /* NOLINTNEXTLINE(bugprone-easily-swappable-parameters) */
        std::string_view value_separator     = ": ",
        std::string_view parameter_separator = ", "
    ) const noexcept
    {
        using Utility::StaticString;
        StaticString<CapacityV> result{};
        for (
            const auto& [value, name] :
            std::views::zip(m_values, TO_STRING_NAMES)
        ) {
            if (value.has_value()) {
                if (!result.Empty()) {
                    result.Append(parameter_separator);
                }
                result.Append(name);
                if (format == StringFormat::NamesAndValues) {
                    result.Append(value_separator);
                    result.Append(
                        std::string_view{
                            StaticString<CapacityV>::Format("{}", *value)
                        }
                    );
                }
            }
        }
        return result;
    }

private:
    /**
     * @brief Number of Parameters ordinals, and capacity of the inline
     *        value container.
     */
    static constexpr std::size_t PARAMETER_COUNT{
        static_cast<std::size_t>(Parameters::MagWarnMax) + 1UZ
    };

    /**
     * @brief Human-readable name of each Parameters ordinal.
     *
     * Indexed by ordinal; consumed by ToString().
     */
    static constexpr std::array<std::string_view, PARAMETER_COUNT>
        TO_STRING_NAMES{
            "IndirMax",
            "NameMax",
            "ElfPhnumMax",
            "ElfShnumMax",
            "ElfNotesMax",
            "RegexMax",
            "BytesMax",
            "EncodingMax",
            "ElfShsizeMax",
            "MagWarnMax"
        };

    static_assert(
        TO_STRING_NAMES.size() == PARAMETER_COUNT,
        "TO_STRING_NAMES must cover every Parameters ordinal"
    );

    static_assert(
        []() noexcept {
            constexpr std::size_t MAX_DIGITS{
                std::numeric_limits<std::size_t>::digits10 + 1
            };
            std::size_t required{};
            for (const auto name : TO_STRING_NAMES) {
                required += name.size()
                          + 2UZ /* ": " */
                          + MAX_DIGITS
                          + 2UZ /* ", " */;
            }
            return required - 2UZ; /* no trailing separator */
        }() <= TO_STRING_CAPACITY,
        "TO_STRING_CAPACITY must fit every parameter with the default "
        "separators"
    );

    static_assert(
        std::ranges::all_of(
            TO_STRING_NAMES,
            [](const std::string_view name) {
                return !name.empty();
            }
        ),
        "TO_STRING_NAMES must carry a non-empty name for every ordinal"
    );

    /**
     * @brief Convert a parameter to its libmagic constant.
     *
     * @param[in] parameter The parameter to convert.
     *
     * @returns Libmagic-compatible `MAGIC_PARAM_*` constant, or
     *          `std::nullopt` if `parameter` is not a valid ordinal.
     */
    [[nodiscard]] static std::optional<int> ToUnderlying(
        Parameters parameter
    ) noexcept;

    /**
     * @brief Store a parameter value in the inline container.
     *
     * @param[in] parameter The parameter to store.
     * @param[in] value     The value for the parameter.
     */
    constexpr void Store(Parameters parameter, std::size_t value) noexcept
    {
        std::size_t ordinal{};
        for (auto& slot : m_values) {
            if (parameter == static_cast<Parameters>(ordinal)) {
                slot = value;
            }
            ++ordinal;
        }
    }

    friend class Magic; /**< Grants Magic access to the libmagic bridge. */

    std::array<std::optional<std::size_t>, PARAMETER_COUNT>
        m_values{}; /**< Stored value of each parameter, empty when absent. */
};
} /* namespace Recognition */

#endif /* MAGIC_PARAMETERS_HPP */
