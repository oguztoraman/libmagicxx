/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <ranges>
#include <concepts>

namespace utility {

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
template <typename ContainerType, typename StringConverterType>
requires std::ranges::range<ContainerType> && requires (ContainerType c){c.empty(); typename ContainerType::value_type;}
[[nodiscard]]
inline std::string to_string(
    const ContainerType& container,
    const std::string& value_separator, StringConverterType string_converter)
{
    static_assert(
        std::same_as<std::invoke_result_t<StringConverterType, typename ContainerType::value_type>, std::string>,
        "StringConverterType must return std::string"
    );
    return container | std::views::transform(string_converter)
                     | std::views::join_with(value_separator)
                     | std::ranges::to<std::string>();
}

} /* namespace utility */

#endif /* UTILITY_HPP */
