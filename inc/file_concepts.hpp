/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef FILE_CONCEPTS_HPP
#define FILE_CONCEPTS_HPP

#include <filesystem>

#include "utility.hpp"

namespace file_concepts {

/**
 * @brief The file_container concept specifies the requirements of
 *        a container which can be used as a file container by the magic class.
 */
template <typename ContainerType>
concept file_container
    = std::ranges::range<ContainerType>
   && std::default_initializable<ContainerType>
   && std::same_as<typename ContainerType::value_type, std::filesystem::path>
   && requires(ContainerType c, std::filesystem::path p) {
          c.push_back(p);
          c.empty();
          typename ContainerType::value_type;
      };

/**
 * @brief Convert the file container to a string.
 *
 * @param[in] container     The container that holds the paths of the files.
 * @param[in] separator     The separator between the paths of the files, default is ", ".
 *
 * @returns The container as a string.
 */
[[nodiscard]] inline std::string to_string(
    const file_container auto& container,
    const std::string&         separator = ", "
)
{
    return utility::to_string(
        container,
        separator,
        [](const std::filesystem::path& path) {
            return path.string();
        }
    );
}

} /* namespace file_concepts */

#endif /* FILE_CONCEPTS_HPP */
