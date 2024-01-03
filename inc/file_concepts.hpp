/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef FILE_CONCEPTS_HPP
#define FILE_CONCEPTS_HPP

#include <algorithm>
#include <filesystem>

/**
 * @brief The file_container concept specifies the requirements of
 *        a container which can be used as a file container by the magic class.
 */
template <typename ContainerType>
concept file_container =
        requires (ContainerType c, std::filesystem::path p){
            c.push_back(p);
            typename ContainerType::value_type;
        } &&
        std::ranges::range<ContainerType> &&
        std::default_initializable<ContainerType> &&
        std::same_as<typename ContainerType::value_type, std::filesystem::path>;

/**
 * @brief The error_container concept specifies the requirements of
 *        a container which can be used as a error container by the magic class.
 */
template <typename ContainerType>
concept error_container =
        requires (ContainerType c, std::string s){
            c.clear();
            c.push_back(s);
            typename ContainerType::value_type;
        } &&
        std::ranges::range<ContainerType> &&
        std::same_as<typename ContainerType::value_type, std::string>;

/**
 * @brief Operator<< overload for the file or the error containers.
 *
 * @tparam ContainerType    The type of the container.
 *
 * @param[out] os           The output stream.
 * @param[in]  container    The container that holds the paths of the files or the errors.
 *
 * @returns os.
 */
template <typename ContainerType>
requires file_container<ContainerType> || error_container<ContainerType>
inline std::ostream& operator<<(std::ostream& os, const ContainerType& container)
{
    std::ranges::for_each(container,
        [&](const auto& file_or_error){
            os << file_or_error << "\n";
        }
    );
    return os;
}

#endif /* FILE_CONCEPTS_HPP */
