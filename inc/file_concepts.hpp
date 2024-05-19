/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef FILE_CONCEPTS_HPP
#define FILE_CONCEPTS_HPP

#include <string>
#include <algorithm>
#include <filesystem>

namespace file_concepts {

/**
 * @brief The file_container concept specifies the requirements of
 *        a container which can be used as a file container by the magic class.
 */
template <typename ContainerType>
concept file_container =
        requires (ContainerType c, std::filesystem::path p){
            c.push_back(p);
            c.empty();
            c.front();
            typename ContainerType::value_type;
        } &&
        std::ranges::range<ContainerType> &&
        std::default_initializable<ContainerType> &&
        std::same_as<typename ContainerType::value_type, std::filesystem::path>;

/**
 * @brief Convert the file container to a string.
 *
 * @param[in] container     The container that holds the paths of the files.
 * @param[in] separator     The separator between the paths of the files, default is ", ".
 *
 * @returns The container as a string.
 */
inline std::string to_string(const file_container auto& container, const std::string& separator = ", ")
{
    if (container.empty()){
        return {};
    }
    return std::ranges::fold_left(
        std::ranges::next(std::ranges::begin(container)),
        std::ranges::end(container),
        container.front().string(),
        [&](const auto& left, const auto& right){
            return left + separator + right.string();
        }
    );
}

} /* namespace file_concepts */

#endif /* FILE_CONCEPTS_HPP */
