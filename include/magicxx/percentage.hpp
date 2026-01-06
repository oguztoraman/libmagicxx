/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file percentage.hpp
 * @brief Percentage value type for progress tracking.
 * @ingroup magic_utility
 *
 * This file defines the Percentage class, a simple value type that
 * represents a percentage in the range [0, 100]. It provides automatic
 * clamping to ensure values always remain valid.
 *
 * @section percentage_usage Usage Examples
 *
 * Direct construction:
 * @code{.cpp}
 * Recognition::Utility::Percentage p1{50};      // 50%
 * Recognition::Utility::Percentage p2{150};     // Clamped to 100%
 * Recognition::Utility::Percentage p3{-10};     // Clamped to 0%
 * @endcode
 *
 * From completed/total steps:
 * @code{.cpp}
 * Recognition::Utility::Percentage p{75, 100};  // 75%
 * Recognition::Utility::Percentage p2{1, 3};    // 33%
 * @endcode
 *
 * @see ProgressTracker
 */

#ifndef PERCENTAGE_HPP
#define PERCENTAGE_HPP

#include <algorithm>
#include <cstdint>
#include <string>

namespace Recognition {
namespace Utility {
/**
 * @defgroup magic_utility Utility Classes
 * @ingroup magic_core
 * @brief Utility types used throughout the Magic library.
 *
 * @{
 */

/**
 * @class Percentage
 * @ingroup magic_utility
 *
 * @brief A type-safe percentage value in the range [0, 100].
 *
 * Percentage provides a strongly-typed representation of percentage values
 * with automatic clamping to ensure values always remain within valid bounds.
 * This class is particularly useful for progress tracking scenarios.
 *
 * @section percentage_features Key Features
 *
 * - **Automatic Clamping**: Values are automatically clamped to [0, 100]
 * - **Step-Based Construction**: Calculate percentage from completed/total steps
 * - **String Formatting**: Convert to human-readable "X%" format
 * - **noexcept Guarantee**: All operations are noexcept
 *
 * @section percentage_example Example
 *
 * @code{.cpp}
 * // Direct percentage
 * Percentage progress{75};
 * std::println("Progress: {}", progress.ToString());  // "75%"
 *
 * // From steps
 * Percentage ratio{7, 10};  // 70%
 *
 * // Update value
 * progress.Set(100);
 * std::println("Complete: {}", progress.ToString());  // "100%"
 * @endcode
 *
 * @see ProgressTracker
 *
 * @since 10.0.0
 */
class Percentage {
public:
    /**
     * @brief Construct Percentage with a direct value.
     *
     * The value is automatically clamped to the range [0, 100].
     *
     * @param[in] percentage Value in range [0, 100] (default: 0).
     *                       Values outside this range are clamped.
     *
     * @code{.cpp}
     * Percentage p1;         // 0%
     * Percentage p2{50};     // 50%
     * Percentage p3{200};    // 100% (clamped)
     * @endcode
     *
     * @since 10.0.0
     */
    explicit Percentage(int percentage = 0) noexcept
      : m_percentage{std::clamp(percentage, 0, 100)}
    { }

    /**
     * @brief Construct Percentage from completed and total steps.
     *
     * Calculates the percentage as `(completed_steps * 100) / total_steps`,
     * with protection against division by zero (total_steps of 0 results in 0%).
     *
     * @param[in] completed_steps Number of steps completed.
     * @param[in] total_steps     Total number of steps (0 is treated as 1).
     *
     * @code{.cpp}
     * Percentage p1{50, 100};   // 50%
     * Percentage p2{1, 4};      // 25%
     * Percentage p3{10, 0};     // 0% (safe division by zero)
     * @endcode
     *
     * @since 10.0.0
     */
    Percentage(
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
     * @brief Get the percentage value.
     *
     * @return The percentage value in the range [0, 100].
     *
     * @code{.cpp}
     * Percentage p{75};
     * int value = p.Get();  // 75
     * @endcode
     *
     * @since 10.0.0
     */
    [[nodiscard]] int Get() const noexcept
    {
        return m_percentage;
    }

    /**
     * @brief Set a new percentage value.
     *
     * The value is automatically clamped to the range [0, 100].
     *
     * @param[in] percentage New value (clamped to [0, 100]).
     *
     * @code{.cpp}
     * Percentage p;
     * p.Set(50);   // Now 50%
     * p.Set(-10);  // Clamped to 0%
     * @endcode
     *
     * @since 10.0.0
     */
    void Set(int percentage) noexcept
    {
        m_percentage = std::clamp(percentage, 0, 100);
    }

    /**
     * @brief Convert percentage to a human-readable string.
     *
     * @return String in the format "X%" where X is the percentage value.
     *
     * @code{.cpp}
     * Percentage p{42};
     * std::string s = p.ToString();  // "42%"
     * @endcode
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::string ToString() const noexcept
    {
        return std::to_string(m_percentage) + "%";
    }

private:
    int m_percentage; ///< The percentage value, always in range [0, 100].
};

/** @} */ // End of magic_utility group
} /* namespace Utility */
} /* namespace Recognition */

#endif /* PERCENTAGE_HPP */
