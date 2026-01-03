/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#ifndef PROGRESS_TRACKER_HPP
#define PROGRESS_TRACKER_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "percentage.hpp"

namespace Recognition {
namespace Utility {
/**
 * @class ProgressTracker
 *
 * @brief Thread-safe tracker for monitoring the progress of a job composed of multiple steps.
 */
class ProgressTracker {
public:
    /**
     * @brief Construct ProgressTracker with a given total number of steps.
     * 
     * @param total_steps           Total number of steps (must be >= 1). Defaults to 1.
     */
    explicit ProgressTracker(std::uint64_t total_steps = 1u) noexcept
      : m_total_steps{std::max<std::uint64_t>(total_steps, 1u)}
    { }

    /**
     * @brief Increments the number of completed steps.
     * 
     * @param step_count            Number of steps to mark as completed. Defaults to 1.
     */
    void Advance(std::uint64_t step_count = 1u) noexcept
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
    [[nodiscard]] std::uint64_t GetCompletedSteps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_completed_steps;
    }

    /**
     * @returns The completion percentage of the job.
     */
    [[nodiscard]] Percentage GetCompletionPercentage() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return Percentage{m_completed_steps, m_total_steps};
    }

    /**
     * @returns The number of steps that are yet to be completed.
     */
    [[nodiscard]] std::uint64_t GetIncompletedSteps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_total_steps - m_completed_steps;
    }

    /**
     * @returns The total number of steps.
     */
    [[nodiscard]] std::uint64_t GetTotalSteps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_total_steps;
    }

    /**
     * @returns True if the job has completed all steps, false otherwise.
     */
    [[nodiscard]] bool IsCompleted() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_completed_steps >= m_total_steps;
    }

    /**
     * @brief Marks the job as fully completed.
     */
    void MarkAsCompleted() noexcept
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
    void Reset(std::uint64_t total_steps) noexcept
    {
        std::lock_guard lock{m_mutex};
        m_total_steps     = std::max<std::uint64_t>(total_steps, 1u);
        m_completed_steps = 0u;
        m_condition_variable.notify_all();
    }

    /**
     * @brief Wait for the job to complete, or until the specified timeout duration elapses.
     *
     * @tparam RepresentationT   A representation type for the duration.
     * @tparam PeriodT           A ratio type representing the duration's tick period.
     *
     * @param timeout               Maximum duration to wait.
     *
     * @returns True if the job completed before the timeout, false otherwise.
     */
    template <typename RepresentationT, typename PeriodT>
    [[nodiscard]] bool TryWaitForCompletion(
        std::chrono::duration<RepresentationT, PeriodT> timeout
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
     * @tparam ClockT            A clock type used for time points.
     * @tparam DurationT         A duration type representing the clock's precision or resolution.
     *
     * @param deadline              The absolute time point to wait until.
     *
     * @returns True if the job completed before the deadline, false otherwise.
     */
    template <typename ClockT, typename DurationT>
    [[nodiscard]] bool TryWaitForCompletionUntil(
        std::chrono::time_point<ClockT, DurationT> deadline
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
    void WaitForCompletion() const
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
 * @typedef SharedProgressTrackerT
 *
 * @brief Alias for a shared pointer to ProgressTracker.
 */
using SharedProgressTrackerT = std::shared_ptr<ProgressTracker>;

/**
 * @brief Create a shared pointer to a ProgressTracker.
 *
 * @param total_steps               Total number of steps (must be >= 1). Defaults to 1.
 *
 * @return A shared pointer to the newly created ProgressTracker.
 */
inline SharedProgressTrackerT MakeSharedProgressTracker(
    std::uint64_t total_steps = 1u
) noexcept
{
    return std::make_shared<ProgressTracker>(total_steps);
}

/**
 * @class MarkTrackerAsCompleted
 *
 * @brief RAII helper that marks a shared ProgressTracker as completed upon destruction.
 */
class MarkTrackerAsCompleted {
public:
    /**
     * @brief Construct MarkTrackerAsCompleted.
     *
     * @param shared_progress_tracker The shared progress tracker to mark as completed.
     */
    explicit MarkTrackerAsCompleted(
        SharedProgressTrackerT shared_progress_tracker
    ) noexcept
      : m_shared_progress_tracker(shared_progress_tracker)
    { }

    /**
     * @brief Mark the progress tracker as completed if it exists.
     */
    ~MarkTrackerAsCompleted()
    {
        if (!m_shared_progress_tracker) {
            return;
        }
        m_shared_progress_tracker->MarkAsCompleted();
    }

private:
    SharedProgressTrackerT m_shared_progress_tracker;
};

/**
 * @class AdvanceTracker
 *
 * @brief RAII helper that advances a shared ProgressTracker by a given step count upon destruction.
 */
class AdvanceTracker {
public:
    /**
     * @brief Construct AdvanceTracker with a shared progress tracker and step count.
     *
     * @param shared_progress_tracker The shared progress tracker to advance.
     * @param step_count              Number of steps to advance (default: 1).
     */
    explicit AdvanceTracker(
        SharedProgressTrackerT shared_progress_tracker,
        std::uint64_t          step_count = 1u
    ) noexcept
      : m_shared_progress_tracker(shared_progress_tracker)
      , m_step_count(step_count)
    { }

    /**
     * @brief Advance the progress tracker by the step count if it exists.
     */
    ~AdvanceTracker()
    {
        if (!m_shared_progress_tracker) {
            return;
        }
        m_shared_progress_tracker->Advance(m_step_count);
    }

private:
    SharedProgressTrackerT m_shared_progress_tracker;
    std::uint64_t          m_step_count;
};
} /* namespace Utility */
} /* namespace Recognition */

#endif /* PROGRESS_TRACKER_HPP */
