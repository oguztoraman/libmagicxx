/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef PERCENTAGE_HPP
#define PERCENTAGE_HPP

#include <algorithm>
#include <cstdint>
#include <string>

namespace recognition {
namespace utility {
/**
 * @class percentage
 * 
 * @brief Represents a value in the range [0, 100].
 */
class percentage {
public:
    /**
     * @brief Construct percentage with a given value.
     * 
     * @param percentage            Value in range [0, 100]. Clamped if out of range. Default is 0.
     */
    explicit percentage(int percentage = 0) noexcept
      : m_percentage{std::clamp(percentage, 0, 100)}
    { }

    /**
     * @brief Construct percentage from completed and total steps.
     *
     * @param completed_steps       Number of steps completed.
     * @param total_steps           Total number of steps.
     */
    percentage(
        std::uint64_t completed_steps,
        std::uint64_t total_steps
    ) noexcept
      : m_percentage{std::clamp(
            static_cast<int>(
                (completed_steps * 100)
                / std::max<std::uint64_t>(total_steps, 1u)
            ),
            0,
            100
        )}
    { }

    /**
     * @returns The percentage value.
     */
    [[nodiscard]] int get() const noexcept
    {
        return m_percentage;
    }

    /**
     * @brief Set the percentage value.
     * 
     * @param percentage            New value in range [0, 100]. Clamped if out of range.
     */
    void set(int percentage) noexcept
    {
        m_percentage = std::clamp(percentage, 0, 100);
    }

    /**
     * @brief Convert percentage to string.
     * 
     * @returns String representation of the percentage, in the format "???%".
     */
    [[nodiscard]] std::string to_string() const noexcept
    {
        return std::to_string(m_percentage) + "%";
    }

private:
    int m_percentage;
};
} /* namespace utility */
} /* namespace recognition */

#endif /* PERCENTAGE_HPP */
