/* SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef FILE_CONCEPTS_HPP
#define FILE_CONCEPTS_HPP

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
            typename ContainerType::value_type;
        } &&
        std::ranges::range<ContainerType> &&
        std::default_initializable<ContainerType> &&
        std::same_as<typename ContainerType::value_type, std::filesystem::path>;

/**
 * @brief Operator<< overload for the file containers.
 *
 * @param[out] os           The output stream.
 * @param[in]  container    The container that holds the paths of the files.
 *
 * @returns os.
 */
inline std::ostream& operator<<(std::ostream& os, const file_container auto& container)
{
    std::ranges::for_each(container,
        [&](const auto& file){
            os << file << "\n";
        }
    );
    return os;
}

} /* namespace file_concepts */

#endif /* FILE_CONCEPTS_HPP */
