/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file inplace_string.hpp
 * @brief Allocation-free fixed-capacity string for `noexcept` boundaries.
 * @ingroup magic_utility
 *
 * This file defines Utility::InplaceString, a fixed-capacity string whose
 * entire lifetime — construction, copy, move, assignment, append, and
 * formatting — is `constexpr noexcept`.
 *
 * @see Recognition::Utility::InplaceString
 */

#ifndef INPLACE_STRING_HPP
#define INPLACE_STRING_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <format>
#include <functional>
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
 * @class InplaceString
 * @ingroup magic_utility
 *
 * @brief A fixed-capacity, allocation-free string with `constexpr
 *        noexcept` operations, suitable for use across `noexcept`
 *        boundaries.
 *
 * `InplaceString` holds at most `CapacityV` characters (plus a terminating
 * null character) and never grows. Every operation — construction, copy,
 * move, assignment, append, and formatting — is `constexpr noexcept`.
 *
 * If the represented text exceeds `CapacityV`, the text is truncated to the
 * first `CapacityV` characters and `IsTruncated()` reports `true`.
 *
 * The stored text is exposed as a contiguous range of characters
 * (`begin()`/`end()`), indexable (`operator[]`, `Front()`, `Back()`),
 * searchable (`Find()`, `Contains()`, `StartsWith()`, `EndsWith()`), and
 * hashable for use as an unordered container key.
 *
 * ### Comparison transparency
 *
 * `InplaceString` compares equal (and orders) against `std::string_view`,
 * `const char*`, and `std::string`, and compares equal and orders against
 * other `InplaceString` instantiations, via dedicated comparison operators
 * that compare the underlying text.
 *
 * @tparam CapacityV Maximum number of characters the string can hold
 *                   (excluding the terminating null character).
 *
 * @section inplace_string_example Example
 *
 * @code{.cpp}
 * using namespace Recognition::Utility;
 *
 * InplaceString<64> message{"hello"};          // noexcept, no allocation
 * message.Append(" world");                   // noexcept
 * auto formatted = InplaceString<64>::Format(
 *     "value: {}", 42                          // bounded formatting
 * );
 * std::string_view view{message};             // borrows the stored text
 * @endcode
 *
 * @since 11.0.0
 */
template <std::size_t CapacityV>
class InplaceString {
public:
    /**
     * @brief Construct an empty string.
     *
     * @since 11.0.0
     */
    constexpr InplaceString() noexcept = default;

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
    constexpr explicit InplaceString(std::string_view text) noexcept
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
    constexpr explicit InplaceString(const char* text) noexcept
      : InplaceString{
            text != nullptr ? std::string_view{text} : std::string_view{}
        }
    { }

    /**
     * @brief Copy constructor. `noexcept` and allocation-free.
     *
     * @since 11.0.0
     */
    constexpr InplaceString(const InplaceString&) noexcept = default;

    /**
     * @brief Move constructor. `noexcept` and allocation-free.
     *
     * @since 11.0.0
     */
    constexpr InplaceString(InplaceString&&) noexcept = default;

    /**
     * @brief Copy assignment. `noexcept` and allocation-free.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr InplaceString& operator=(const InplaceString&) noexcept = default;

    /**
     * @brief Move assignment. `noexcept` and allocation-free.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr InplaceString& operator=(InplaceString&&) noexcept = default;

    /**
     * @brief Destructor. Trivially `noexcept`.
     *
     * @since 11.0.0
     */
    ~InplaceString() noexcept = default;

    /**
     * @brief Get the first character.
     *
     * @returns The first character. The behavior is undefined when the
     *          string is empty, mirroring `std::string_view::front`.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr char Front() const noexcept
    {
        return std::string_view{*this}.front();
    }

    /**
     * @brief Get the last character.
     *
     * @returns The last character. The behavior is undefined when the
     *          string is empty, mirroring `std::string_view::back`.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr char Back() const noexcept
    {
        return std::string_view{*this}.back();
    }

    /**
     * @brief Return an iterator to the first stored character.
     *
     * The iterators expose the stored text as a contiguous range of
     * `char`. They are invalidated when the string is destroyed or
     * modified.
     *
     * @returns Pointer to the first character.
     *
     * @since 11.0.0
     */
    /* NOLINTNEXTLINE(readability-identifier-naming) */
    [[nodiscard]] constexpr const char* begin() const noexcept
    {
        return m_buffer.data();
    }

    /**
     * @brief Return an iterator past the last stored character.
     *
     * @returns Pointer past the last stored character (the terminating
     *          null position).
     *
     * @since 11.0.0
     */
    /* NOLINTNEXTLINE(readability-identifier-naming) */
    [[nodiscard]] constexpr const char* end() const noexcept
    {
        /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
        return m_buffer.data() + m_size;
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
     * @brief Get the maximum number of characters the string can hold
     *        (excluding the terminating null character).
     *
     * @returns The capacity `CapacityV`. Being a static member function, it
     *          is callable without an instance and usable in constant
     *          expressions.
     *
     * @code{.cpp}
     * constexpr auto cap{InplaceString<64>::MaxSize()}; // 64
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
     * @brief Reset the string to empty.
     *
     * After the call the string holds no characters and `IsTruncated()`
     * reports `false`, matching a default-constructed string.
     *
     * @since 11.0.0
     */
    constexpr void Clear() noexcept
    {
        m_size      = 0UZ;
        m_truncated = false;
        m_buffer[0] = '\0';
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
     * @brief Find the first occurrence of text.
     *
     * @param[in] text The text to search for.
     *
     * @returns The index of the first occurrence, or `std::string_view::npos`
     *          when the text is not found.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr std::size_t Find(
        std::string_view text
    ) const noexcept
    {
        return std::string_view{*this}.find(text);
    }

    /**
     * @brief Check whether the stored text contains the given text.
     *
     * @param[in] text The text to search for.
     *
     * @returns `true` if `text` occurs in the stored text, `false`
     *          otherwise.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool Contains(std::string_view text) const noexcept
    {
        return Find(text) != std::string_view::npos;
    }

    /**
     * @brief Check whether the stored text starts with the given text.
     *
     * @param[in] prefix The text to look for at the start.
     *
     * @returns `true` if `prefix` occurs at position 0, matching
     *          `std::string_view::starts_with` (an empty prefix always
     *          matches).
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool StartsWith(
        std::string_view prefix
    ) const noexcept
    {
        return std::string_view{*this}.starts_with(prefix);
    }

    /**
     * @brief Check whether the stored text ends with the given text.
     *
     * @param[in] suffix The text to look for at the end.
     *
     * @returns `true` if `suffix` occurs at the end, matching
     *          `std::string_view::ends_with` (an empty suffix always
     *          matches).
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool EndsWith(
        std::string_view suffix
    ) const noexcept
    {
        return std::string_view{*this}.ends_with(suffix);
    }

    /**
     * @brief Format arguments into a new string, truncating on overflow.
     *
     * Formats `args` according to `format_text` (in `std::format` syntax),
     * truncating on overflow. The operation never throws: if formatting
     * reports an error, the format string text itself is stored (truncated
     * as needed) as a fallback.
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
     * auto msg = InplaceString<64>::Format("{}: {}", "code", 42);
     * // msg == "code: 42"
     * @endcode
     *
     * @since 11.0.0
     */
    template <typename... ArgsT>
    requires(std::formattable<ArgsT, char> && ...)
    [[nodiscard]] static constexpr InplaceString Format(
        std::string_view format_text,
        /* NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward) */
        ArgsT&&... args
    ) noexcept
    {
        InplaceString result{};
        try {
            const auto writer = std::vformat_to(
                BoundedCharWriter{result.m_buffer},
                format_text,
                std::make_format_args(args...)
            );
            result.m_size      = writer.WrittenSize();
            result.m_truncated = writer.IsFull();
        } catch (...) {
            result = InplaceString{format_text};
        }
        return result;
    }

    /**
     * @brief Assign from a `std::string_view`, truncating on overflow.
     *
     * @param[in] text The text to store.
     *
     * @returns Reference to this string.
     *
     * @since 11.0.0
     */
    constexpr InplaceString& operator=(std::string_view text) noexcept
    {
        return *this = InplaceString{text};
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
    constexpr InplaceString& operator=(const char* text) noexcept
    {
        return *this = InplaceString{text};
    }

    /**
     * @brief Convert to a `std::string_view` over the stored text.
     *
     * The returned view borrows the stored text and is invalidated when the
     * string is destroyed or modified.
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
     * @brief Get the character at an index.
     *
     * @param[in] index The position to read, at most `Size()`.
     *
     * @returns The character at `index`, mirroring
     *          `std::string_view::operator[]`: the behavior is undefined
     *          when `index > Size()`; at `index == Size()` it yields the
     *          terminating null character.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr char operator[](std::size_t index) const noexcept
    {
        /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access) */
        return std::string_view{*this}[index];
    }

    /**
     * @brief Equality comparison against another `InplaceString`.
     *
     * @related InplaceString
     * @since 11.0.0
     */
    template <std::size_t OtherCapacityV>
    friend constexpr bool operator==(
        const InplaceString&                 lhs,
        const InplaceString<OtherCapacityV>& rhs
    ) noexcept
    {
        return std::string_view{lhs} == std::string_view{rhs};
    }

    /**
     * @brief Equality comparison against a `std::string_view`.
     *
     * @related InplaceString
     * @since 11.0.0
     */
    friend constexpr bool operator==(
        const InplaceString& lhs,
        std::string_view     rhs
    ) noexcept
    {
        return std::string_view{lhs} == rhs;
    }

    /**
     * @brief Three-way comparison against a `std::string_view`.
     *
     * @related InplaceString
     * @since 11.0.0
     */
    friend constexpr std::strong_ordering operator<=>(
        const InplaceString& lhs,
        std::string_view     rhs
    ) noexcept
    {
        return std::string_view{lhs} <=> rhs;
    }

    /**
     * @brief Three-way comparison against another `InplaceString`.
     *
     * @related InplaceString
     * @since 11.0.0
     */
    template <std::size_t OtherCapacityV>
    friend constexpr std::strong_ordering operator<=>(
        const InplaceString&                 lhs,
        const InplaceString<OtherCapacityV>& rhs
    ) noexcept
    {
        return std::string_view{lhs} <=> std::string_view{rhs};
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
 * @brief Stream the stored text of a `InplaceString` to an output stream.
 *
 * @param[in,out] output_stream Destination stream.
 * @param[in]     value         String to print.
 *
 * @returns Reference to `output_stream`.
 *
 * @related InplaceString
 * @since 11.0.0
 */
template <std::size_t CapacityV>
inline std::ostream& operator<<(
    std::ostream&                   output_stream,
    const InplaceString<CapacityV>& value
)
{
    return output_stream << std::string_view{value};
}

/**
 * @brief Print a `InplaceString` for GoogleTest diagnostics.
 * @ingroup magic_utility
 *
 * Found via ADL by GoogleTest when a `InplaceString` value is printed on
 * assertion failure.
 *
 * @param[in]  value          The string to print.
 * @param[out] output_stream  The stream to print to.
 *
 * @related InplaceString
 * @since 11.0.0
 */
template <std::size_t CapacityV>
inline void PrintTo(
    const InplaceString<CapacityV>& value,
    std::ostream*                   output_stream
)
{
    *output_stream << "\"" << std::string_view{value} << "\"";
}

/** @} magic_utility */
} /* namespace Recognition::Utility */

namespace std {
/**
 * @brief Format specialization making `Utility::InplaceString` usable with
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
struct formatter<Recognition::Utility::InplaceString<CapacityV>, char>
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
        const Recognition::Utility::InplaceString<CapacityV>& value,
        FormatContextT&                                       ctx
    ) const
    {
        return formatter<string_view>::format(std::string_view{value}, ctx);
    }
};

/**
 * @brief Hash specialization making `Utility::InplaceString` usable as an
 *        unordered container key.
 *
 * Delegates to the `std::string_view` hash, consistent with `operator==`:
 * equal texts hash equal across capacities.
 *
 * @tparam CapacityV Maximum number of inline characters of the string being
 *                   hashed.
 *
 * @since 11.0.0
 */
template <std::size_t CapacityV>
/* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
struct hash<Recognition::Utility::InplaceString<CapacityV>> {
    /**
     * @brief Hash a string.
     *
     * @param[in] value The string to hash.
     *
     * @returns The hash of the stored text.
     */
    [[nodiscard]] std::size_t operator()(
        const Recognition::Utility::InplaceString<CapacityV>& value
    ) const noexcept
    {
        return std::hash<std::string_view>{}(std::string_view{value});
    }
};
} /* namespace std */

#endif /* INPLACE_STRING_HPP */
