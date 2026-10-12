/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file static_string.hpp
 * @brief Allocation-free fixed-capacity inline string for `noexcept`
 *        boundaries.
 * @ingroup magic_utility
 *
 * This file defines Utility::StaticString, a fixed-capacity string whose
 * entire lifetime — construction, copy, move, assignment, append, and
 * formatting — is `noexcept` and performs zero heap allocation.
 *
 * @see Recognition::Utility::StaticString
 */

#ifndef STATIC_STRING_HPP
#define STATIC_STRING_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <format>
#include <iterator>
#include <ostream>
#include <ranges>
#include <string_view>

namespace Recognition::Utility {
/**
 * @addtogroup magic_utility
 * @{
 */

/**
 * @class StaticString
 * @ingroup magic_utility
 *
 * @brief A fixed-capacity, allocation-free inline string with `noexcept`
 *        operations, suitable for use across `noexcept` boundaries.
 *
 * `StaticString` stores its characters inline in a fixed-size buffer of at
 * most `CapacityV` characters (plus a terminating null character). Every
 * operation — construction, copy, move, assignment, append, and formatting
 * — is `noexcept` and performs zero heap allocation.
 *
 * If the represented text exceeds `CapacityV`, the text is truncated to the
 * first `CapacityV` characters and `IsTruncated()` reports `true`.
 *
 * ### Comparison transparency
 *
 * `StaticString` compares equal (and orders) against `std::string_view`,
 * `const char*`, and `std::string`, and compares equal against other
 * `StaticString` instantiations, via implicit conversion to
 * `std::string_view` and dedicated comparison operators. Ordering against
 * another `StaticString` uses its explicit conversion to
 * `std::string_view`.
 *
 * @tparam CapacityV Maximum number of characters stored inline (excluding
 *                   the terminating null character).
 *
 * @section static_string_example Example
 *
 * @code{.cpp}
 * using namespace Recognition::Utility;
 *
 * StaticString<64> message{"hello"};          // noexcept, no allocation
 * message.Append(" world");                   // noexcept
 * auto formatted = StaticString<64>::Format(
 *     "value: {}", 42                          // bounded formatting
 * );
 * std::string_view view{message};             // borrowed inline storage
 * @endcode
 *
 * @since 11.0.0
 */
template <std::size_t CapacityV>
class StaticString {
public:
    /**
     * @brief Construct an empty string.
     *
     * @since 11.0.0
     */
    constexpr StaticString() noexcept = default;

    /**
     * @brief Construct from a `std::string_view`, truncating on overflow.
     *
     * @param[in] text The text to store.
     *
     * @par Error Handling
     * Never throws. If `text.size() > CapacityV`, the stored text is
     * truncated to the first `CapacityV` characters and `IsTruncated()`
     * returns `true`.
     *
     * @since 11.0.0
     */
    constexpr explicit StaticString(std::string_view text) noexcept
      : m_size{text.size() < CapacityV ? text.size() : CapacityV}
      , m_truncated{text.size() > CapacityV}
    {
        std::ranges::copy(text | std::views::take(m_size), m_buffer.begin());
    }

    /**
     * @brief Construct from a null-terminated character pointer.
     *
     * @param[in] text The text to store. A null pointer is treated as empty
     *                 text.
     *
     * @par Error Handling
     * Never throws. Overlong text is truncated (see the `std::string_view`
     * constructor).
     *
     * @since 11.0.0
     */
    constexpr explicit StaticString(const char* text) noexcept
      : StaticString{
            text != nullptr ? std::string_view{text} : std::string_view{}
        }
    { }

    /**
     * @brief Copy constructor. `noexcept` and allocation-free.
     *
     * @since 11.0.0
     */
    constexpr StaticString(const StaticString&) noexcept = default;

    /**
     * @brief Move constructor. `noexcept` and allocation-free.
     *
     * @since 11.0.0
     */
    constexpr StaticString(StaticString&&) noexcept = default;

    /**
     * @brief Copy assignment. `noexcept` and allocation-free.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr StaticString& operator=(const StaticString&) noexcept = default;

    /**
     * @brief Move assignment. `noexcept` and allocation-free.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr StaticString& operator=(StaticString&&) noexcept = default;

    /**
     * @brief Destructor. Trivially `noexcept`.
     *
     * @since 11.0.0
     */
    ~StaticString() noexcept = default;

    /**
     * @brief Assign from a `std::string_view`, truncating on overflow.
     *
     * @param[in] text The text to store.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr StaticString& operator=(std::string_view text) noexcept
    {
        return *this = StaticString{text};
    }

    /**
     * @brief Assign from a null-terminated character pointer.
     *
     * @param[in] text The text to store.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr StaticString& operator=(const char* text) noexcept
    {
        return *this = StaticString{text};
    }

    /**
     * @brief Convert to a `std::string_view` over the stored text.
     *
     * The returned view borrows the inline buffer and is invalidated when
     * the string is destroyed or modified.
     *
     * @returns A `std::string_view` over the stored text.
     *
     * @since 11.0.0
     */
    constexpr explicit operator std::string_view() const noexcept
    {
        return std::string_view{m_buffer.data(), m_size};
    }

    /**
     * @brief Get a pointer to the stored (null-terminated) text.
     *
     * @returns Pointer to the internal buffer, always null-terminated.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr const char* Data() const noexcept
    {
        return m_buffer.data();
    }

    /**
     * @brief Get a pointer to the stored (null-terminated) C string.
     *
     * @returns Pointer to the internal buffer, always null-terminated.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr const char* CStr() const noexcept
    {
        return m_buffer.data();
    }

    /**
     * @brief Get the number of stored characters.
     *
     * @returns The number of characters, excluding the terminating null
     *          character. Never exceeds `CapacityV`.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr std::size_t Size() const noexcept
    {
        return m_size;
    }

    /**
     * @brief Get the maximum number of characters stored inline (excluding
     *        the terminating null character).
     *
     * @returns The capacity `CapacityV`. Being a static member function, it
     *          is callable without an instance and usable in constant
     *          expressions.
     *
     * @code{.cpp}
     * constexpr auto cap{StaticString<64>::MaxSize()}; // 64
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] static constexpr std::size_t MaxSize() noexcept
    {
        return CapacityV;
    }

    /**
     * @brief Check whether the string is empty.
     *
     * @returns `true` if `Size() == 0`, `false` otherwise.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool Empty() const noexcept
    {
        return m_size == 0UZ;
    }

    /**
     * @brief Check whether text was dropped due to capacity overflow.
     *
     * @returns `true` if any construction, assignment, append, or formatting
     *          operation truncated the text, `false` otherwise.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool IsTruncated() const noexcept
    {
        return m_truncated;
    }

    /**
     * @brief Append text to the end of the string, truncating on overflow.
     *
     * @param[in] text The text to append.
     *
     * @par Error Handling
     * Never throws. If the combined length exceeds `CapacityV`, the stored
     * text is truncated to the first `CapacityV` characters and
     * `IsTruncated()` returns `true`.
     *
     * @since 11.0.0
     */
    constexpr void Append(std::string_view text) noexcept
    {
        const std::size_t available{CapacityV - m_size};
        const std::size_t copied{
            text.size() < available ? text.size() : available
        };
        std::ranges::copy(
            text | std::views::take(copied),
            std::ranges::next(
                std::begin(m_buffer),
                static_cast<std::ptrdiff_t>(m_size)
            )
        );
        m_size += copied;
        m_truncated |= (text.size() > copied);
    }

    /**
     * @brief Format arguments into a new string, truncating on overflow.
     *
     * Formats `args` according to `format_text` (in `std::vformat` syntax)
     * into the inline buffer using bounded formatting. The operation never
     * allocates and never throws: if formatting reports an error, the
     * format string text itself is stored (truncated as needed) as a
     * fallback.
     *
     * @tparam ArgsT Argument types (each must be formattable as `char`).
     *
     * @param[in] format_text The format string (e.g., "'{}' does not exist.").
     * @param[in] args        Values referenced by the format string.
     *
     * @returns A new string holding the formatted text, or the format string
     *          text itself if formatting reported an error.
     *
     * @par Error Handling
     * Never throws: any exception raised during formatting is contained and
     * the format string text is stored verbatim instead. Overflow sets the
     * truncation flag (see `Append()`).
     *
     * @code{.cpp}
     * auto msg = StaticString<64>::Format("{}: {}", "code", 42);
     * // msg == "code: 42"
     * @endcode
     *
     * @since 11.0.0
     */
    template <typename... ArgsT>
    requires(std::formattable<ArgsT, char> && ...)
    [[nodiscard]] static constexpr StaticString Format(
        std::string_view format_text,
        /* NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward) */
        ArgsT&&... args
    ) noexcept
    {
        StaticString result{};
        try {
            const auto writer = std::vformat_to(
                BoundedCharWriter{result.m_buffer},
                format_text,
                std::make_format_args(args...)
            );
            result.m_size      = writer.WrittenSize();
            result.m_truncated = writer.IsFull();
        } catch (...) {
            result = StaticString{format_text};
        }
        return result;
    }

    /**
     * @brief Equality comparison against another `StaticString`.
     *
     * @related StaticString
     * @since 11.0.0
     */
    template <std::size_t OtherCapacityV>
    friend constexpr bool operator==(
        const StaticString&                 lhs,
        const StaticString<OtherCapacityV>& rhs
    ) noexcept
    {
        return std::string_view{lhs} == std::string_view{rhs};
    }

    /**
     * @brief Equality comparison against a `std::string_view`.
     *
     * @related StaticString
     * @since 11.0.0
     */
    friend constexpr bool operator==(
        const StaticString& lhs,
        std::string_view    rhs
    ) noexcept
    {
        return std::string_view{lhs} == rhs;
    }

    /**
     * @brief Three-way comparison against a `std::string_view`.
     *
     * @related StaticString
     * @since 11.0.0
     */
    friend constexpr std::strong_ordering operator<=>(
        const StaticString& lhs,
        std::string_view    rhs
    ) noexcept
    {
        return std::string_view{lhs} <=> rhs;
    }

private:
    /**
     * @brief Bounded output iterator writing into the inline buffer.
     *
     * Characters are written until the buffer is full; further characters
     * are discarded and `IsFull()` reports the drop. Used by `Format()` to
     * perform allocation-free bounded formatting.
     *
     * @since 11.0.0
     */
    struct BoundedCharWriter {
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using value_type = char; /**< Element type written. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using difference_type = std::ptrdiff_t; /**< Signed distance type. */
        using StorageT        = std::array<
            value_type,
            CapacityV + 1UZ
        >; /**< Buffer type. */

        /**
         * @brief Construct a writer over `storage`'s inline buffer.
         *
         * @param[in] storage The inline buffer characters are written into.
         */
        constexpr explicit BoundedCharWriter(StorageT& storage) noexcept
          : m_cursor{storage.begin()}
        { }

        /**
         * @brief Return this writer (the sink form of `*it = c`).
         *
         * @returns Reference to this writer.
         */
        constexpr BoundedCharWriter& operator*() noexcept
        {
            return *this;
        }

        /**
         * @brief Write `character` at the cursor, discarding it when the
         *        buffer is full.
         *
         * @param[in] character The character to write.
         *
         * @returns Reference to this writer.
         */
        constexpr BoundedCharWriter& operator=(char character) noexcept
        {
            if (m_size != CapacityV) {
                *m_cursor = character;
                std::advance(m_cursor, 1);
                ++m_size;
            } else {
                m_is_full = true;
            }
            return *this;
        }

        /**
         * @brief No-op prefix increment (sink semantics).
         *
         * @returns Reference to this writer.
         */
        constexpr BoundedCharWriter& operator++() noexcept
        {
            return *this;
        }

        /**
         * @brief No-op postfix increment (sink semantics).
         *
         * @returns A copy of this writer.
         */
        constexpr BoundedCharWriter operator++(int) noexcept
        {
            return *this;
        }

        /**
         * @brief Number of characters actually written.
         *
         * @returns The count of characters written so far.
         */
        [[nodiscard]] constexpr std::size_t WrittenSize() const noexcept
        {
            return m_size;
        }

        /**
         * @brief Whether any character was discarded.
         *
         * @returns `true` if a write was discarded because the buffer was
         *          full, `false` otherwise.
         */
        [[nodiscard]] constexpr bool IsFull() const noexcept
        {
            return m_is_full;
        }

    private:
        std::ranges::iterator_t<StorageT>
                    m_cursor{}; /**< Write cursor into the inline buffer. */
        std::size_t m_size{};   /**< Characters written so far. */
        bool        m_is_full{false}; /**< Whether a write was discarded. */
    };

    std::array<char, CapacityV + 1UZ>
                m_buffer{};    /**< Inline storage (NUL-terminated). */
    std::size_t m_size{};      /**< Stored characters, excluding NUL. */
    bool        m_truncated{}; /**< Whether text was dropped. */
};

/**
 * @brief Stream the stored text of a `StaticString` to an output stream.
 *
 * @param[in,out] output_stream Destination stream.
 * @param[in]     value         String to print.
 *
 * @returns Reference to `output_stream`.
 *
 * @related StaticString
 * @since 11.0.0
 */
template <std::size_t CapacityV>
inline std::ostream& operator<<(
    std::ostream&                  output_stream,
    const StaticString<CapacityV>& value
)
{
    return output_stream << std::string_view{value};
}

/**
 * @brief Print a `StaticString` for GoogleTest diagnostics.
 * @ingroup magic_utility
 *
 * Found via ADL by GoogleTest when a `StaticString` value is printed on
 * assertion failure.
 *
 * @param[in]  value          The string to print.
 * @param[out] output_stream  The stream to print to.
 *
 * @related StaticString
 * @since 11.0.0
 */
template <std::size_t CapacityV>
inline void PrintTo(
    const StaticString<CapacityV>& value,
    std::ostream*                  output_stream
)
{
    *output_stream << "\"" << std::string_view{value} << "\"";
}

/** @} magic_utility */
} /* namespace Recognition::Utility */

namespace std {
/**
 * @brief Format specialization making `Utility::StaticString` usable with
 *        `std::format` / `std::print`.
 *
 * Delegates to the `std::string_view` formatter.
 *
 * @tparam CapacityV Maximum number of inline characters of the string being
 *                   formatted.
 *
 * @since 11.0.0
 */
template <std::size_t CapacityV>
/* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
struct formatter<Recognition::Utility::StaticString<CapacityV>, char>
  : formatter<string_view> {
    /**
     * @brief Format `value` into `ctx`.
     *
     * @param[in] value The string to format.
     * @param[in] ctx   The output format context.
     *
     * @returns Iterator past the last formatted character.
     */
    template <typename FormatContextT>
    auto format(
        const Recognition::Utility::StaticString<CapacityV>& value,
        FormatContextT&                                      ctx
    ) const
    {
        return formatter<string_view>::format(std::string_view{value}, ctx);
    }
};
} /* namespace std */

#endif /* STATIC_STRING_HPP */
