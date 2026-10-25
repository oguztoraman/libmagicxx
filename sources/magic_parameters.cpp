/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters.cpp
 * @brief Implementation of the MagicParameters class.
 *
 * This file contains the complete implementation of the MagicParameters
 * value class, including:
 * - The file-local libmagic parameters constant table
 *   (LIBMAGIC_PARAMETER_VALUES) and the ParameterToUnderlying() helper
 * - MagicParameters public member function implementations
 *
 * @section magic_parameters_cpp_architecture Architecture
 *
 * MagicParameters stores its snapshot in a bounded inline container (a
 * fixed-size array of optionals indexed by the dense `Parameters`
 * ordinals), which keeps every public operation allocation-free,
 * `constexpr`, and `noexcept` — a heap map could not. The authoritative
 * values stay in libmagic, reached through Magic::GetParameter(). The
 * functions that do not reference libmagic constants (the constructors,
 * `Store()`, and `ToString()`) are defined inline in the header; this
 * file holds the libmagic constant table and the bridge that needs it,
 * kept in an anonymous namespace so libmagic stays out of the public
 * header.
 *
 * The underlying libmagic C library is wrapped in the `Detail` namespace to
 * isolate its symbols; the `MAGIC_PARAM_*` constants are referenced only here.
 *
 * @section magic_parameters_cpp_components Key Components
 *
 * | Component | Purpose |
 * |-----------|---------|
 * | `Detail` (magic.h) | Wrapped libmagic C constants |
 * | `LIBMAGIC_PARAMETER_VALUES` | Ordinal to `MAGIC_PARAM_*` constant |
 * | `ParameterToUnderlying()` | `Parameters` to `MAGIC_PARAM_*` (nullopt when invalid) |
 *
 * @see magic_parameters.hpp for the public API documentation
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 */

#include "magic_parameters.hpp"

#include <array>
#include <cstddef>
#include <optional>
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
 * Type alias for the parameters constant lookup table.
 */
/** @{ */
using LibmagicValueT = int; /**< libmagic integer type */
/** @} parameters_impl_type_aliases */

/**
 * @brief Count of `Parameters` enum ordinals.
 */
constexpr std::size_t LIBMAGIC_PARAMETER_COUNT{
    static_cast<std::size_t>(MagicParameters::Parameters::MagWarnMax) + 1UZ
};

using LibmagicParameterValuesT = std::array<
    LibmagicValueT,
    LIBMAGIC_PARAMETER_COUNT
>; /**< Array type holding all libmagic parameter constants in order. */

/**
 * @brief Mapping from MagicParameters::Parameters ordinals to libmagic
 *        constants.
 *
 * Static lookup table mapping each Parameters enum ordinal to the
 * corresponding libmagic `MAGIC_PARAM_*` constant. The human-readable
 * names live in the header's `MagicParameters::TO_STRING_NAMES` table,
 * which shares this order.
 *
 * @note The Parameters enum values are dense ordinals (0 through
 *       `LIBMAGIC_PARAMETER_COUNT - 1`), so the table index equals the
 *       `std::to_underlying(parameter)` result.
 *
 * @see MagicParameters::Parameters
 * @see MagicParameters::ToString()
 * @see ParameterToUnderlying()
 */
constexpr LibmagicParameterValuesT LIBMAGIC_PARAMETER_VALUES{
    MAGIC_PARAM_INDIR_MAX,
    MAGIC_PARAM_NAME_MAX,
    MAGIC_PARAM_ELF_PHNUM_MAX,
    MAGIC_PARAM_ELF_SHNUM_MAX,
    MAGIC_PARAM_ELF_NOTES_MAX,
    MAGIC_PARAM_REGEX_MAX,
    MAGIC_PARAM_BYTES_MAX,
    MAGIC_PARAM_ENCODING_MAX,
    MAGIC_PARAM_ELF_SHSIZE_MAX,
    MAGIC_PARAM_MAGWARN_MAX
};

/**
 * @brief Convert a parameter to its libmagic constant.
 *
 * Looks up the `MAGIC_PARAM_*` constant corresponding to the given
 * `Parameters` ordinal for use with `magic_getparam()` and
 * `magic_setparam()`. An out-of-range ordinal is reported as `std::nullopt`.
 *
 * @param[in] parameter The parameter to convert.
 *
 * @returns Libmagic-compatible `MAGIC_PARAM_*` constant, or `std::nullopt`
 *          if `parameter` is not a valid ordinal.
 */
[[nodiscard]] constexpr std::optional<LibmagicValueT> ParameterToUnderlying(
    MagicParameters::Parameters parameter
) noexcept
{
    const auto ordinal{static_cast<std::size_t>(std::to_underlying(parameter))};
    if (ordinal >= LIBMAGIC_PARAMETER_COUNT) {
        return std::nullopt;
    }
    std::size_t index{};
    for (const auto value : LIBMAGIC_PARAMETER_VALUES) {
        if (index == ordinal) {
            return value;
        }
        ++index;
    }
    return std::nullopt;
}
} /* anonymous namespace */

std::optional<int> MagicParameters::ToUnderlying(Parameters parameter) noexcept
{
    return ParameterToUnderlying(parameter);
}
} /* namespace Recognition */
