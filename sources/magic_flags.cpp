/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags.cpp
 * @brief Implementation of the MagicFlags class.
 *
 * This file contains the complete implementation of the MagicFlags value
 * class, including:
 * - The file-local libmagic flags lookup table (LIBMAGIC_FLAGS,
 *   LIBMAGIC_FLAG_NONE) and its helpers
 * - MagicFlags public member function implementations
 *
 * @section magic_flags_cpp_architecture Architecture
 *
 * MagicFlags stores its flag set as a raw 32-bit bitmask member, so every
 * operation (construction, assignment, combination, and the libmagic bridge)
 * is allocation-free and `noexcept`; only the `ToContainer()` and
 * `ToString()` conversions allocate. The functions that do not reference
 * libmagic constants (the constructors, `operator|`, and `ToContainer()`)
 * are defined inline in the header; this file holds only the definitions
 * that need the lookup table. The lookup table lives in an anonymous
 * namespace in this translation unit, keeping it hidden from the public
 * header while the class layout itself (a single `std::uint32_t`) remains
 * ABI-stable behind the shared library boundary.
 *
 * The underlying libmagic C library is wrapped in the `Detail` namespace to
 * isolate its symbols; the `MAGIC_*` constants are referenced only here.
 *
 * @section magic_flags_cpp_components Key Components
 *
 * | Component | Purpose |
 * |-----------|---------|
 * | `Detail` (magic.h) | Wrapped libmagic C constants |
 * | `LIBMAGIC_FLAGS` | Bit position to `MAGIC_*` constant and name |
 * | `MaskToUnderlying()` | Bitmask to `MAGIC_*` OR-value |
 * | `PairValue()` | Extracts the integer from a value-name pair |
 * | `PairName()` | Extracts the name from a value-name pair |
 * | `FlagName()` | Flag enum value to name (countr_zero bit index) |
 *
 * @see magic_flags.hpp for the public API documentation
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 */

#include "magic_flags.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include "utility.hpp"

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
 * Type aliases for flag value-name pairs and the flags lookup table.
 */
/** @{ */
using LibmagicValueT     = int; /**< libmagic integer type */
using LibmagicValueNameT = std::
    string_view; /**< Flag name type (static-storage view) */
using LibmagicPairT = std::pair<
    LibmagicValueT,
    LibmagicValueNameT
>; /**< Value-name pair */
/** @} flags_impl_type_aliases */

/**
 * @brief Number of flags in the lookup table.
 *
 * Equals the count of bit positions covered by the Flags enum, from
 * `Debug` (bit 0) to `NoCheckBuiltin` (bit 29).
 */
constexpr std::size_t LIBMAGIC_FLAGS_COUNT{
    std::bit_width(std::to_underlying(MagicFlags::Flags::NoCheckBuiltin))
};

using LibmagicFlagsT = std::array<
    LibmagicPairT,
    LIBMAGIC_FLAGS_COUNT
>; /**< Array type mapping all libmagic flag values to names. */

/**
 * @brief The MAGIC_NONE flag pair for default output.
 *
 * @see MagicFlags::Flags::None
 */
constexpr LibmagicPairT LIBMAGIC_FLAG_NONE{std::make_pair(MAGIC_NONE, "None")};

/**
 * @brief Mapping from MagicFlags::Flags bit positions to libmagic
 *        constants.
 *
 * Static lookup table mapping each Flags enum bit position to
 * the corresponding libmagic `MAGIC_*` constant and its
 * human-readable name.
 *
 * @note The table values are not required to be single-bit constants: some
 *       libmagic flags (`MAGIC_MIME`, `MAGIC_NODESC`,
 *       `MAGIC_NO_CHECK_BUILTIN`) are defined as composites of other flags.
 *       The bit positions of the Flags enum — not the table values — define
 *       the mask layout.
 *
 * @see MagicFlags::Flags
 * @see MaskToUnderlying()
 * @see FlagName()
 */
constexpr LibmagicFlagsT LIBMAGIC_FLAGS{
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

static_assert(
    std::ranges::all_of(
        LIBMAGIC_FLAGS,
        [](const LibmagicPairT& pair) {
            return !pair.second.empty();
        }
    ),
    "LIBMAGIC_FLAGS must carry a non-empty name for every bit position"
);

/**
 * @brief Verify that the Flags enum bit positions are dense (0 through
 *        `LIBMAGIC_FLAGS.size() - 1`), so table index `i` corresponds to
 *        enum value `1U << i`.
 *
 * Composite libmagic constants (`MAGIC_MIME`, `MAGIC_NODESC`,
 * `MAGIC_NO_CHECK_BUILTIN`) intentionally do NOT equal `1 << i`; the
 * mask layout is defined by the enum bit positions, so the ordering
 * invariant is asserted on the enum instead of the table values.
 */
static_assert(
    std::to_underlying(MagicFlags::Flags::NoCheckBuiltin)
        == (1U << (LIBMAGIC_FLAGS.size() - 1UZ)),
    "Flags enum bit positions must be dense for the table order to hold"
);

/**
 * @brief Extract the libmagic integer constant from a value-name pair.
 *
 * @param[in] pair The libmagic value-name pair.
 *
 * @returns The libmagic constant (e.g. `MAGIC_MIME`).
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
 * @returns The flag name (e.g. "Mime") as a view referencing the static
 *          table literal; valid for the program lifetime.
 */
[[nodiscard]] constexpr LibmagicValueNameT PairName(
    const LibmagicPairT& pair
) noexcept
{
    return std::get<LibmagicValueNameT>(pair);
}

/**
 * @brief Convert a raw bitmask to libmagic flag constants.
 *
 * ORs together the `MAGIC_*` constants corresponding to every set bit for
 * use with `magic_open()` and `magic_setflags()`.
 *
 * @param[in] mask Raw bitmask with the flag bits set.
 *
 * @returns Libmagic-compatible integer flags value.
 */
[[nodiscard]] int MaskToUnderlying(std::uint32_t mask) noexcept
{
    auto        flags = PairValue(LIBMAGIC_FLAG_NONE);
    std::size_t bit{};
    for (const auto& pair : LIBMAGIC_FLAGS) {
        if (((mask >> bit) & 1U) != 0U) {
            flags |= PairValue(pair);
        }
        ++bit;
    }
    return flags;
}

/**
 * @brief Look up the human-readable name of a single flag.
 *
 * Internal helper used by MagicFlags::ToString(char) to render each set
 * bit. Uses the `std::countr_zero` bit-index lookup into LIBMAGIC_FLAGS;
 * the table order matches the dense Flags enum bit positions (asserted
 * above).
 *
 * @param[in] flag The flag to convert.
 *
 * @returns The flag name (e.g. "Mime"), or "None" for Flags::None.
 */
[[nodiscard]] constexpr LibmagicValueNameT FlagName(MagicFlags::Flags flag)
{
    if (flag == MagicFlags::Flags::None) {
        return PairName(LIBMAGIC_FLAG_NONE);
    }
    return PairName(LIBMAGIC_FLAGS.at(
        static_cast<std::size_t>(std::countr_zero(std::to_underlying(flag)))
    ));
}
} /* anonymous namespace */

std::string MagicFlags::ToString(const char separator) const
{
    return Utility::ToString(
        ToContainer(),
        std::string(1, separator),
        [](Flags flag) {
            return std::string{FlagName(flag)};
        }
    );
}

int MagicFlags::ToUnderlying() const noexcept
{
    return MaskToUnderlying(m_mask);
}
} /* namespace Recognition */
