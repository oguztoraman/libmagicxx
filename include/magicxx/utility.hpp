/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file utility.hpp
 * @brief Utility concepts, classes, and functions for the Magic library.
 *
 * This file provides utility components used throughout the libmagicxx library:
 * - Concepts for constraining template parameters
 * - General-purpose string conversion functions
 * - Progress tracking utilities
 *
 * @defgroup magic_utility Utility Components
 * @brief Supporting utilities for the libmagicxx library.
 *
 * The utility module provides foundational components used by the Magic class
 * and also available for direct use by library consumers.
 *
 * ### Concepts
 *
 * | Concept | Description |
 * |---------|-------------|
 * | `RangeContainer` | Types satisfying `std::ranges::range` with `value_type` |
 * | `StringConverter` | Callables converting a value to `std::string` |
 * | `FileContainer` | Containers of `std::filesystem::path` with `push_back()` |
 *
 * ### Classes
 *
 * | Class | Description |
 * |-------|-------------|
 * | `Percentage` | Represents a percentage value (0-100%) |
 * | `ProgressTracker` | Thread-safe progress monitoring |
 *
 * ### Functions
 *
 * | Function | Description |
 * |----------|-------------|
 * | `ToString()` | Convert containers to strings with custom separator |
 * | `MakeSharedProgressTracker()` | Factory for shared progress trackers |
 *
 * @see Recognition::Utility namespace
 * @see Recognition::Magic::ProgressTrackerT
 */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
#include <concepts>
#include <filesystem>
#include <functional>
#include <ranges>
#include <string>

namespace Recognition {
/**
 * @namespace Recognition::Utility
 * @ingroup magic_utility
 *
 * @brief Utility components for the libmagicxx library.
 *
 * The Utility namespace provides supporting types, concepts, and functions
 * used throughout the libmagicxx library. These utilities are also available
 * for use by consumers of the library.
 *
 * ### Key Components
 *
 * | Component | Description |
 * |-----------|-------------|
 * | **Concepts** | `RangeContainer`, `StringConverter`, `FileContainer` |
 * | **Classes** | `Percentage`, `ProgressTracker` |
 * | **Functions** | `ToString()`, `MakeSharedProgressTracker()` |
 * | **RAII Helpers** | `MarkTrackerAsCompleted`, `AdvanceTracker` |
 *
 * ### Usage Example
 *
 * @code{.cpp}
 * using namespace Recognition;
 *
 * // Progress tracking
 * auto tracker = Utility::MakeSharedProgressTracker(100);
 * tracker->Advance();
 * std::println("Progress: {}", tracker->GetCompletionPercentage().ToString());
 *
 * // Container to string conversion
 * std::vector<int> values = {1, 2, 3};
 * auto str = Utility::ToString(values, ", ", [](int x) {
 *     return std::to_string(x);
 * });
 * @endcode
 *
 * @see Percentage
 * @see ProgressTracker
 * @see MakeSharedProgressTracker()
 *
 * @since 10.0.0
 */
namespace Utility {
/**
 * @concept RangeContainer
 * @ingroup magic_utility
 *
 * @brief Concept for types that are ranges with an accessible value_type.
 *
 * A type satisfies RangeContainer if:
 * - It satisfies `std::ranges::range`
 * - It has a nested `value_type` type alias
 *
 * @tparam ContainerT The container type to check.
 *
 * @section range_container_example Example Types
 *
 * Types that satisfy this concept:
 * - `std::vector<T>`
 * - `std::list<T>`
 * - `std::set<T>`
 * - `std::array<T, N>`
 *
 * @code{.cpp}
 * static_assert(RangeContainer<std::vector<int>>);
 * static_assert(RangeContainer<std::list<std::string>>);
 * @endcode
 *
 * @since 10.0.0
 */
template <typename ContainerT>
concept RangeContainer = std::ranges::range<ContainerT>
                      && requires(ContainerT c) {
                             typename ContainerT::value_type;
                         };

/**
 * @concept StringConverter
 * @ingroup magic_utility
 *
 * @brief Concept for callables that convert a value to std::string.
 *
 * A callable satisfies StringConverter if it can be invoked with a value
 * of type ValueT and returns std::string.
 *
 * @tparam ValueT           The type of value to convert.
 * @tparam StringConverterT The callable type.
 *
 * @section string_converter_example Example
 *
 * @code{.cpp}
 * // Lambda satisfying StringConverter<int>
 * auto converter = [](int x) { return std::to_string(x); };
 * static_assert(StringConverter<int, decltype(converter)>);
 *
 * // Function pointer
 * std::string intToString(int x);
 * static_assert(StringConverter<int, decltype(&intToString)>);
 * @endcode
 *
 * @since 10.0.0
 */
template <typename ValueT, typename StringConverterT>
concept StringConverter = std::same_as<
    std::invoke_result_t<StringConverterT, ValueT>,
    std::string
>;

/**
 * @brief Convert a container to a string using a custom converter.
 * @ingroup magic_utility
 *
 * Converts each element in the container to a string using the provided
 * converter function, then joins them with the specified separator.
 *
 * @tparam ContainerT       Container type satisfying RangeContainer.
 * @tparam StringConverterT Callable type satisfying StringConverter.
 *
 * @param[in] container        The container to convert.
 * @param[in] value_separator  Separator string between elements.
 * @param[in] string_converter Callable that converts each element to string.
 *
 * @return All elements joined as a string, or empty string if container is empty.
 *
 * @code{.cpp}
 * std::vector<int> numbers = {1, 2, 3};
 * auto result = ToString(numbers, ", ", [](int x) {
 *     return std::to_string(x);
 * });
 * // result == "1, 2, 3"
 * @endcode
 *
 * @since 10.0.0
 */
template <RangeContainer ContainerT, typename StringConverterT>
requires StringConverter<typename ContainerT::value_type, StringConverterT>
[[nodiscard]] inline std::string ToString(
    const ContainerT&  container,
    const std::string& value_separator,
    StringConverterT   string_converter
)
{
    if (container.empty()) {
        return {};
    }
    return std::ranges::fold_left(
        std::ranges::next(std::ranges::begin(container)),
        std::ranges::end(container),
        std::invoke(string_converter, *std::ranges::begin(container)),
        [&](const auto& left, const auto& right) {
            return left
                 + value_separator
                 + std::invoke(string_converter, right);
        }
    );
}

/**
 * @concept FileContainer
 * @ingroup magic_utility
 *
 * @brief Concept for containers that can hold filesystem paths.
 *
 * A type satisfies FileContainer if:
 * - It satisfies `std::ranges::range`
 * - It is default-constructible
 * - Its `value_type` is `std::filesystem::path`
 * - It supports `push_back()` and `empty()` operations
 *
 * @tparam ContainerT The container type to check.
 *
 * @section file_container_types Satisfying Types
 *
 * - `std::vector<std::filesystem::path>`
 * - `std::list<std::filesystem::path>`
 * - `std::deque<std::filesystem::path>`
 *
 * @section file_container_usage Usage with Magic
 *
 * @code{.cpp}
 * std::vector<std::filesystem::path> files = {
 *     "/path/to/file1.txt",
 *     "/path/to/file2.png"
 * };
 *
 * // Works because vector<path> satisfies FileContainer
 * auto results = magic.IdentifyFiles(files);
 * @endcode
 *
 * @see Magic::IdentifyFiles()
 *
 * @since 10.0.0
 */
template <typename ContainerT>
concept FileContainer = std::ranges::range<ContainerT>
                     && std::default_initializable<ContainerT>
                     && std::same_as<
                            typename ContainerT::value_type,
                            std::filesystem::path
                     >
                     && requires(ContainerT c, std::filesystem::path p) {
                            c.push_back(p);
                            c.empty();
                            typename ContainerT::value_type;
                        };

/**
 * @brief Convert a FileContainer to a string.
 * @ingroup magic_utility
 *
 * Converts each path in the container to its string representation
 * and joins them with the specified separator.
 *
 * @param[in] container The container of file paths to convert.
 * @param[in] separator Separator string between paths (default: ", ").
 *
 * @return All paths joined as a string, or empty string if container is empty.
 *
 * @code{.cpp}
 * std::vector<std::filesystem::path> files = {
 *     "/path/to/file1.txt",
 *     "/path/to/file2.png"
 * };
 * std::string result = ToString(files);
 * // result == "/path/to/file1.txt, /path/to/file2.png"
 *
 * std::string newlineSeparated = ToString(files, "\n");
 * // Each path on its own line
 * @endcode
 *
 * @see FileContainer
 *
 * @since 10.0.0
 */
[[nodiscard]] inline std::string ToString(
    const FileContainer auto& container,
    const std::string&        separator = ", "
)
{
    return ToString(
        container,
        separator,
        [](const std::filesystem::path& path) {
            return path.string();
        }
    );
}
} /* namespace Utility */
} /* namespace Recognition */

#endif /* UTILITY_HPP */
