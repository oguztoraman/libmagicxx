/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags.hpp
 * @brief Header file for the MagicFlags class.
 *
 * This file contains the MagicFlags class, a self-contained value type that
 * owns the libmagicxx configuration flags: the `Flags` enum and the
 * operations to combine flags, test their membership, iterate the set
 * flags, and render them as human-readable names.
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
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <limits>
#include <optional>
#include <string_view>
#include <utility>

#include "inplace_string.hpp"

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
 * MagicFlags represents a set of `Flags` values as a bitmask: a single flag
 * converts implicitly, flags combine with bitwise OR,
 * membership is queried with `Contains()`, and the set renders as
 * human-readable names with `ToString()`.
 *
 * ### Key Features
 *
 * - **Value Semantics**: Copyable and movable flag set.
 * - **Exception Safety**: Every class member is `constexpr noexcept`.
 * - **Combination**: Implicit conversion from a single `Flags` value and
 *   `operator|` for building flag sets (`Flags::Mime | Flags::Compress`),
 *   with `operator|=` and `Erase()` for in-place updates.
 * - **Queries**: `Contains()` for membership, `operator&` for
 *   intersections, and `Count()` for the number of set bits, all
 *   `constexpr noexcept`.
 * - **Iteration**: Range-for yields the set flags as `Flags` values in
 *   ascending bit position, for use with algorithms and views.
 * - **Rendering**: `ToString()` renders the set flags as names. A single
 *   `Flags` value also renders as its name with `std::format`.
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
 * constexpr MagicFlags combined =
 *     MagicFlags::Flags::Mime | MagicFlags::Flags::Compress;
 *
 * // Query membership (constexpr, noexcept, allocation-free)
 * constexpr bool is_mime = combined.Contains(MagicFlags::Flags::Mime);
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
     * // Print a single flag by name
     * std::println("{}", Magic::FlagsT::Mime);  // "Mime"
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
     * @brief Default capacity of the string returned by ToString().
     *
     * The smallest capacity that fits every flag name with the default
     * separator.
     *
     * @since 11.0.0
     */
    static constexpr std::size_t TO_STRING_CAPACITY{360UZ};

    /**
     * @class Iterator
     * @brief Forward iterator over the set flags of a flag set.
     *
     * The Iterator walks the set flags in ascending bit position, yielding
     * each flag by value, so the iterator models `std::forward_iterator`.
     *
     * @see MagicFlags::begin()
     * @see MagicFlags::end()
     *
     * @since 11.0.0
     */
    class Iterator {
    public:
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using value_type = Flags; /**< Flag yielded by dereference. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using difference_type = std::ptrdiff_t; /**< Signed distance type. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using reference = Flags; /**< Dereference yields by value. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using iterator_category = std::
            forward_iterator_tag; /**< Iterator category. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using iterator_concept = std::
            forward_iterator_tag; /**< Iterator concept. */

        /**
         * @brief Construct a past-the-end iterator.
         *
         * A default-constructed iterator compares equal to the end of any
         * flag set.
         *
         * @since 11.0.0
         */
        constexpr Iterator() noexcept = default;

        /**
         * @brief Dereference the iterator.
         *
         * @returns The flag for the current bit position. The behavior is
         *          undefined at the end position.
         *
         * @since 11.0.0
         */
        [[nodiscard]] constexpr Flags operator*() const noexcept
        {
            return static_cast<Flags>(
                static_cast<std::uint32_t>(BIT_MASK << m_bit)
            );
        }

        /**
         * @brief Advance to the next set bit.
         *
         * @returns Reference to this iterator.
         *
         * @since 11.0.0
         */
        constexpr Iterator& operator++() noexcept
        {
            m_remaining &= ~static_cast<std::uint32_t>(BIT_MASK << m_bit);
            m_bit = m_remaining != 0U
                      ? static_cast<std::size_t>(std::countr_zero(m_remaining))
                      : BIT_COUNT;
            return *this;
        }

        /**
         * @brief Advance to the next set bit, returning the previous
         *        position.
         *
         * @returns A copy of this iterator taken before the increment.
         *
         * @since 11.0.0
         */
        constexpr Iterator operator++(int) noexcept
        {
            const Iterator previous{*this};
            ++*this;
            return previous;
        }

        /**
         * @brief Compare two iterators by bit position.
         *
         * @param[in] lhs Left-hand side iterator.
         * @param[in] rhs Right-hand side iterator.
         *
         * @returns `true` if both iterators sit at the same bit position.
         *
         * @since 11.0.0
         */
        [[nodiscard]] friend constexpr bool operator==(
            const Iterator& lhs,
            const Iterator& rhs
        ) noexcept
        {
            return lhs.m_bit == rhs.m_bit;
        }

    private:
        /**
         * @brief Number of bit positions in the iterated bitmask.
         */
        static constexpr std::size_t BIT_COUNT{
            std::numeric_limits<std::uint32_t>::digits
        };

        /**
         * @brief Unit bit value for bitmask shifts.
         */
        static constexpr std::uint32_t BIT_MASK{1U};

        /**
         * @brief Construct over a bitmask at its lowest set bit.
         *
         * @param[in] mask The bitmask being iterated.
         */
        constexpr explicit Iterator(std::uint32_t mask) noexcept
          : m_remaining{mask}
          , m_bit{
                mask != 0U ? static_cast<std::size_t>(std::countr_zero(mask))
                           : BIT_COUNT
            }
        { }

        std::uint32_t
            m_remaining{}; /**< Bits not yet yielded, including m_bit. */
        std::size_t m_bit{
            BIT_COUNT
        }; /**< Current bit position, BIT_COUNT at end. */

        friend class MagicFlags; /**< Grants flag-set construction. */
    };

    /**
     * @brief Default constructor. Creates an empty flag set.
     *
     * @code{.cpp}
     * MagicFlags flags;  // No flags set
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicFlags() noexcept = default;

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
     * @brief Copy constructor.
     *
     * @param[in] other The MagicFlags instance to copy.
     *
     * @since 11.0.0
     */
    constexpr MagicFlags(const MagicFlags& other) noexcept = default;

    /**
     * @brief Move constructor.
     *
     * @param[in,out] other The MagicFlags instance to move from.
     *
     * @since 11.0.0
     */
    constexpr MagicFlags(MagicFlags&& other) noexcept = default;

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
    constexpr MagicFlags& operator=(const MagicFlags& other) noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * @param[in,out] other The MagicFlags instance to move from.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    constexpr MagicFlags& operator=(MagicFlags&& other) noexcept = default;

    /**
     * @brief Test whether a single flag is set.
     *
     * @param[in] flag The flag to look for.
     *
     * @returns `true` if every bit of `flag` is set in this set.
     *
     * @note `Flags::None` carries no bits: it is contained only in an
     *       empty set.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * flags.Contains(MagicFlags::Flags::Mime);      // true
     * flags.Contains(MagicFlags::Flags::Symlink);   // false
     * flags.Contains(MagicFlags::Flags::None);      // false, flags is not
     *                                               // empty
     * MagicFlags{}.Contains(MagicFlags::Flags::None);  // true
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool Contains(Flags flag) const noexcept
    {
        const auto bit{std::to_underlying(flag)};
        if (bit == 0U) {
            return m_mask == 0U;
        }
        return (m_mask & bit) == bit;
    }

    /**
     * @brief Test whether several flags are all set (AND semantics).
     *
     * @tparam RestT Additional `Flags`-compatible flag types.
     *
     * @param[in] first The first flag to look for.
     * @param[in] rest  The remaining flags to look for.
     *
     * @returns `true` if every listed flag is set in this set.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * flags.Contains(MagicFlags::Flags::Mime,
     *                MagicFlags::Flags::Compress);  // true
     * flags.Contains(MagicFlags::Flags::Mime,
     *                MagicFlags::Flags::Symlink);   // false
     * @endcode
     *
     * @since 11.0.0
     */
    template <typename... RestT>
    requires(std::convertible_to<RestT, Flags> && ...)
    [[nodiscard]] constexpr bool Contains(
        Flags first,
        RestT... rest
    ) const noexcept
    {
        return Contains(first) && (Contains(static_cast<Flags>(rest)) && ...);
    }

    /**
     * @brief Test whether every flag of another set is set (subset test).
     *
     * @param[in] subset The flag set to look for.
     *
     * @returns `true` if every flag of `subset` is set in this set.
     *
     * @note An empty subset is contained in every set.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * flags.Contains(MagicFlags::Flags::Mime
     *                | MagicFlags::Flags::Compress);  // true
     * flags.Contains(MagicFlags::Flags::Mime
     *                | MagicFlags::Flags::Debug);     // false
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool Contains(
        const MagicFlags& subset
    ) const noexcept
    {
        return (m_mask & subset.m_mask) == subset.m_mask;
    }

    /**
     * @brief Count the set flag bits.
     *
     * @returns The number of bits set in this flag set.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * flags.Count();  // 2
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr std::size_t Count() const noexcept
    {
        return static_cast<std::size_t>(std::popcount(m_mask));
    }

    /**
     * @brief Remove a single flag from this set in place.
     *
     * @param[in] flag The flag to remove.
     *
     * @returns Reference to this instance, without the bit of `flag`.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * flags.Erase(MagicFlags::Flags::Compress);
     * // flags == MagicFlags{MagicFlags::Flags::Mime}
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicFlags& Erase(Flags flag) noexcept
    {
        m_mask &= ~std::to_underlying(flag);
        return *this;
    }

    /**
     * @brief Remove several flags from this set in place.
     *
     * @param[in] to_erase The flag set to remove.
     *
     * @returns Reference to this instance, without the flags of
     *          `to_erase`.
     *
     * @since 11.0.0
     */
    constexpr MagicFlags& Erase(const MagicFlags& to_erase) noexcept
    {
        m_mask &= ~to_erase.m_mask;
        return *this;
    }

    /**
     * @brief Return an iterator to the first set flag.
     *
     * Iteration yields the set flags in ascending bit position, matching
     * the name order of ToString().
     *
     * @returns Iterator at the set flag with the lowest bit position, or
     *          the end position when no flag is set.
     *
     * @code{.cpp}
     * for (MagicFlags::Flags flag : flags) {
     *     std::println("{}", flag);
     * }
     * @endcode
     *
     * @see Iterator
     *
     * @since 11.0.0
     */
    /* NOLINTNEXTLINE(readability-identifier-naming) */
    [[nodiscard]] constexpr Iterator begin() const noexcept
    {
        return Iterator{m_mask};
    }

    /**
     * @brief Return a past-the-end iterator.
     *
     * @returns Iterator that compares equal once iteration has consumed
     *          every set flag.
     *
     * @see Iterator
     *
     * @since 11.0.0
     */
    /* NOLINTNEXTLINE(readability-identifier-naming) */
    [[nodiscard]] constexpr Iterator end() const noexcept
    {
        return Iterator{};
    }

    /**
     * @brief Convert the flag set to a separator-joined names string.
     *
     * The result is a fixed-capacity Utility::InplaceString: rendering never
     * throws, and text beyond `CapacityV` characters is truncated (see
     * InplaceString::IsTruncated()).
     *
     * @tparam CapacityV Capacity, in characters, of the returned
     *                   InplaceString (default: `TO_STRING_CAPACITY`).
     *
     * @param[in] separator String separating the names (default: `, `).
     *
     * @returns The flag names in bit order (e.g. "Compress, Mime") and
     *          "None" when no flag is set.
     *
     * @note Unknown bits are rendered as "Invalid(N)"; masks carrying
     *       them are rejected by Magic::Open() and Magic::SetFlags().
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
    [[nodiscard]] constexpr Utility::InplaceString<CapacityV> ToString(
        std::string_view separator = ", "
    ) const noexcept
    {
        using Utility::InplaceString;
        if (m_mask == 0U) {
            return InplaceString<CapacityV>{std::string_view{"None"}};
        }
        InplaceString<CapacityV> result{};
        std::size_t              bit{};
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
                std::string_view{InplaceString<CapacityV>::Format(
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
     * @brief Combine this flag set with another one in place.
     *
     * @param[in] other The other flag set to combine.
     *
     * @returns Reference to this instance, holding the flags of both
     *          operands.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime};
     * flags |= MagicFlags{MagicFlags::Flags::Compress};
     * // flags == MagicFlags::Flags::Mime | MagicFlags::Flags::Compress
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicFlags& operator|=(const MagicFlags& other) noexcept
    {
        m_mask |= other.m_mask;
        return *this;
    }

    /**
     * @brief Combine this flag set with a single flag in place.
     *
     * @param[in] flag The flag to add.
     *
     * @returns Reference to this instance, holding the flags of both
     *          operands.
     *
     * @since 11.0.0
     */
    constexpr MagicFlags& operator|=(Flags flag) noexcept
    {
        return *this |= MagicFlags{flag};
    }

    /**
     * @brief Intersect this flag set with another one.
     *
     * @param[in] other The other flag set to intersect with.
     *
     * @returns A new MagicFlags holding only the flags set in both operands.
     *
     * @code{.cpp}
     * MagicFlags a{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * MagicFlags b{MagicFlags::Flags::Mime};
     * MagicFlags shared = a & b;  // MagicFlags{Flags::Mime}
     * @endcode
     *
     * @see Contains(const MagicFlags&) to test whether one set is a subset
     *      of another
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr MagicFlags operator&(
        const MagicFlags& other
    ) const noexcept
    {
        MagicFlags result{};
        result.m_mask = m_mask & other.m_mask;
        return result;
    }

    /**
     * @brief Intersect a Flags value with a MagicFlags.
     *
     * @param[in] lhs Left-hand side flag.
     * @param[in] rhs Right-hand side flag set.
     *
     * @returns A MagicFlags holding only the flags set in both operands.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr MagicFlags operator&(
        Flags             lhs,
        const MagicFlags& rhs
    ) noexcept
    {
        return MagicFlags{lhs} & rhs;
    }

    /**
     * @brief Intersect a MagicFlags with a Flags value.
     *
     * @param[in] lhs Left-hand side flag set.
     * @param[in] rhs Right-hand side flag.
     *
     * @returns A MagicFlags holding only the flags set in both operands.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr MagicFlags operator&(
        const MagicFlags& lhs,
        Flags             rhs
    ) noexcept
    {
        return lhs & MagicFlags{rhs};
    }

    /**
     * @brief Intersect this flag set with another one in place.
     *
     * @param[in] other The other flag set to intersect with.
     *
     * @returns Reference to this instance, holding only the flags set in
     *          both operands.
     *
     * @code{.cpp}
     * MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
     * flags &= MagicFlags{MagicFlags::Flags::Mime};
     * // flags == MagicFlags{MagicFlags::Flags::Mime}
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicFlags& operator&=(const MagicFlags& other) noexcept
    {
        m_mask &= other.m_mask;
        return *this;
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

static_assert(
    std::forward_iterator<MagicFlags::Iterator>,
    "Iterator must stay a forward iterator so flag sets support multi-pass"
    " algorithms"
);
} /* namespace Recognition */

namespace std {
/**
 * @brief Format specialization rendering a `MagicFlags::Flags` value as its
 *        flag name.
 *
 * Makes the enum usable directly with `std::format` / `std::print` /
 * `std::println`, rendering the name of the flag (e.g. "Mime", "None" for
 * `Flags::None`, or "Invalid(N)" when no enumerator covers the value),
 * consistent with `MagicFlags::ToString()`. Format specs (fill, alignment,
 * width) are applied to the rendered name.
 *
 * @code{.cpp}
 * std::println("{}", Recognition::MagicFlags::Flags::Mime);  // "Mime"
 * @endcode
 *
 * @since 11.0.0
 */
template <>
/* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
struct formatter<Recognition::MagicFlags::Flags, char>
  : formatter<std::string_view> {
    /**
     * @brief Format `flag` into `ctx` as its flag name.
     *
     * @param[in] flag The flag to render.
     * @param[in] ctx  The output format context.
     *
     * @returns Iterator past the last formatted character.
     */
    template <typename FormatContextT>
    auto format(
        const Recognition::MagicFlags::Flags flag,
        FormatContextT&                      ctx
    ) const
    {
        const auto names{Recognition::MagicFlags{flag}.ToString<64UZ>()};
        return formatter<std::string_view>::format(
            std::string_view{names},
            ctx
        );
    }
};

/**
 * @brief Format specialization rendering a `MagicFlags` set as its
 *        comma-joined flag names.
 *
 * Makes the flag set usable directly with `std::format` / `std::print` /
 * `std::println`, rendering the names in bit order (e.g. "Compress, Mime",
 * or "None" when empty), consistent with `MagicFlags::ToString()`. Format
 * specs (fill, alignment, width) are applied to the rendered names.
 *
 * @code{.cpp}
 * std::println("{}", Recognition::MagicFlags{
 *     Recognition::MagicFlags::Flags::Mime});  // "Mime"
 * @endcode
 *
 * @since 11.0.0
 */
template <>
/* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
struct formatter<Recognition::MagicFlags, char> : formatter<std::string_view> {
    /**
     * @brief Format `flags` into `ctx` as their names.
     *
     * @param[in] flags The flag set to render.
     * @param[in] ctx   The output format context.
     *
     * @returns Iterator past the last formatted character.
     */
    template <typename FormatContextT>
    auto format(const Recognition::MagicFlags& flags, FormatContextT& ctx) const
    {
        return formatter<std::string_view>::format(
            std::string_view{flags.ToString()},
            ctx
        );
    }
};
} /* namespace std */

#endif /* MAGIC_FLAGS_HPP */
