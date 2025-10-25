/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef PROGRESS_TRACKER_HPP
#define PROGRESS_TRACKER_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "percentage.hpp"

namespace recognition {
namespace utility {
/**
 * @class progress_tracker
 *
 * @brief Thread-safe tracker for monitoring the progress of a job composed of multiple steps.
 */
class progress_tracker {
public:
    /**
     * @brief Construct progress_tracker with a given total number of steps.
     * 
     * @param total_steps           Total number of steps (must be >= 1). Defaults to 1.
     */
    explicit progress_tracker(std::uint64_t total_steps = 1u) noexcept
      : m_total_steps{std::max<std::uint64_t>(total_steps, 1u)}
    { }

    /**
     * @brief Increments the number of completed steps.
     * 
     * @param step_count            Number of steps to mark as completed. Defaults to 1.
     */
    void advance(std::uint64_t step_count = 1u) noexcept
    {
        std::lock_guard lock{m_mutex};
        m_completed_steps = std::min(
            m_completed_steps + step_count,
            m_total_steps
        );
        m_condition_variable.notify_all();
    }

    /**
     * @returns The number of completed steps.
     */
    [[nodiscard]] std::uint64_t get_completed_steps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_completed_steps;
    }

    /**
     * @returns The completion percentage of the job.
     */
    [[nodiscard]] percentage get_completion_percentage() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return percentage{m_completed_steps, m_total_steps};
    }

    /**
     * @returns The number of steps that are yet to be completed.
     */
    [[nodiscard]] std::uint64_t get_incompleted_steps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_total_steps - m_completed_steps;
    }

    /**
     * @returns The total number of steps.
     */
    [[nodiscard]] std::uint64_t get_total_steps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_total_steps;
    }

    /**
     * @returns True if the job has completed all steps, false otherwise.
     */
    [[nodiscard]] bool is_completed() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_completed_steps >= m_total_steps;
    }

    /**
     * @brief Marks the job as fully completed.
     */
    void mark_as_completed() noexcept
    {
        std::lock_guard lock{m_mutex};
        m_completed_steps = m_total_steps;
        m_condition_variable.notify_all();
    }

    /**
     * @brief Reset the tracker with a new total step count.
     *
     * @param total_steps           New total number of steps (must be >= 1).
     */
    void reset(std::uint64_t total_steps) noexcept
    {
        std::lock_guard lock{m_mutex};
        m_total_steps     = std::max<std::uint64_t>(total_steps, 1u);
        m_completed_steps = 0u;
        m_condition_variable.notify_all();
    }

    /**
     * @brief Wait for the job to complete, or until the specified timeout duration elapses.
     *
     * @tparam RepresentationType   A representation type for the duration.
     * @tparam PeriodType           A ratio type representing the duration's tick period.
     *
     * @param timeout               Maximum duration to wait.
     *
     * @returns True if the job completed before the timeout, false otherwise.
     */
    template <typename RepresentationType, typename PeriodType>
    [[nodiscard]] bool try_wait_for_completion(
        std::chrono::duration<RepresentationType, PeriodType> timeout
    ) const
    {
        std::unique_lock lock{m_mutex};
        return m_condition_variable.wait_for(lock, timeout, [this] {
            return m_completed_steps >= m_total_steps;
        });
    }

    /**
     * @brief Wait until the job is complete or the specified deadline is reached.
     *
     * @tparam ClockType            A clock type used for time points.
     * @tparam DurationType         A duration type representing the clock's precision or resolution.
     *
     * @param deadline              The absolute time point to wait until.
     *
     * @returns True if the job completed before the deadline, false otherwise.
     */
    template <typename ClockType, typename DurationType>
    [[nodiscard]] bool try_wait_for_completion_until(
        std::chrono::time_point<ClockType, DurationType> deadline
    ) const
    {
        std::unique_lock lock{m_mutex};
        return m_condition_variable.wait_until(lock, deadline, [this] {
            return m_completed_steps >= m_total_steps;
        });
    }

    /**
     * @brief Wait until all steps are completed.
     */
    void wait_for_completion() const
    {
        std::unique_lock lock{m_mutex};
        m_condition_variable.wait(lock, [this] {
            return m_completed_steps >= m_total_steps;
        });
    }

private:
    mutable std::mutex              m_mutex{};
    mutable std::condition_variable m_condition_variable{};
    std::uint64_t                   m_total_steps;
    std::uint64_t                   m_completed_steps{0u};
};

/**
 * @typedef shared_progress_tracker_t
 *
 * @brief Alias for a shared pointer to progress_tracker.
 */
using shared_progress_tracker_t = std::shared_ptr<progress_tracker>;

/**
 * @brief Create a shared pointer to a progress_tracker.
 *
 * @param total_steps               Total number of steps (must be >= 1). Defaults to 1.
 *
 * @return A shared pointer to the newly created progress_tracker.
 */
inline shared_progress_tracker_t make_shared_progress_tracker(
    std::uint64_t total_steps = 1u
) noexcept
{
    return std::make_shared<progress_tracker>(total_steps);
}

/**
 * @class mark_tracker_as_completed
 *
 * @brief RAII helper that marks a shared progress_tracker as completed upon destruction.
 */
class mark_tracker_as_completed {
public:
    /**
     * @brief Construct mark_tracker_as_completed.
     *
     * @param shared_tracker        The shared tracker to mark as completed.
     */
    explicit mark_tracker_as_completed(
        shared_progress_tracker_t shared_tracker
    ) noexcept
      : m_shared_tracker(shared_tracker)
    { }

    /**
     * @brief Mark the tracker as completed if it exists.
     */
    ~mark_tracker_as_completed()
    {
        if (!m_shared_tracker) {
            return;
        }
        m_shared_tracker->mark_as_completed();
    }

private:
    shared_progress_tracker_t m_shared_tracker;
};

/**
 * @class advance_tracker
 *
 * @brief RAII helper that advances a shared progress_tracker by a given step count upon destruction.
 */
class advance_tracker {
public:
    /**
     * @brief Construct advance_tracker with a shared tracker and step count.
     *
     * @param shared_tracker        The shared tracker to advance.
     * @param step_count            Number of steps to advance (default: 1).
     */
    explicit advance_tracker(
        shared_progress_tracker_t shared_tracker,
        std::uint64_t             step_count = 1u
    ) noexcept
      : m_shared_tracker(shared_tracker)
      , m_step_count(step_count)
    { }

    /**
     * @brief Advance the tracker by the step count if it exists.
     */
    ~advance_tracker()
    {
        if (!m_shared_tracker) {
            return;
        }
        m_shared_tracker->advance(m_step_count);
    }

private:
    shared_progress_tracker_t m_shared_tracker;
    std::uint64_t             m_step_count;
};
} /* namespace utility */
} /* namespace recognition */

#endif /* PROGRESS_TRACKER_HPP */
