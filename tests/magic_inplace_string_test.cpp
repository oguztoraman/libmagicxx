/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_inplace_string_test.cpp
 * @brief Unit tests for Utility::InplaceString.
 *
 * Verifies the allocation-free fixed-capacity string returned by
 * MagicFlags::ToString() and MagicParameters::ToString():
 * - Construction (default, from `const char*`, from a null pointer, from
 *   `std::string_view`, by copy, and by move)
 * - Value transfer by return from a callee and by move assignment
 * - Assignment (copy, `std::string_view`, and `const char*`)
 * - `MaxSize()` reporting the template capacity
 * - Observers (`Data()`, `CStr()`, `Size()`, `Empty()`) and NUL termination
 * - Element access (`operator[]`, `Front()`, `Back()`)
 * - Contiguous-range iteration (`begin()`, `end()`) with ranges algorithms
 * - `Clear()` resetting text and the truncation flag
 * - Truncation behavior and the `IsTruncated()` flag
 * - `Append()` growth, including boundary and overflow cases
 * - `Find()`, `Contains()`, `StartsWith()`, and `EndsWith()` searching
 * - `Format()` matching `std::format`, including rvalue arguments, bounded
 *   output, and the error fallback
 * - Comparison transparency against `const char*`, `std::string_view`,
 *   `std::string`, and other `InplaceString` instantiations
 * - Hashing consistent with equality, for unordered container keys
 * - Printing (`operator<<`, `PrintTo()`, and `std::format`)
 * - `noexcept` and constant-evaluation guarantees required by the
 *   `noexcept` API boundary
 *
 * @section inplace_string_test_cases Test Cases
 *
 * | Test Case | Description |
 * |-----------|-------------|
 * | max_size_reports_capacity | `MaxSize()` equals the template capacity |
 * | default_construction_is_empty | Default state is empty and NUL-terminated |
 * | construct_from_char_pointer | Null-terminated input is stored |
 * | construct_from_null_char_pointer | Null pointer yields empty text |
 * | construct_from_string_view | Viewed input is stored |
 * | construct_up_to_capacity | Exact-capacity input does not truncate |
 * | long_capacity_boundaries | Truncation works at production capacities |
 * | truncation_sets_flag | Overlong input is dropped and flagged |
 * | copy_constructor_copies_content | Copy is independent and keeps the flag |
 * | move_constructor_transfers_content | Container moves keep content and flag |
 * | return_from_callee_transfers_content | Returned strings keep their text |
 * | copy_assignment_replaces_state | Copy assignment replaces text and flag |
 * | move_assignment_replaces_state | Move assignment replaces text and flag |
 * | assignment_from_view_and_pointer | Both assignment overloads truncate |
 * | conversion_to_string_view | View aliases the inline buffer |
 * | data_and_cstr_agree | Observers report NUL-terminated text |
 * | append_keeps_text | `Append()` grows the stored text |
 * | append_fills_to_capacity_without_truncation | Boundary append is exact |
 * | append_after_full_drops_text | Appending to a full string truncates |
 * | element_access_reads_characters | `operator[]`, `Front()`, `Back()` read the text |
 * | iterates_as_contiguous_range | `begin()`/`end()` expose the characters |
 * | clear_resets_state | `Clear()` empties text and the truncation flag |
 * | find_and_contains_search_text | `Find()` and `Contains()` search the stored text |
 * | starts_with_and_ends_with_check_affixes | Prefix and suffix queries match `string_view` |
 * | hash_is_consistent_with_equality | Equal texts hash equal across capacities |
 * | equality_with_string_view_and_char_pointer | Equality is transparent |
 * | equality_across_capacities | Different capacities compare equal |
 * | ordering_against_string_view | Relational operators order as views |
 * | ordering_across_capacities | Different capacities order as views |
 * | format_produces_identical_text_to_std_format | `Format()` matches `std::format` |
 * | format_accepts_temporary_arguments | Rvalue arguments are formatted |
 * | format_without_arguments | A placeholder-free template is stored |
 * | format_invalid_template_keeps_template_text | Errors fall back to the template |
 * | format_truncates_instead_of_growing | Bounded output sets the flag |
 * | noexcept_assignment_in_noexcept_function | Mutators work in `noexcept` code |
 * | unexpected_construction_is_noexcept | Error payloads build without throwing |
 * | ostream_printing | `operator<<` prints the stored text |
 * | print_to_quotes_text | `PrintTo()` prints a quoted string |
 * | std_format_specialization | Usable with `std::format` |
 * | constant_evaluation_of_mutators | Construction and mutation are `constexpr` |
 *
 * @see Utility::InplaceString
 */

#include <gtest/gtest.h>

#include <compare>
#include <cstddef>
#include <expected>
#include <format>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "inplace_string.hpp"

using Recognition::Utility::InplaceString;

/**
 * @brief Fixture for Utility::InplaceString tests.
 *
 * Holds the shared capacities, string aliases, helper functions, and the
 * `noexcept` trait checks exercised by the `MagicInplaceStringTest` cases.
 */
struct MagicInplaceStringTest : testing::Test {
    /**
     * @brief Small capacity used throughout these tests.
     */
    static constexpr std::size_t TEST_CAPACITY = 32UZ;

    /**
     * @brief Capacity large enough to hold any production error message.
     */
    static constexpr std::size_t LONG_CAPACITY = 4'096UZ;

    /**
     * @brief Tiny capacity used to probe capacity boundaries.
     */
    static constexpr std::size_t SHORT_CAPACITY = 4UZ;

    /**
     * @brief Convenience alias exercised by these tests.
     */
    using TestStringT = InplaceString<TEST_CAPACITY>;

    /**
     * @brief Larger alias used where messages must not truncate.
     */
    using LongStringT = InplaceString<LONG_CAPACITY>;

    /**
     * @brief Boundary alias used for exact-capacity and overflow cases.
     */
    using ShortStringT = InplaceString<SHORT_CAPACITY>;

    /**
     * @brief Build text one character longer than @ref TEST_CAPACITY.
     *
     * @param[in] fill Character to repeat.
     *
     * @returns Overlong text used to exercise truncation.
     */
    static std::string OverlongText(char fill)
    {
        std::string text{};
        text.append(TEST_CAPACITY + 1UZ, fill);
        return text;
    }

    /**
     * @brief Build a string in a callee and hand it back by value.
     *
     * Returning by value is how the library produces every error message, so
     * this exercises the transfer of the inline buffer to the caller.
     *
     * @param[in] text Text to store, truncated when overlong.
     *
     * @returns The string built by the callee.
     */
    static TestStringT MakeStringInCallee(std::string_view text)
    {
        TestStringT result{text};
        return result;
    }

    /**
     * @brief Build a formatted string in a callee and hand it back.
     *
     * @param[in] format_text The format string.
     * @param[in] value       Value referenced by the format string.
     *
     * @returns The string formatted by the callee.
     */
    static TestStringT FormatInCallee(
        std::string_view format_text,
        std::string_view value
    )
    {
        return TestStringT::Format(format_text, value);
    }

    static_assert(
        std::is_nothrow_default_constructible_v<TestStringT>,
        "InplaceString must be noexcept default-constructible."
    );
    static_assert(
        std::is_nothrow_copy_constructible_v<TestStringT>,
        "InplaceString must be noexcept copy-constructible."
    );
    static_assert(
        std::is_nothrow_move_constructible_v<TestStringT>,
        "InplaceString must be noexcept move-constructible."
    );
    static_assert(
        std::is_nothrow_copy_assignable_v<TestStringT>,
        "InplaceString must be noexcept copy-assignable."
    );
    static_assert(
        std::is_nothrow_move_assignable_v<TestStringT>,
        "InplaceString must be noexcept move-assignable."
    );
    static_assert(
        std::is_nothrow_destructible_v<TestStringT>,
        "InplaceString must be noexcept destructible."
    );
    static_assert(
        std::is_trivially_copyable_v<TestStringT>,
        "InplaceString must stay relocatable so containers can move error"
        " payloads without allocation."
    );
    static_assert(
        std::is_nothrow_constructible_v<TestStringT, std::nullptr_t>,
        "InplaceString must be noexcept constructible from a null pointer."
    );
    static_assert(
        noexcept(std::declval<TestStringT&>() = std::string_view{}),
        "InplaceString must be noexcept assignable from string_view."
    );
    static_assert(
        noexcept(std::declval<TestStringT&>() = "literal"),
        "InplaceString must be noexcept assignable from const char*."
    );
    static_assert(
        noexcept(std::declval<TestStringT&>().Append(std::string_view{})),
        "Append must be noexcept."
    );
    static_assert(
        noexcept(TestStringT::Format(
            std::declval<std::string_view>(),
            std::declval<int>()
        )),
        "Format must be noexcept."
    );
    static_assert(
        noexcept(std::declval<const TestStringT&>().Data())
            && noexcept(std::declval<const TestStringT&>().CStr())
            && noexcept(std::declval<const TestStringT&>().Size())
            && noexcept(std::declval<const TestStringT&>().Empty())
            && noexcept(std::declval<const TestStringT&>().IsTruncated()),
        "Observers must be noexcept."
    );
};

TEST_F(MagicInplaceStringTest, max_size_reports_capacity)
{
    static_assert(TestStringT::MaxSize() == TEST_CAPACITY);
    static_assert(ShortStringT::MaxSize() < TestStringT::MaxSize());
    static_assert(LongStringT::MaxSize() > TestStringT::MaxSize());

    const TestStringT message{"0123456789"};
    EXPECT_GE(TestStringT::MaxSize(), message.Size());
}

TEST_F(MagicInplaceStringTest, default_construction_is_empty)
{
    constexpr TestStringT MESSAGE{};
    static_assert(MESSAGE.Empty());
    static_assert(MESSAGE.Size() == 0UZ);
    static_assert(!MESSAGE.IsTruncated());
    EXPECT_EQ(MESSAGE, "");
    EXPECT_EQ(std::string_view{MESSAGE}, "");
    EXPECT_TRUE(MESSAGE.Empty());
    EXPECT_STREQ(MESSAGE.CStr(), "");
    EXPECT_STREQ(MESSAGE.Data(), "");
    EXPECT_NE(MESSAGE.Data(), nullptr);
}

TEST_F(MagicInplaceStringTest, construct_from_char_pointer)
{
    constexpr TestStringT MESSAGE{"text/plain"};
    static_assert(MESSAGE.Size() == std::string_view{"text/plain"}.size());
    static_assert(!MESSAGE.IsTruncated());
    EXPECT_STREQ(MESSAGE.CStr(), "text/plain");
    EXPECT_EQ(std::string_view{MESSAGE}, "text/plain");
    EXPECT_FALSE(MESSAGE.Empty());
}

TEST_F(MagicInplaceStringTest, construct_from_null_char_pointer)
{
    constexpr const char* NULL_TEXT{nullptr};
    constexpr TestStringT MESSAGE{NULL_TEXT};
    static_assert(MESSAGE.Size() == 0UZ);
    static_assert(MESSAGE.Empty());
    static_assert(!MESSAGE.IsTruncated());
    EXPECT_STREQ(MESSAGE.CStr(), "");
    EXPECT_EQ(MESSAGE, "");

    const char*       runtime_null{nullptr};
    const TestStringT runtime_message{runtime_null};
    EXPECT_TRUE(runtime_message.Empty());
    EXPECT_FALSE(runtime_message.IsTruncated());
}

TEST_F(MagicInplaceStringTest, construct_from_string_view)
{
    const std::string text{"ASCII text"};
    const TestStringT message{std::string_view{text}};
    EXPECT_EQ(message, text);
    EXPECT_FALSE(message.IsTruncated());
    EXPECT_EQ(message.Size(), text.size());

    const TestStringT empty_message{std::string_view{}};
    EXPECT_TRUE(empty_message.Empty());
    EXPECT_EQ(empty_message, "");
}

TEST_F(MagicInplaceStringTest, construct_up_to_capacity)
{
    const std::string exact_text(TEST_CAPACITY, 'a');
    const TestStringT exact{std::string_view{exact_text}};
    EXPECT_FALSE(exact.IsTruncated());
    EXPECT_EQ(exact.Size(), TEST_CAPACITY);
    EXPECT_STREQ(exact.CStr(), exact_text.c_str());

    const ShortStringT oversized{std::string_view{"abcdef"}};
    EXPECT_TRUE(oversized.IsTruncated());
    EXPECT_EQ(oversized.Size(), ShortStringT::MaxSize());
    EXPECT_EQ(oversized, "abcd");
    EXPECT_STREQ(oversized.CStr(), "abcd");
}

TEST_F(MagicInplaceStringTest, long_capacity_boundaries)
{
    const std::string oversized_text(LongStringT::MaxSize() + 1UZ, 'L');
    const LongStringT constructed{std::string_view{oversized_text}};
    EXPECT_TRUE(constructed.IsTruncated());
    EXPECT_EQ(
        constructed,
        std::string_view{oversized_text}.substr(0, LongStringT::MaxSize())
    );

    const std::string full_text(LongStringT::MaxSize(), 'a');
    LongStringT       appended{std::string_view{full_text}};
    EXPECT_FALSE(appended.IsTruncated());
    appended.Append("b");
    EXPECT_TRUE(appended.IsTruncated());
    EXPECT_EQ(appended.Size(), LongStringT::MaxSize());

    const auto formatted = LongStringT::Format("{}", oversized_text);
    EXPECT_TRUE(formatted.IsTruncated());
    EXPECT_EQ(formatted.Size(), LongStringT::MaxSize());

    const char*       null_text{nullptr};
    const LongStringT empty{null_text};
    EXPECT_TRUE(empty.Empty());
    EXPECT_FALSE(empty.IsTruncated());
}

TEST_F(MagicInplaceStringTest, truncation_sets_flag)
{
    const std::string long_text{OverlongText('x')};
    const TestStringT message{std::string_view{long_text}};
    EXPECT_TRUE(message.IsTruncated());
    EXPECT_EQ(message.Size(), TEST_CAPACITY);
    EXPECT_EQ(
        std::string_view{message},
        std::string_view{long_text}.substr(0, TEST_CAPACITY)
    );

    TestStringT appender{"0123456789"};
    appender.Append(std::string(TEST_CAPACITY, '9'));
    EXPECT_TRUE(appender.IsTruncated());
    EXPECT_EQ(appender.Size(), TEST_CAPACITY);
}

TEST_F(MagicInplaceStringTest, copy_constructor_copies_content)
{
    const std::string long_text{OverlongText('c')};
    const TestStringT truncated{std::string_view{long_text}};
    const TestStringT copy{truncated};
    EXPECT_TRUE(copy.IsTruncated());
    EXPECT_EQ(copy.Size(), TEST_CAPACITY);
    EXPECT_EQ(copy, std::string_view{long_text}.substr(0, TEST_CAPACITY));

    const TestStringT original{"abc"};
    TestStringT       independent{original};
    independent.Append("def");
    EXPECT_EQ(independent, "abcdef");
    EXPECT_EQ(original, "abc");
}

TEST_F(MagicInplaceStringTest, move_constructor_transfers_content)
{
    static_assert(std::is_nothrow_move_constructible_v<TestStringT>);
    const std::string        long_text{OverlongText('v')};
    std::vector<TestStringT> storage{};
    storage.emplace_back(std::string_view{long_text});
    storage.emplace_back("moved value");
    const std::string_view truncated_prefix{
        std::string_view{long_text}.substr(0, TEST_CAPACITY)
    };
    storage.reserve(storage.size() * 4UZ);
    storage.emplace_back("grown");
    ASSERT_EQ(storage.size(), 3UZ);
    EXPECT_TRUE(storage[0].IsTruncated());
    EXPECT_EQ(storage[0].Size(), TEST_CAPACITY);
    EXPECT_EQ(storage[0], truncated_prefix);
    EXPECT_EQ(storage[1], "moved value");
    EXPECT_FALSE(storage[1].IsTruncated());
    EXPECT_EQ(storage[2], "grown");
    EXPECT_STREQ(storage[1].CStr(), "moved value");
}

TEST_F(MagicInplaceStringTest, return_from_callee_transfers_content)
{
    const TestStringT short_text{MakeStringInCallee("moved")};
    EXPECT_EQ(short_text, "moved");
    EXPECT_FALSE(short_text.IsTruncated());

    const std::string long_text{OverlongText('m')};
    const TestStringT truncated_text{
        MakeStringInCallee(std::string_view{long_text})
    };
    EXPECT_TRUE(truncated_text.IsTruncated());
    EXPECT_EQ(truncated_text.Size(), TEST_CAPACITY);
    EXPECT_EQ(
        truncated_text,
        std::string_view{long_text}.substr(0, TEST_CAPACITY)
    );
}

TEST_F(MagicInplaceStringTest, copy_assignment_replaces_state)
{
    const std::string long_text{OverlongText('C')};
    const TestStringT truncated_source{std::string_view{long_text}};

    TestStringT target{"short"};
    target = truncated_source;
    EXPECT_TRUE(target.IsTruncated());
    EXPECT_EQ(target.Size(), TEST_CAPACITY);

    const TestStringT plain{"reset"};
    target = plain;
    EXPECT_FALSE(target.IsTruncated());
    EXPECT_EQ(target, "reset");
    EXPECT_EQ(target.Size(), std::string_view{"reset"}.size());
}

TEST_F(MagicInplaceStringTest, move_assignment_replaces_state)
{
    TestStringT target{"stale"};
    target = TestStringT{"overwritten"};
    EXPECT_EQ(target, "overwritten");
    EXPECT_FALSE(target.IsTruncated());

    target = MakeStringInCallee("move assign");
    EXPECT_EQ(target, "move assign");
    EXPECT_FALSE(target.IsTruncated());
}

TEST_F(MagicInplaceStringTest, assignment_from_view_and_pointer)
{
    TestStringT message{"start"};
    message = std::string_view{"assigned"};
    EXPECT_EQ(message, "assigned");
    EXPECT_FALSE(message.IsTruncated());

    message = "re-assigned";
    EXPECT_EQ(message, "re-assigned");

    const std::string long_text{OverlongText('z')};
    message = std::string_view{long_text};
    EXPECT_TRUE(message.IsTruncated());
    EXPECT_EQ(message.Size(), TEST_CAPACITY);

    message = "short";
    EXPECT_FALSE(message.IsTruncated());
    EXPECT_EQ(message, "short");
}

TEST_F(MagicInplaceStringTest, conversion_to_string_view)
{
    constexpr TestStringT MESSAGE{"convert"};
    static_assert(MESSAGE.Size() == std::string_view{"convert"}.size());
    static_assert(std::string_view{MESSAGE}.size() == MESSAGE.Size());

    const std::string_view view{MESSAGE};
    EXPECT_EQ(view.data(), MESSAGE.Data());
    EXPECT_EQ(view, "convert");

    const ShortStringT     truncated{std::string_view{"abcdefgh"}};
    const std::string_view truncated_view{truncated};
    EXPECT_EQ(truncated_view.data(), truncated.Data());
    EXPECT_EQ(truncated_view.size(), truncated.Size());
    EXPECT_EQ(truncated_view, "abcd");
}

TEST_F(MagicInplaceStringTest, data_and_cstr_agree)
{
    const TestStringT message{"text/plain"};
    EXPECT_STREQ(message.Data(), "text/plain");
    EXPECT_STREQ(message.CStr(), message.Data());
    EXPECT_EQ(message.Data(), message.CStr());
    EXPECT_EQ(std::char_traits<char>::length(message.Data()), message.Size());
    EXPECT_EQ(std::char_traits<char>::length(message.CStr()), message.Size());

    const ShortStringT truncated{std::string_view{"abcdef"}};
    EXPECT_STREQ(truncated.Data(), "abcd");
    EXPECT_EQ(
        std::char_traits<char>::length(truncated.CStr()),
        truncated.Size()
    );
}

TEST_F(MagicInplaceStringTest, append_keeps_text)
{
    TestStringT message{"ASCII"};
    message.Append(" text");
    EXPECT_EQ(message, "ASCII text");
    message.Append(std::string_view{" file"});
    EXPECT_EQ(message, "ASCII text file");
    EXPECT_FALSE(message.IsTruncated());

    const std::size_t size_before{message.Size()};
    message.Append(std::string_view{});
    EXPECT_EQ(message.Size(), size_before);
    EXPECT_FALSE(message.IsTruncated());
}

TEST_F(MagicInplaceStringTest, append_fills_to_capacity_without_truncation)
{
    ShortStringT message{"ab"};
    message.Append("cd");
    EXPECT_FALSE(message.IsTruncated());
    EXPECT_EQ(message, "abcd");
    EXPECT_EQ(message.Size(), ShortStringT::MaxSize());
}

TEST_F(MagicInplaceStringTest, append_after_full_drops_text)
{
    ShortStringT message{"abcd"};
    message.Append("e");
    EXPECT_TRUE(message.IsTruncated());
    EXPECT_EQ(message, "abcd");
    EXPECT_EQ(message.Size(), ShortStringT::MaxSize());
}

TEST_F(MagicInplaceStringTest, equality_with_string_view_and_char_pointer)
{
    constexpr TestStringT MESSAGE{"text/plain; charset=us-ascii"};
    static_assert(MESSAGE == "text/plain; charset=us-ascii");
    static_assert(MESSAGE == std::string_view{"text/plain; charset=us-ascii"});
    static_assert("text/plain; charset=us-ascii" == MESSAGE);
    EXPECT_EQ(MESSAGE, std::string{"text/plain; charset=us-ascii"});
    EXPECT_NE(MESSAGE, "text/plain");
    EXPECT_NE(std::string{"text/plain"}, MESSAGE);
}

TEST_F(MagicInplaceStringTest, equality_across_capacities)
{
    EXPECT_EQ(ShortStringT{"abcd"}, TestStringT{"abcd"});
    EXPECT_EQ(TestStringT{"abcd"}, ShortStringT{"abcd"});
    EXPECT_NE(ShortStringT{"abcd"}, TestStringT{"abc"});
    EXPECT_NE(TestStringT{"abcde"}, ShortStringT{"abcd"});
}

TEST_F(MagicInplaceStringTest, ordering_against_string_view)
{
    EXPECT_LT(TestStringT{"a"}, std::string_view{"b"});
    EXPECT_GT(TestStringT{"b"}, std::string_view{"a"});
    EXPECT_LT(TestStringT{"a"}, "b");
    EXPECT_GE(TestStringT{"b"}, "a");
    EXPECT_LE(ShortStringT{"abcd"}, "abcd");
    EXPECT_EQ(
        TestStringT{"a"} <=> std::string_view{"a"},
        std::strong_ordering::equal
    );
    EXPECT_EQ(
        TestStringT{"b"} <=> std::string_view{"a"},
        std::strong_ordering::greater
    );

    const TestStringT first{"11.0.0"};
    const TestStringT second{"11.0.1"};
    EXPECT_LT(std::string_view{first}, std::string_view{second});
}

TEST_F(MagicInplaceStringTest, format_produces_identical_text_to_std_format)
{
    const std::string path{"/tmp/nonexistent file.txt"};
    const auto static_text = LongStringT::Format("'{}' does not exist.", path);
    const std::string std_text = std::format("'{}' does not exist.", path);
    EXPECT_EQ(std::string_view{static_text}, std_text);
    EXPECT_FALSE(static_text.IsTruncated());

    const std::string reason{"no such file or directory"};
    const auto        quoted = LongStringT::Format(
        "Magic::IdentifyFile({}) failed with {}.",
        path,
        reason
    );
    EXPECT_EQ(
        std::string_view{quoted},
        std::format("Magic::IdentifyFile({}) failed with {}.", path, reason)
    );

    EXPECT_EQ(TestStringT::Format("code: {}", 42), "code: 42");
    EXPECT_EQ(TestStringT::Format("{}-{}-{}", "a", 1, true), "a-1-true");

    const auto returned = FormatInCallee("'{}'", "by value");
    EXPECT_EQ(returned, "'by value'");
    EXPECT_FALSE(returned.IsTruncated());
}

TEST_F(MagicInplaceStringTest, format_accepts_temporary_arguments)
{
    const std::string path{"/tmp/x"};
    const auto        temporary_string_argument = LongStringT::Format(
        "'{}' does not exist.",
        std::string{path}
    );
    const auto expected = std::format(
        "'{}' does not exist.",
        std::string{path}
    );
    EXPECT_EQ(std::string_view{temporary_string_argument}, expected);
    EXPECT_FALSE(temporary_string_argument.IsTruncated());

    const auto temporary_view_argument = TestStringT::Format(
        "{}",
        std::string_view{"view"}
    );
    EXPECT_EQ(temporary_view_argument, "view");

    const auto number_argument = TestStringT::Format("{}%", 42);
    EXPECT_EQ(number_argument, "42%");
    EXPECT_FALSE(number_argument.IsTruncated());
}

TEST_F(MagicInplaceStringTest, format_without_arguments)
{
    const TestStringT message = TestStringT::Format("no placeholders");
    EXPECT_EQ(message, "no placeholders");
    EXPECT_FALSE(message.IsTruncated());
    EXPECT_EQ(message.Size(), std::string_view{"no placeholders"}.size());
}

TEST_F(MagicInplaceStringTest, format_invalid_template_keeps_template_text)
{
    const TestStringT unterminated = TestStringT::Format("broken {");
    EXPECT_EQ(unterminated, "broken {");
    EXPECT_STREQ(unterminated.CStr(), "broken {");
    EXPECT_FALSE(unterminated.IsTruncated());

    const TestStringT missing_argument = TestStringT::Format("no argument {}");
    EXPECT_EQ(missing_argument, "no argument {}");
    EXPECT_STREQ(missing_argument.CStr(), "no argument {}");

    const ShortStringT partial = ShortStringT::Format("ab{}");
    EXPECT_EQ(partial, "ab{}");
    EXPECT_STREQ(partial.CStr(), "ab{}");
    EXPECT_FALSE(partial.IsTruncated());

    const TestStringT bad_option = TestStringT::Format("{:d}", "text");
    EXPECT_EQ(bad_option, "{:d}");
    EXPECT_STREQ(bad_option.CStr(), "{:d}");
}

TEST_F(MagicInplaceStringTest, format_truncates_instead_of_growing)
{
    const std::string long_path(TEST_CAPACITY * 2UZ, 'p');
    const TestStringT message = TestStringT::Format(
        "'{}' does not exist.",
        long_path
    );
    EXPECT_TRUE(message.IsTruncated());
    EXPECT_EQ(message.Size(), TEST_CAPACITY);
    EXPECT_EQ(
        std::string_view{message},
        std::format("'{}' does not exist.", long_path).substr(0, TEST_CAPACITY)
    );

    const ShortStringT exact = ShortStringT::Format("{}cd", "ab");
    EXPECT_FALSE(exact.IsTruncated());
    EXPECT_EQ(exact, "abcd");

    const ShortStringT overflow = ShortStringT::Format("{}cde", "ab");
    EXPECT_TRUE(overflow.IsTruncated());
    EXPECT_EQ(overflow, "abcd");
    EXPECT_EQ(overflow.Size(), ShortStringT::MaxSize());
}

TEST_F(MagicInplaceStringTest, noexcept_assignment_in_noexcept_function)
{
    const auto build = []() noexcept {
        TestStringT message;
        message = std::string_view{"assigned"};
        message = "re-assigned";
        message = TestStringT::Format("{}-{}", "formatted", 1);
        message.Append("!");
        return message;
    }();
    EXPECT_EQ(build, "formatted-1!");
}

TEST_F(MagicInplaceStringTest, unexpected_construction_is_noexcept)
{
    const auto make_error = []() noexcept {
        return std::unexpected{TestStringT::Format("{}: {}", "err", 1)};
    }();
    EXPECT_EQ(make_error.error(), "err: 1");

    const auto make_truncated_error = []() noexcept {
        return std::unexpected{ShortStringT::Format("path: {}", "pppp")};
    }();
    EXPECT_TRUE(make_truncated_error.error().IsTruncated());
    EXPECT_EQ(make_truncated_error.error(), "path");
}

TEST_F(MagicInplaceStringTest, ostream_printing)
{
    const TestStringT  message{"print me"};
    std::ostringstream stream;
    stream << message;
    EXPECT_EQ(stream.str(), "print me");

    const std::string  long_text{OverlongText('o')};
    const TestStringT  truncated_message{std::string_view{long_text}};
    std::ostringstream truncated_stream;
    truncated_stream << truncated_message;
    EXPECT_EQ(truncated_stream.str(), std::string(TEST_CAPACITY, 'o'));
}

TEST_F(MagicInplaceStringTest, print_to_quotes_text)
{
    const TestStringT  message{"quoted"};
    std::ostringstream stream;
    Recognition::Utility::PrintTo(message, &stream);
    EXPECT_EQ(stream.str(), "\"quoted\"");

    std::ostringstream empty_stream;
    Recognition::Utility::PrintTo(TestStringT{}, &empty_stream);
    EXPECT_EQ(empty_stream.str(), "\"\"");

    std::ostringstream boundary_stream;
    Recognition::Utility::PrintTo(ShortStringT{"abcdef"}, &boundary_stream);
    EXPECT_EQ(boundary_stream.str(), "\"abcd\"");

    std::ostringstream long_stream;
    Recognition::Utility::PrintTo(LongStringT{"long message"}, &long_stream);
    EXPECT_EQ(long_stream.str(), "\"long message\"");
}

TEST_F(MagicInplaceStringTest, std_format_specialization)
{
    const TestStringT message{"formatted"};
    EXPECT_EQ(std::format("[{}]", message), "[formatted]");
    EXPECT_EQ(std::format("{:>12}", TestStringT{"x"}), "           x");
}

TEST_F(MagicInplaceStringTest, constant_evaluation_of_mutators)
{
    constexpr auto BUILT = []() noexcept {
        TestStringT text{"abc"};
        text.Append("defg");
        text = "xyz";
        text.Append("!");
        return text;
    }();
    static_assert(BUILT == "xyz!");
    static_assert(!BUILT.IsTruncated());
    static_assert(BUILT.Size() == std::string_view{"xyz!"}.size());

    constexpr auto TRUNCATED = []() noexcept {
        ShortStringT text{"ab"};
        text.Append("cdefgh");
        return text;
    }();
    static_assert(TRUNCATED.IsTruncated());
    static_assert(TRUNCATED == "abcd");

    EXPECT_EQ(BUILT, "xyz!");
    EXPECT_EQ(TRUNCATED, "abcd");
}

TEST_F(MagicInplaceStringTest, find_and_contains_search_text)
{
    constexpr TestStringT MESSAGE{"text/plain; charset=us-ascii"};

    static_assert(MESSAGE.Find("charset") == 12UZ);
    static_assert(MESSAGE.Contains("charset=us-ascii"));
    static_assert(!MESSAGE.Contains("binary"));
    static_assert(MESSAGE.Find("zzz") == std::string_view::npos);

    static_assert(MESSAGE.Find("") == 0UZ);
    static_assert(MESSAGE.Contains(""));

    const ShortStringT truncated{std::string_view{"abcdefgh"}};
    EXPECT_TRUE(truncated.Contains("abcd"));
    EXPECT_FALSE(truncated.Contains("abcde"));
    EXPECT_EQ(truncated.Find("abcd"), 0UZ);

    const TestStringT empty{};
    EXPECT_EQ(empty.Find("a"), std::string_view::npos);
    EXPECT_FALSE(empty.Contains("a"));
}

TEST_F(MagicInplaceStringTest, ordering_across_capacities)
{
    EXPECT_LT(ShortStringT{"ab"}, TestStringT{"abc"});
    EXPECT_GT(TestStringT{"abd"}, ShortStringT{"abcd"});
    EXPECT_EQ(
        ShortStringT{"abcd"} <=> TestStringT{"abcd"},
        std::strong_ordering::equal
    );
    EXPECT_LE(TestStringT{"a"}, ShortStringT{"ab"});
    EXPECT_GE(TestStringT{"b"}, ShortStringT{"ab"});

    constexpr auto ORDERED = []() noexcept {
        return InplaceString<8>{"abc"} < InplaceString<16>{"abd"};
    }();
    static_assert(ORDERED);

    EXPECT_GT(
        TestStringT{"zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"},
        LongStringT{"a"}
    );
}

TEST_F(MagicInplaceStringTest, element_access_reads_characters)
{
    constexpr TestStringT MESSAGE{"text/plain"};

    static_assert(MESSAGE[0] == 't');
    static_assert(MESSAGE[4] == '/');
    static_assert(MESSAGE.Front() == 't');
    static_assert(MESSAGE.Back() == 'n');

    EXPECT_EQ(MESSAGE[MESSAGE.Size() - 1UZ], 'n');
    EXPECT_EQ(MESSAGE.Front(), MESSAGE[0]);
    EXPECT_EQ(MESSAGE.Back(), MESSAGE[MESSAGE.Size() - 1UZ]);

    const ShortStringT truncated{std::string_view{"abcdef"}};
    EXPECT_EQ(truncated[3], 'd');
    EXPECT_EQ(truncated.Size(), 4UZ);
}

TEST_F(MagicInplaceStringTest, iterates_as_contiguous_range)
{
    static_assert(std::ranges::contiguous_range<TestStringT>);
    static_assert(std::ranges::contiguous_range<LongStringT>);

    const TestStringT message{"abc"};

    EXPECT_EQ(std::ranges::distance(message), 3);
    EXPECT_EQ(message.begin(), message.Data());
    EXPECT_EQ(message.end(), message.Data() + message.Size());

    std::string collected{};
    std::ranges::copy(message, std::back_inserter(collected));
    EXPECT_EQ(collected, "abc");

    const std::string_view  view{message};
    const std::vector<char> from_iterators{message.begin(), message.end()};
    const std::vector<char> from_view{view.begin(), view.end()};
    EXPECT_EQ(from_view, from_iterators);

    const ShortStringT truncated{std::string_view{"abcdefgh"}};
    EXPECT_EQ(std::ranges::distance(truncated), 4);

    const TestStringT empty{};
    EXPECT_EQ(empty.begin(), empty.end());
    EXPECT_EQ(std::ranges::distance(empty), 0);

    constexpr auto SUM = []() noexcept {
        InplaceString<8> digits{"1234"};
        int              total{};
        for (const char digit : digits) {
            total += digit - '0';
        }
        return total;
    }();
    static_assert(SUM == 10);
}

TEST_F(MagicInplaceStringTest, clear_resets_state)
{
    ShortStringT truncated{std::string_view{"abcdefgh"}};
    ASSERT_TRUE(truncated.IsTruncated());

    truncated.Clear();
    EXPECT_TRUE(truncated.Empty());
    EXPECT_FALSE(truncated.IsTruncated());
    EXPECT_EQ(truncated, "");
    EXPECT_STREQ(truncated.CStr(), "");

    truncated.Append("xy");
    EXPECT_EQ(truncated, "xy");
    EXPECT_FALSE(truncated.IsTruncated());

    TestStringT empty{};
    empty.Clear();
    EXPECT_TRUE(empty.Empty());
    EXPECT_FALSE(empty.IsTruncated());

    constexpr auto CLEARED = []() noexcept {
        TestStringT text{"reset me"};
        text.Clear();
        text.Append("fresh");
        return text;
    }();
    static_assert(CLEARED == "fresh" && !CLEARED.IsTruncated());
}

TEST_F(MagicInplaceStringTest, starts_with_and_ends_with_check_affixes)
{
    constexpr TestStringT MESSAGE{"text/plain; charset=us-ascii"};

    static_assert(MESSAGE.StartsWith("text/plain"));
    static_assert(MESSAGE.EndsWith("us-ascii"));
    static_assert(!MESSAGE.StartsWith("application"));
    static_assert(!MESSAGE.EndsWith("binary"));

    static_assert(MESSAGE.StartsWith(""));
    static_assert(MESSAGE.EndsWith(""));
    static_assert(TestStringT{}.StartsWith(""));

    const TestStringT short_text{"ab"};
    EXPECT_FALSE(short_text.StartsWith("abc"));
    EXPECT_FALSE(short_text.EndsWith("abc"));
    EXPECT_TRUE(short_text.StartsWith("ab"));
    EXPECT_TRUE(short_text.EndsWith("ab"));

    const ShortStringT truncated{std::string_view{"abcdefgh"}};
    EXPECT_TRUE(truncated.StartsWith("abcd"));
    EXPECT_FALSE(truncated.EndsWith("efgh"));
}

TEST_F(MagicInplaceStringTest, hash_is_consistent_with_equality)
{
    const TestStringT a{"text/plain"};
    const TestStringT b{"text/plain"};
    const LongStringT same_text_long_capacity{"text/plain"};
    const TestStringT different{"text/html"};

    EXPECT_TRUE(a == b);
    EXPECT_EQ(std::hash<TestStringT>{}(a), std::hash<TestStringT>{}(b));

    EXPECT_EQ(
        std::hash<TestStringT>{}(a),
        std::hash<LongStringT>{}(same_text_long_capacity)
    );

    const ShortStringT truncated_full{std::string_view{"abcd"}};
    const ShortStringT truncated_cut{std::string_view{"abcdefgh"}};
    EXPECT_EQ(truncated_full, truncated_cut);
    EXPECT_EQ(
        std::hash<ShortStringT>{}(truncated_full),
        std::hash<ShortStringT>{}(truncated_cut)
    );

    std::unordered_map<TestStringT, int> counts{};
    counts[a] += 1;
    counts[b] += 1;
    counts[different] += 1;
    ASSERT_EQ(counts.size(), 2UZ);
    EXPECT_EQ(counts[TestStringT{"text/plain"}], 2);
}
