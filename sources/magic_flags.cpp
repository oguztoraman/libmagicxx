/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags.cpp
 * @brief Implementation of the MagicFlags class.
 *
 * This file contains the complete implementation of the MagicFlags value
 * class, including:
 * - The file-local libmagic flags constant table (LIBMAGIC_FLAG_VALUES)
 *   and the MaskToUnderlying() helper
 * - MagicFlags public member function implementations
 *
 * @section magic_flags_cpp_architecture Architecture
 *
 * MagicFlags stores its flag set as a raw 32-bit bitmask member. The
 * functions that do not reference libmagic constants (the constructors,
 * `operator|`, `operator&`, `Contains()`, `Count()`, `Erase()`, the
 * compound assignments, iteration, and `ToString()`) are defined inline in
 * the header; this file holds only the libmagic constant table and the
 * bridge that needs it, kept in an anonymous namespace so libmagic stays
 * out of the public header and the class layout remains ABI-stable.
 *
 * The underlying libmagic C library is wrapped in the `Detail` namespace to
 * isolate its symbols; the `MAGIC_*` constants are referenced only here.
 *
 * @section magic_flags_cpp_components Key Components
 *
 * | Component | Purpose |
 * |-----------|---------|
 * | `Detail` (magic.h) | Wrapped libmagic C constants |
 * | `LIBMAGIC_FLAG_VALUES` | Bit position to `MAGIC_*` constant |
 * | `MaskToUnderlying()` | Bitmask to `MAGIC_*` OR-value (nullopt for unknown bits) |
 *
 * @see magic_flags.hpp for the public API documentation
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 */

#include "magic_flags.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
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
 * @defgroup flags_impl_type_aliases Libmagic Type Aliases
 * @ingroup magic_implementation
 * @brief Type definitions for libmagic interoperability.
 *
 * Type alias for the flags constant lookup table.
 */
/** @{ */
using LibmagicValueT = int; /**< libmagic integer type */
/** @} flags_impl_type_aliases */

/**
 * @brief Count of bit positions covered by the Flags enum.
 */
constexpr std::size_t LIBMAGIC_FLAGS_COUNT{
    std::bit_width(std::to_underlying(MagicFlags::Flags::NoCheckBuiltin))
};

using LibmagicFlagValuesT = std::array<
    LibmagicValueT,
    LIBMAGIC_FLAGS_COUNT
>; /**< Array type holding all libmagic flag constants in bit order. */

/**
 * @brief Mapping from MagicFlags::Flags bit positions to libmagic
 *        constants.
 *
 * Static lookup table mapping each Flags enum bit position to the
 * corresponding libmagic `MAGIC_*` constant. The human-readable names
 * live in the header's `MagicFlags::TO_STRING_NAMES` table, which shares
 * this order.
 *
 * @note The table values are not required to be single-bit constants: some
 *       libmagic flags (`MAGIC_MIME`, `MAGIC_NODESC`,
 *       `MAGIC_NO_CHECK_BUILTIN`) are defined as composites of other flags.
 *       The bit positions of the Flags enum — not the table values — define
 *       the mask layout.
 *
 * @see MagicFlags::Flags
 * @see MagicFlags::ToString()
 * @see MaskToUnderlying()
 */
constexpr LibmagicFlagValuesT LIBMAGIC_FLAG_VALUES{
    MAGIC_DEBUG,
    MAGIC_SYMLINK,
    MAGIC_COMPRESS,
    MAGIC_DEVICES,
    MAGIC_MIME_TYPE,
    MAGIC_CONTINUE,
    MAGIC_CHECK,
    MAGIC_PRESERVE_ATIME,
    MAGIC_RAW,
    MAGIC_ERROR,
    MAGIC_MIME_ENCODING,
    MAGIC_MIME,
    MAGIC_APPLE,
    MAGIC_EXTENSION,
    MAGIC_COMPRESS_TRANSP,
    MAGIC_NO_COMPRESS_FORK,
    MAGIC_NODESC,
    MAGIC_NO_CHECK_COMPRESS,
    MAGIC_NO_CHECK_TAR,
    MAGIC_NO_CHECK_SOFT,
    MAGIC_NO_CHECK_APPTYPE,
    MAGIC_NO_CHECK_ELF,
    MAGIC_NO_CHECK_TEXT,
    MAGIC_NO_CHECK_CDF,
    MAGIC_NO_CHECK_CSV,
    MAGIC_NO_CHECK_TOKENS,
    MAGIC_NO_CHECK_ENCODING,
    MAGIC_NO_CHECK_JSON,
    MAGIC_NO_CHECK_SIMH,
    MAGIC_NO_CHECK_BUILTIN
};

/**
 * @brief Verify that the Flags enum bit positions are dense (0 through
 *        `LIBMAGIC_FLAG_VALUES.size() - 1`), so table index `i`
 *        corresponds to enum value `1U << i`.
 *
 * Composite libmagic constants (`MAGIC_MIME`, `MAGIC_NODESC`,
 * `MAGIC_NO_CHECK_BUILTIN`) intentionally do NOT equal `1 << i`; the
 * mask layout is defined by the enum bit positions, so the ordering
 * invariant is asserted on the enum instead of the table values.
 */
static_assert(
    std::to_underlying(MagicFlags::Flags::NoCheckBuiltin)
        == (1U << (LIBMAGIC_FLAG_VALUES.size() - 1UZ)),
    "Flags enum bit positions must be dense for the table order to hold"
);

/**
 * @brief Convert a raw bitmask to libmagic flag constants.
 *
 * ORs together the `MAGIC_*` constants corresponding to every set bit for
 * use with `magic_open()` and `magic_setflags()`. Bit positions outside
 * `[0, LIBMAGIC_FLAGS_COUNT)` are reported as `std::nullopt` so the caller
 * can reject the mask.
 *
 * @param[in] mask Raw bitmask with the flag bits set.
 *
 * @returns Libmagic-compatible integer flags value, or `std::nullopt` if
 *          `mask` carries a bit position that no `Flags` enumerator covers.
 */
[[nodiscard]] constexpr std::optional<int> MaskToUnderlying(
    std::uint32_t mask
) noexcept
{
    if ((mask >> LIBMAGIC_FLAGS_COUNT) != 0U) {
        return std::nullopt;
    }
    auto        flags = static_cast<LibmagicValueT>(MAGIC_NONE);
    std::size_t bit{};
    for (const auto value : LIBMAGIC_FLAG_VALUES) {
        if (((mask >> bit) & 1U) != 0U) {
            flags |= value;
        }
        ++bit;
    }
    return flags;
}
} /* anonymous namespace */

std::optional<int> MagicFlags::ToUnderlying() const noexcept
{
    return MaskToUnderlying(m_mask);
}
} /* namespace Recognition */
