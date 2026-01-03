/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
#include <concepts>
#include <filesystem>
#include <functional>
#include <ranges>
#include <string>

namespace Recognition {
namespace Utility {
/**
 * @brief Define requirements for a range container.
 * 
 * @tparam ContainerT        The type of the container.
 */
template <typename ContainerT>
concept RangeContainer = std::ranges::range<ContainerT>
                      && requires(ContainerT c) {
                             typename ContainerT::value_type;
                         };

/**
 * @brief Define requirements for a string converter.
 * 
 * @tparam ValueT            The type of the value.
 * @tparam StringConverterT  The type of the string converter callable.
 */
template <typename ValueT, typename StringConverterT>
concept StringConverter = std::same_as<
    std::invoke_result_t<StringConverterT, ValueT>,
    std::string
>;

/**
 * @brief Convert any container to string using the StringConverter.
 *
 * @tparam ContainerT         The type of the container.
 * @tparam StringConverterT   The type of the string converter callable.
 *
 * @param[in] container
 * @param[in] value_separator    The separator between the values of the container.
 * @param[in] string_converter   The callable that converts values of the container to string.
 * 
 * @returns The container as a string.
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
 * @brief The FileContainer concept specifies the requirements of
 *        a container which can be used as a file container by the Magic class.
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
 * @brief Convert the file container to a string.
 *
 * @param[in] container     The container that holds the paths of the files.
 * @param[in] separator     The separator between the paths of the files, default is ", ".
 *
 * @returns The container as a string.
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
