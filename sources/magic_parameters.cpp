/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters.cpp
 * @brief Implementation of the MagicParameters class.
 *
 * This file contains the complete implementation of the MagicParameters
 * value class, including:
 * - The file-local libmagic parameters lookup table (LIBMAGIC_PARAMETERS)
 *   and its helpers
 * - MagicParameters public member function implementations
 *
 * @section magic_parameters_cpp_architecture Architecture
 *
 * MagicParameters stores its snapshot in a bounded inline container (a
 * fixed-size array of optionals indexed by the dense `Parameters`
 * ordinals), so every operation except `ToString()` performs no
 * heap allocation and is `noexcept`. A heap map could not provide this:
 * inserting into or copying a `std::map`/`std::flat_map` allocates and may
 * throw. The authoritative values stay in libmagic, reached through
 * Magic::GetParameter(). The functions that do not reference libmagic
 * constants (the constructors and `Store()`) are defined inline in the
 * header; this file holds only the definitions that need the lookup table.
 * The lookup table lives in an anonymous namespace in this translation
 * unit, keeping libmagic entirely out of the public header.
 *
 * The underlying libmagic C library is wrapped in the `Detail` namespace to
 * isolate its symbols; the `MAGIC_PARAM_*` constants are referenced only here.
 *
 * @section magic_parameters_cpp_components Key Components
 *
 * | Component | Purpose |
 * |-----------|---------|
 * | `Detail` (magic.h) | Wrapped libmagic C constants |
 * | `LIBMAGIC_PARAMETERS` | Ordinal to `MAGIC_PARAM_*` constant and name |
 * | `ParameterToUnderlying()` | `Parameters` to `MAGIC_PARAM_*` constant |
 * | `ParameterName()` | `Parameters` to name (ordinal index) |
 * | `PairValue()` | Extracts the integer from a value-name pair |
 * | `PairName()` | Extracts the name from a value-name pair |
 *
 * @see magic_parameters.hpp for the public API documentation
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 */

#include "magic_parameters.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace Recognition {
/**
 * @namespace Recognition::Detail
 * @ingroup magic_implementation
 * @brief Internal namespace for libmagic C library integration.
 *
 * This namespace isolates the libmagic C header to prevent symbol
 * pollution in client code.
 */
namespace Detail {
#include <magic.h>
} /* namespace Detail */

namespace {
/**
 * @defgroup parameters_impl_type_aliases Libmagic Type Aliases
 * @ingroup magic_implementation
 * @brief Type definitions for libmagic interoperability.
 *
 * Type aliases for parameter value-name pairs and the parameters lookup
 * table.
 */
/** @{ */
using LibmagicValueT     = int;              /**< libmagic integer type */
using LibmagicValueNameT = std::string_view; /**< Parameter name type. */
using LibmagicPairT      = std::pair<
    LibmagicValueT,
    LibmagicValueNameT
>; /**< Value-name pair */
/** @} parameters_impl_type_aliases */

/**
 * @brief Number of parameters in the lookup table.
 *
 * Equals the count of `Parameters` enum ordinals, from `IndirMax` (0) to
 * `MagWarnMax` (9).
 */
constexpr std::size_t LIBMAGIC_PARAMETER_COUNT{
    static_cast<std::size_t>(MagicParameters::Parameters::MagWarnMax) + 1UZ
};

using LibmagicParametersT = std::array<
    LibmagicPairT,
    LIBMAGIC_PARAMETER_COUNT
>; /**< Array type mapping all libmagic parameter values to names. */

/**
 * @brief Mapping from MagicParameters::Parameters ordinals to libmagic
 *        constants.
 *
 * Static lookup table mapping each Parameters enum ordinal to the
 * corresponding libmagic `MAGIC_PARAM_*` constant and its human-readable
 * name.
 *
 * @note The Parameters enum values are dense ordinals (0 through
 *       `LIBMAGIC_PARAMETER_COUNT - 1`), so the table index equals the
 *       `std::to_underlying(parameter)` result.
 *
 * @see MagicParameters::Parameters
 * @see ParameterToUnderlying()
 * @see ParameterName()
 */
constexpr LibmagicParametersT LIBMAGIC_PARAMETERS{
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

static_assert(
    std::ranges::all_of(
        LIBMAGIC_PARAMETERS,
        [](const LibmagicPairT& pair) {
            return !pair.second.empty();
        }
    ),
    "LIBMAGIC_PARAMETERS must carry a non-empty name for every ordinal"
);

/**
 * @brief Extract the libmagic integer constant from a value-name pair.
 *
 * @param[in] pair The libmagic value-name pair.
 *
 * @returns The libmagic constant (e.g. `MAGIC_PARAM_BYTES_MAX`).
 */
[[nodiscard]] constexpr LibmagicValueT PairValue(
    const LibmagicPairT& pair
) noexcept
{
    return std::get<LibmagicValueT>(pair);
}

/**
 * @brief Extract the human-readable name from a value-name pair.
 *
 * @param[in] pair The libmagic value-name pair.
 *
 * @returns The parameter name (e.g. "BytesMax") as a view referencing
 *          the static table literal; valid for the program lifetime.
 */
[[nodiscard]] constexpr LibmagicValueNameT PairName(
    const LibmagicPairT& pair
) noexcept
{
    return std::get<LibmagicValueNameT>(pair);
}

/**
 * @brief Convert a parameter to its libmagic constant.
 *
 * Looks up the `MAGIC_PARAM_*` constant corresponding to the given
 * `Parameters` ordinal for use with `magic_getparam()` and
 * `magic_setparam()`. The fallback keeps the function total and
 * non-throwing for an out-of-range enum value.
 *
 * @param[in] parameter The parameter to convert.
 *
 * @returns Libmagic-compatible `MAGIC_PARAM_*` constant.
 */
[[nodiscard]] constexpr LibmagicValueT ParameterToUnderlying(
    MagicParameters::Parameters parameter
) noexcept
{
    std::size_t ordinal{};
    for (const auto& pair : LIBMAGIC_PARAMETERS) {
        if (parameter == static_cast<MagicParameters::Parameters>(ordinal)) {
            return PairValue(pair);
        }
        ++ordinal;
    }
    return PairValue(LIBMAGIC_PARAMETERS.front());
}

/**
 * @brief Look up the human-readable name of a single parameter.
 *
 * Internal helper used by MagicParameters::ToString() to render
 * each entry's name. Uses direct ordinal indexing into
 * LIBMAGIC_PARAMETERS; the table order
 * matches the dense Parameters enum ordinals (asserted above).
 *
 * @param[in] parameter The parameter to convert.
 *
 * @returns The parameter name (e.g. "BytesMax") as a view referencing
 *          the static table literal; valid for the program lifetime.
 *
 * @throws std::out_of_range If the ordinal exceeds the table bounds
 *         (impossible for valid Parameters enumerators; asserted above).
 */
[[nodiscard]] LibmagicValueNameT ParameterName(
    MagicParameters::Parameters parameter
)
{
    return PairName(LIBMAGIC_PARAMETERS.at(
        static_cast<std::size_t>(std::to_underlying(parameter))
    ));
}
} /* anonymous namespace */

std::string MagicParameters::ToString(
    /* NOLINTNEXTLINE(bugprone-easily-swappable-parameters) */
    const char         value_separator,
    const char         parameter_separator,
    const StringFormat format
) const
{
    std::string result;
    std::size_t ordinal{};
    for (const auto& value : m_values) {
        if (value.has_value()) {
            if (!result.empty()) {
                result += parameter_separator;
            }
            const auto parameter{static_cast<Parameters>(ordinal)};
            if (format == StringFormat::Names) {
                result += ParameterName(parameter);
            } else {
                result += std::format(
                    "{}{}{}",
                    ParameterName(parameter),
                    std::string(1, value_separator),
                    value.value()
                );
            }
        }
        ++ordinal;
    }
    return result;
}

int MagicParameters::ToUnderlying(Parameters parameter) noexcept
{
    return ParameterToUnderlying(parameter);
}
} /* namespace Recognition */
