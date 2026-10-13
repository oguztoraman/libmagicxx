/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags.hpp
 * @brief Header file for the MagicFlags class.
 *
 * This file contains the MagicFlags class, a self-contained value type that
 * owns the libmagicxx configuration flags: the `Flags` enum and the
 * operations to combine flags and convert them to containers of flag values
 * or human-readable names.
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 *
 * @see https://github.com/oguztoraman/libmagicxx
 * @see https://github.com/file/file (underlying libmagic)
 */

#ifndef MAGIC_FLAGS_HPP
#define MAGIC_FLAGS_HPP

#include <algorithm>
#include <array>
#include <bit>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include "static_string.hpp"

/**
 * @namespace Recognition
 * @brief Root namespace for the libmagicxx library.
 *
 * @since 10.0.0
 */
namespace Recognition {
/**
 * @class MagicFlags
 * @ingroup magic_core
 *
 * @brief A modern C++23 value class owning Magic configuration flags.
 *
 * MagicFlags represents a set of `Flags` values: a single flag converts
 * implicitly, flags combine with bitwise OR, and the set converts to a
 * container of flag values or to human-readable names.
 *
 * ### Key Features
 *
 * - **Value Semantics**: Copyable and movable flag container.
 * - **Exception Safety**: Every operation is `noexcept`; only
 *   `ToContainer()` allocates.
 * - **Combination**: Implicit conversion from a single `Flags` value and
 *   `operator|` for building flag sets.
 * - **Explicit Conversions**: `ToContainer()` and `ToString()` named member
 *   functions instead of implicit conversion operators.
 * - **Ordering**: Flag sets are comparable with `operator==` and ordered by
 *   bitmask with `operator<=>`.
 *
 * ### Usage Examples
 *
 * @code{.cpp}
 * #include <magic_flags.hpp>
 * #include <print>
 *
 * using namespace Recognition;
 *
 * // Single flag converts implicitly
 * MagicFlags flags{MagicFlags::Flags::Mime};
 *
 * // Combine flags with bitwise OR
 * MagicFlags combined = MagicFlags::Flags::Mime | MagicFlags::Flags::Compress;
 *
 * // Names of the set flags (ordered by flag position)
 * std::println("{}", combined.ToString());  // "Compress, Mime"
 * std::println("{}", flags.ToString());     // "Mime"
 * @endcode
 *
 * @see Magic for the file type identification class using these flags.
 *
 * @since 11.0.0
 */
class MagicFlags {
public:
    /**
     * @brief Flags for configuring Magic behavior.
     * @ingroup magic_core
     *
     * The Flags enum controls how Magic identifies files and formats output.
     * Flags can be combined using bitwise OR operations.
     *
     * ### Common Flag Combinations
     *
     * @code{.cpp}
     * // Get MIME type only
     * Magic magic1{Magic::FlagsT::MimeType};
     *
     * // Get full MIME with encoding
     * Magic magic2{Magic::FlagsT::Mime};
     *
     * // Follow symlinks and decompress files
     * Magic magic3{Magic::FlagsT::Symlink | Magic::FlagsT::Compress};
     *
     * // Using a container of flags
     * Magic magic4{{Magic::FlagsT::Mime, Magic::FlagsT::Debug}};
     * @endcode
     *
     * @see Magic::SetFlags() to change flags after construction
     * @see Magic::GetFlags() to retrieve current flags
     *
     * @since 11.0.0
     */
    enum class Flags : std::uint32_t {
        /* clang-format off */
        None             = 0U,       /**< No special handling. Default textual output. */
        Debug            = 1U << 0,  /**< Print debugging messages to stderr. Useful for troubleshooting. */
        Symlink          = 1U << 1,  /**< If the file is a symlink, follow it and identify the target. */
        Compress         = 1U << 2,  /**< If the file is compressed, decompress and identify contents. */
        Devices          = 1U << 3,  /**< Open block/character devices and examine their contents. */
        MimeType         = 1U << 4,  /**< Return MIME type (e.g., "text/plain") instead of description. */
        ContinueSearch   = 1U << 5,  /**< Return all matches, not just the first one. */
        CheckDatabase    = 1U << 6,  /**< Check database consistency and print warnings to stderr. */
        PreserveAtime    = 1U << 7,  /**< Preserve access time of analyzed files (if supported by OS). */
        Raw              = 1U << 8,  /**< Don't convert unprintable characters to \\ooo octal. */
        Error            = 1U << 9,  /**< Treat OS errors as real errors instead of printing in buffer. */
        MimeEncoding     = 1U << 10, /**< Return MIME encoding (e.g., "us-ascii") instead of description. */
        Mime             = 1U << 11, /**< Shorthand for MimeType | MimeEncoding. Returns full MIME. */
        Apple            = 1U << 12, /**< Return Apple creator and type codes. */
        Extension        = 1U << 13, /**< Return slash-separated list of file extensions. */
        CompressTransp   = 1U << 14, /**< Report on uncompressed data only, hide compression layer. */
        NoCompressFork   = 1U << 15, /**< Don't use decompressors that require fork(). */
        Nodesc           = 1U << 16, /**< Shorthand for Extension | Mime | Apple. */
        NoCheckCompress  = 1U << 17, /**< Skip compressed file inspection. */
        NoCheckTar       = 1U << 18, /**< Skip tar archive examination. */
        NoCheckSoft      = 1U << 19, /**< Skip magic file consultation. */
        NoCheckApptype   = 1U << 20, /**< Skip EMX application type check (EMX only). */
        NoCheckElf       = 1U << 21, /**< Skip ELF details printing. */
        NoCheckText      = 1U << 22, /**< Skip text file type detection. */
        NoCheckCdf       = 1U << 23, /**< Skip MS Compound Document inspection. */
        NoCheckCsv       = 1U << 24, /**< Skip CSV file examination. */
        NoCheckTokens    = 1U << 25, /**< Skip known token search in ASCII files. */
        NoCheckEncoding  = 1U << 26, /**< Skip text encoding detection. */
        NoCheckJson      = 1U << 27, /**< Skip JSON file examination. */
        NoCheckSimh      = 1U << 28, /**< Skip SIMH tape file examination. */
        NoCheckBuiltin   = 1U << 29  /**< Use only magic file, skip all built-in tests. */
        /* clang-format on */
    };

    /**
     * @typedef FlagsContainerT
     *
     * @brief Container type holding a collection of MagicFlags::Flags.
     *
     * @since 11.0.0
     */
    using FlagsContainerT = std::vector<Flags>;

    /**
     * @brief Default constructor. Creates an empty flag set.
     *
     * @code{.cpp}
     * MagicFlags flags;  // No flags set
     * @endcode
     *
     * @since 11.0.0
     */
    MagicFlags() noexcept = default;

    /**
     * @brief Implicit constructor from a single flag.
     *
     * Allows Flags values to be used wherever MagicFlags is expected,
     * without explicit conversion.
     *
     * @param[in] flag The flag to include in the set.
     *
     * @code{.cpp}
     * // Single flag converts implicitly
     * Magic magic{Magic::FlagsT::Mime};
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicFlags(/* NOLINT(google-explicit-constructor) */
        Flags flag
    ) noexcept
      : m_mask{std::to_underlying(flag)}
    { }

    /**
     * @brief Construct from a container of flags.
     *
     * The resulting instance contains every flag in the container.
     *
     * @param[in] flags_container Container of individual Flags values.
     *
     * @code{.cpp}
     * Magic magic{{Magic::FlagsT::Mime, Magic::FlagsT::Debug}};
     * @endcode
     *
     * @since 11.0.0
     */
    explicit MagicFlags(const FlagsContainerT& flags_container) noexcept
      : m_mask{std::accumulate(
            flags_container.begin(),
            flags_container.end(),
            std::uint32_t{},
            [](std::uint32_t accumulator, Flags flag) {
                return accumulator | std::to_underlying(flag);
            }
        )}
    { }

    /**
     * @brief Copy constructor.
     *
     * @param[in] other The MagicFlags instance to copy.
     *
     * @since 11.0.0
     */
    MagicFlags(const MagicFlags& other) noexcept = default;

    /**
     * @brief Move constructor.
     *
     * @param[in,out] other The MagicFlags instance to move from.
     *
     * @since 11.0.0
     */
    MagicFlags(MagicFlags&& other) noexcept = default;

    /**
     * @brief Destructor.
     *
     * @since 11.0.0
     */
    ~MagicFlags() = default;

    /**
     * @brief Copy assignment operator.
     *
     * @param[in] other The MagicFlags instance to copy.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    MagicFlags& operator=(const MagicFlags& other) noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * @param[in,out] other The MagicFlags instance to move from.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    MagicFlags& operator=(MagicFlags&& other) noexcept = default;

    /**
     * @brief Combine this flag set with another one.
     *
     * @param[in] other The other flag set to combine.
     *
     * @returns A new MagicFlags containing the flags of both operands.
     *
     * @code{.cpp}
     * MagicFlags combined = MagicFlags{MagicFlags::Flags::Mime}
     *                     | MagicFlags{MagicFlags::Flags::Compress};
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr MagicFlags operator|(
        const MagicFlags& other
    ) const noexcept
    {
        MagicFlags result{};
        result.m_mask = m_mask | other.m_mask;
        return result;
    }

    /**
     * @brief Combine two Flags values into a MagicFlags.
     *
     * @param[in] lhs Left-hand side flag.
     * @param[in] rhs Right-hand side flag.
     *
     * @returns A MagicFlags with both flags set.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr MagicFlags operator|(
        Flags lhs,
        Flags rhs
    ) noexcept
    {
        return MagicFlags{lhs} | MagicFlags{rhs};
    }

    /**
     * @brief Combine a Flags value with a MagicFlags.
     *
     * Enables expressions like `Flags::A | (Flags::B | Flags::C)`.
     *
     * @param[in] lhs Left-hand side flag.
     * @param[in] rhs Right-hand side flag set.
     *
     * @returns A MagicFlags containing the flags of both operands.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr MagicFlags operator|(
        Flags             lhs,
        const MagicFlags& rhs
    ) noexcept
    {
        return MagicFlags{lhs} | rhs;
    }

    /**
     * @brief Combine a MagicFlags with a Flags value.
     *
     * Enables expressions like `(Flags::A | Flags::B) | Flags::C`.
     *
     * @param[in] lhs Left-hand side flag set.
     * @param[in] rhs Right-hand side flag.
     *
     * @returns A MagicFlags containing the flags of both operands.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr MagicFlags operator|(
        const MagicFlags& lhs,
        Flags             rhs
    ) noexcept
    {
        return lhs | MagicFlags{rhs};
    }

    /**
     * @brief Compare two flag sets for equality.
     *
     * @param[in] lhs Left-hand side flag set.
     * @param[in] rhs Right-hand side flag set.
     *
     * @returns `true` if both sets contain the same flags.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr bool operator==(
        const MagicFlags& lhs,
        const MagicFlags& rhs
    ) noexcept
    {
        return lhs.m_mask == rhs.m_mask;
    }

    /**
     * @brief Compare two flag sets by their underlying bitmask.
     *
     * @param[in] lhs Left-hand side flag set.
     * @param[in] rhs Right-hand side flag set.
     *
     * @returns A `std::strong_ordering` result comparing the two bitmasks.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(
        const MagicFlags& lhs,
        const MagicFlags& rhs
    ) noexcept
    {
        return lhs.m_mask <=> rhs.m_mask;
    }

    /**
     * @brief Convert the flag set to a container of flag values.
     *
     * Returns each active flag as a Flags enum value, ordered by ascending
     * bit position.
     *
     * @returns Container of active Flags values; `{Flags::None}` when no flag
     *          is set.
     *
     * @note Bit positions that no Flags enumerator covers are preserved as
     *       raw enum values with a single unknown bit set, so invalid flag
     *       sets remain observable through this conversion and ToString().
     *       Such masks are still rejected by Magic::Open() and
     *       Magic::SetFlags().
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * MagicFlags::FlagsContainerT container = flags.ToContainer();
     * // container == {Flags::Compress, Flags::Mime}
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] FlagsContainerT ToContainer() const
    {
        if (m_mask == 0U) {
            return {Flags::None};
        }
        FlagsContainerT flags_container;
        for (
            std::uint32_t remaining{m_mask}; remaining != 0U;
            remaining &= remaining - 1U
        ) {
            flags_container.push_back(
                static_cast<Flags>(
                    std::uint32_t{1U} << std::countr_zero(remaining)
                )
            );
        }
        return flags_container;
    }

    /**
     * @brief Default inline capacity of the string returned by ToString().
     *
     * The smallest capacity that fits every flag name with the default
     * separator.
     *
     * @since 11.0.0
     */
    static constexpr std::size_t TO_STRING_CAPACITY{360UZ};

    /**
     * @brief Convert the flag set to a separator-joined names string.
     *
     * The result is a fixed-capacity Utility::StaticString: rendering never
     * allocates and never throws, and text beyond `CapacityV` characters is
     * truncated (see StaticString::IsTruncated()).
     *
     * @tparam CapacityV Inline capacity, in characters, of the returned
     *                   StaticString (default: `TO_STRING_CAPACITY`).
     *
     * @param[in] separator String separating the names (default: `, `).
     *
     * @returns The flag names in bit order (e.g. "Compress, Mime"),
     *          "None" when no flag is set, with unknown bits rendered as
     *          "Invalid(N)".
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * auto names = flags.ToString();      // "Compress, Mime"
     * auto semi  = flags.ToString("; ");  // "Compress; Mime"
     * @endcode
     *
     * @since 11.0.0
     */
    template <std::size_t CapacityV = TO_STRING_CAPACITY>
    [[nodiscard]] Utility::StaticString<CapacityV> ToString(
        std::string_view separator = ", "
    ) const noexcept
    {
        using Utility::StaticString;
        if (m_mask == 0U) {
            return StaticString<CapacityV>{std::string_view{"None"}};
        }
        StaticString<CapacityV> result{};
        std::size_t             bit{};
        for (const auto& name : TO_STRING_NAMES) {
            if (((m_mask >> bit) & 1U) != 0U) {
                if (!result.Empty()) {
                    result.Append(separator);
                }
                result.Append(name);
            }
            ++bit;
        }
        for (
            std::uint32_t unknown{m_mask >> FLAG_COUNT}; unknown != 0U;
            unknown &= unknown - 1U
        ) {
            if (!result.Empty()) {
                result.Append(separator);
            }
            result.Append(
                std::string_view{StaticString<CapacityV>::Format(
                    "Invalid({})",
                    std::uint32_t{1U}
                        << (FLAG_COUNT
                            + static_cast<std::size_t>(
                                std::countr_zero(unknown)
                            ))
                )}
            );
        }
        return result;
    }

private:
    /**
     * @brief Number of bit positions covered by the Flags enum.
     */
    static constexpr std::size_t FLAG_COUNT{
        std::bit_width(std::to_underlying(Flags::NoCheckBuiltin))
    };

    /**
     * @brief Human-readable name of each Flags bit position.
     *
     * Indexed by bit position; consumed by ToString().
     */
    static constexpr std::array<std::string_view, FLAG_COUNT> TO_STRING_NAMES{
        "Debug",
        "Symlink",
        "Compress",
        "Devices",
        "MimeType",
        "ContinueSearch",
        "CheckDatabase",
        "PreserveAtime",
        "Raw",
        "Error",
        "MimeEncoding",
        "Mime",
        "Apple",
        "Extension",
        "CompressTransp",
        "NoCompressFork",
        "Nodesc",
        "NoCheckCompress",
        "NoCheckTar",
        "NoCheckSoft",
        "NoCheckApptype",
        "NoCheckElf",
        "NoCheckText",
        "NoCheckCdf",
        "NoCheckCsv",
        "NoCheckTokens",
        "NoCheckEncoding",
        "NoCheckJson",
        "NoCheckSimh",
        "NoCheckBuiltin"
    };

    static_assert(
        TO_STRING_NAMES.size() == FLAG_COUNT,
        "TO_STRING_NAMES must cover every Flags bit position"
    );

    static_assert(
        []() noexcept {
            std::size_t required{};
            for (const auto name : TO_STRING_NAMES) {
                required += name.size() + 2UZ; /* name + ", " */
            }
            return required - 2UZ; /* no trailing separator */
        }() <= TO_STRING_CAPACITY,
        "TO_STRING_CAPACITY must fit every flag name with the default "
        "separator"
    );

    static_assert(
        std::ranges::all_of(
            TO_STRING_NAMES,
            [](const std::string_view name) {
                return !name.empty();
            }
        ),
        "TO_STRING_NAMES must carry a non-empty name for every bit position"
    );

    /**
     * @brief Convert the flag set to the libmagic flags value.
     *
     * @returns Libmagic-compatible integer flags value, or `std::nullopt` if
     *          the stored mask carries a bit position that no `Flags`
     *          enumerator covers.
     */
    [[nodiscard]] std::optional<int> ToUnderlying() const noexcept;

    friend class Magic; /**< Grants Magic access to the libmagic bridge. */

    std::uint32_t m_mask{0U}; /**< Raw bitmask storing combined flag values. */
};
} /* namespace Recognition */

#endif /* MAGIC_FLAGS_HPP */
