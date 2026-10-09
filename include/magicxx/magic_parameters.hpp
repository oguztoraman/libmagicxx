/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters.hpp
 * @brief Header file for the MagicParameters class.
 *
 * This file contains the MagicParameters class, a C++23 value type that
 * owns the libmagicxx tuning parameters: the `Parameters` enum and the
 * operations that convert parameters to human-readable strings. Values
 * are snapshotted into a bounded inline container, so the class is
 * allocation-free and `noexcept` except for its string conversions;
 * the authoritative values live in libmagic and are reached through
 * Magic::GetParameter() and Magic::SetParameter().
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 *
 * @see https://github.com/oguztoraman/libmagicxx
 * @see https://github.com/file/file (underlying libmagic)
 */

#ifndef MAGIC_PARAMETERS_HPP
#define MAGIC_PARAMETERS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>

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
 * - **Exception Safety**: Every operation except `ToString()` is
 *   `noexcept`; the inline storage performs no heap allocation.
 * - **Explicit Conversions**: `ToString()` named member function instead of
 *   implicit conversion operators.
 *
 * ### Usage Examples
 *
 * @code{.cpp}
 * #include <magic_parameters.hpp>
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
     * // "RegexMax,BytesMax"
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
    MagicParameters(Parameters parameter, std::size_t value) noexcept
    {
        Store(parameter, value);
    }

    /**
     * @brief Construct from a map of parameter values.
     *
     * The resulting snapshot holds exactly the entries of the given map.
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
     * @brief Convert the stored parameter values to a string.
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
     * @returns Formatted string of the stored entries — e.g.
     *          "RegexMax: 8192, BytesMax: 1048576" with the default
     *          separators and `StringFormat::NamesAndValues`, or
     *          "RegexMax,BytesMax" with `StringFormat::Names`; entries render
     *          in Parameters ordinal order and the result is empty when no
     *          value is stored. A single "ParameterName: value" entry is
     *          rendered by constructing an instance from that parameter and
     *          value.
     *
     * @throws std::bad_alloc If allocating the result string fails.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     {{MagicParameters::Parameters::BytesMax, 1048576U},
     *      {MagicParameters::Parameters::RegexMax, 8192U}}
     * };
     * auto text = parameters.ToString();
     * // text == "RegexMax: 8192, BytesMax: 1048576"
     * auto names = parameters.ToString(
     *     MagicParameters::StringFormat::Names);
     * // names == "RegexMax,BytesMax"
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] std::string ToString(
        StringFormat     format              = StringFormat::NamesAndValues,
        std::string_view value_separator     = ": ",
        std::string_view parameter_separator = ", "
    ) const;

private:
    /**
     * @brief Convert a parameter to its libmagic constant.
     *
     * @param[in] parameter The parameter to convert.
     *
     * @returns Libmagic-compatible `MAGIC_PARAM_*` constant.
     */
    [[nodiscard]] static int ToUnderlying(Parameters parameter) noexcept;

    /**
     * @brief Store a parameter value in the inline container.
     *
     * @param[in] parameter The parameter to store.
     * @param[in] value     The value for the parameter.
     */
    void Store(Parameters parameter, std::size_t value) noexcept
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

    /**
     * @brief Capacity of the bounded inline container.
     *
     * Equals the count of dense `Parameters` enum ordinals, from `IndirMax`
     * (0) to `MagWarnMax` (9).
     */
    static constexpr std::size_t PARAMETER_COUNT{
        static_cast<std::size_t>(Parameters::MagWarnMax) + 1UZ
    };

    std::array<std::optional<std::size_t>, PARAMETER_COUNT>
        m_values{}; /**< Stored value of each parameter, empty when absent. */
};
} /* namespace Recognition */

#endif /* MAGIC_PARAMETERS_HPP */
