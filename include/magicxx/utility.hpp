/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
#include <concepts>
#include <functional>
#include <ranges>
#include <string>

namespace utility {

/**
 * @brief Define requirements for a range container.
 * 
 * @tparam ContainerType        The type of the container.
 */
template <typename ContainerType>
concept range_container = std::ranges::range<ContainerType>
                       && requires(ContainerType c) {
                              typename ContainerType::value_type;
                          };

/**
 * @brief Define requirements for a string converter.
 * 
 * @tparam ValueType            The type of the value.
 * @tparam StringConverterType  The type of the string converter callable.
 */
template <typename ValueType, typename StringConverterType>
concept string_converter = std::same_as<
    std::invoke_result_t<StringConverterType, ValueType>,
    std::string>;

/**
 * @brief Convert any container to string using the string_converter.
 *
 * @tparam ContainerType         The type of the container.
 * @tparam StringConverterType   The type of the string converter callable.
 *
 * @param[in] container
 * @param[in] value_separator    The separator between the values of the container.
 * @param[in] string_converter   The callable that converts values of the container to string.
 * 
 * @returns The container as a string.
 */
template <range_container ContainerType, typename StringConverterType>
requires string_converter<
    typename ContainerType::value_type,
    StringConverterType>
[[nodiscard]] inline std::string to_string(
    const ContainerType& container,
    const std::string&   value_separator,
    StringConverterType  string_converter
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
            return left + value_separator
                 + std::invoke(string_converter, right);
        }
    );
}

} /* namespace utility */

#endif /* UTILITY_HPP */
