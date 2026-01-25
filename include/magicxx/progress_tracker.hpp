/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file progress_tracker.hpp
 * @brief Thread-safe progress tracking for batch file operations.
 * @ingroup magic_utility
 *
 * This file provides classes for tracking the progress of long-running operations
 * such as batch file identification. The ProgressTracker class is thread-safe and
 * can be shared between the worker thread and the UI/monitoring thread.
 *
 * @section progress_tracker_overview Overview
 *
 * - ProgressTracker: Core class for tracking completed/total steps
 * - SharedProgressTrackerT: Shared pointer type for thread-safe sharing
 * - MakeSharedProgressTracker(): Factory function for creating trackers
 * - MarkTrackerAsCompleted: RAII helper for completion marking
 * - AdvanceTracker: RAII helper for automatic step advancement
 *
 * @section progress_tracker_usage Usage Example
 *
 * @code{.cpp}
 * using namespace Recognition;
 * using namespace std::chrono_literals;
 *
 * // Create a shared progress tracker
 * auto tracker = Utility::MakeSharedProgressTracker();
 *
 * // Start batch identification in background
 * auto future = std::async([tracker] {
 *     Magic magic{Magic::Flags::Mime};
 *     return magic.IdentifyFiles("/path/to/directory", tracker);
 * });
 *
 * // Monitor progress in main thread
 * while (!tracker->IsCompleted()) {
 *     auto percentage = tracker->GetCompletionPercentage();
 *     std::println("Progress: {} ({}/{})",
 *         percentage.ToString(),
 *         tracker->GetCompletedSteps(),
 *         tracker->GetTotalSteps());
 *     std::this_thread::sleep_for(100ms);
 * }
 *
 * auto results = future.get();
 * @endcode
 *
 * @see Percentage
 * @see Magic::IdentifyFiles()
 */

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
 * @ingroup magic_utility
 *
 * @brief Thread-safe tracker for monitoring multi-step job progress.
 *
 * ProgressTracker provides a thread-safe mechanism for tracking the progress
 * of jobs composed of multiple steps. It supports:
 *
 * - **Step counting**: Track completed steps out of total steps
 * - **Percentage calculation**: Get completion as a percentage
 * - **Waiting**: Block until completion with optional timeouts
 * - **Thread safety**: All operations are protected by mutex
 *
 * @section progress_tracker_threading Thread Safety
 *
 * All public methods are thread-safe. The typical pattern is:
 * - **Worker thread**: Calls Advance() after each step
 * - **Monitor thread**: Calls GetCompletionPercentage() or WaitForCompletion()
 *
 * @section progress_tracker_example Example
 *
 * @code{.cpp}
 * using namespace std::chrono_literals;
 *
 * ProgressTracker tracker{100};  // 100 steps total
 *
 * // Worker updates progress
 * for (int i = 0; i < 100; ++i) {
 *     DoWork();
 *     tracker.Advance();
 * }
 *
 * // Monitor checks progress
 * if (tracker.TryWaitForCompletion(5s)) {
 *     std::println("Completed!");
 * } else {
 *     std::println("Timed out at {}%",
 *         tracker.GetCompletionPercentage().Get());
 * }
 * @endcode
 *
 * @see Percentage
 * @see SharedProgressTrackerT
 * @see MakeSharedProgressTracker()
 *
 * @since 10.0.0
 */
class ProgressTracker {
public:
    /**
     * @brief Construct a ProgressTracker with a given number of total steps.
     *
     * @param[in] total_steps Total number of steps for the job (minimum 1).
     *                        Defaults to 1 if not specified or if 0 is passed.
     *
     * @code{.cpp}
     * ProgressTracker tracker1;       // 1 step (for simple complete/not complete)
     * ProgressTracker tracker2{100};  // 100 steps
     * @endcode
     *
     * @since 10.0.0
     */
    explicit ProgressTracker(std::uint64_t total_steps = 1u) noexcept
      : m_total_steps{std::max<std::uint64_t>(total_steps, 1u)}
    { }

    /**
     * @brief Advance the progress by a number of completed steps.
     *
     * Increments the completed step counter. The value is clamped to
     * not exceed total_steps.
     *
     * @param[in] step_count Number of steps completed (default: 1).
     *
     * @note This method notifies any threads waiting on WaitForCompletion().
     * @note Thread-safe: protected by mutex.
     *
     * @code{.cpp}
     * tracker.Advance();      // Advance by 1
     * tracker.Advance(10);    // Advance by 10
     * @endcode
     *
     * @since 10.0.0
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
     * @brief Get the number of completed steps.
     *
     * @returns Number of steps completed so far.
     *
     * @note Thread-safe: protected by mutex.
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::uint64_t GetCompletedSteps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_completed_steps;
    }

    /**
     * @brief Get the completion percentage.
     *
     * @returns Percentage object representing completion (0% to 100%).
     *
     * @note Thread-safe: protected by mutex.
     *
     * @code{.cpp}
     * auto pct = tracker.GetCompletionPercentage();
     * std::println("Progress: {}", pct.ToString());  // e.g., "75%"
     * @endcode
     *
     * @see Percentage
     *
     * @since 10.0.0
     */
    [[nodiscard]] Percentage GetCompletionPercentage() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return Percentage{m_completed_steps, m_total_steps};
    }

    /**
     * @brief Get the number of remaining (incomplete) steps.
     *
     * @returns Number of steps yet to be completed.
     *
     * @note Thread-safe: protected by mutex.
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::uint64_t GetIncompletedSteps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_total_steps - m_completed_steps;
    }

    /**
     * @brief Get the total number of steps.
     *
     * @returns Total number of steps for the job.
     *
     * @note Thread-safe: protected by mutex.
     *
     * @since 10.0.0
     */
    [[nodiscard]] std::uint64_t GetTotalSteps() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_total_steps;
    }

    /**
     * @brief Check if the job is complete.
     *
     * @returns `true` if completed_steps >= total_steps, `false` otherwise.
     *
     * @note Thread-safe: protected by mutex.
     *
     * @since 10.0.0
     */
    [[nodiscard]] bool IsCompleted() const noexcept
    {
        std::lock_guard lock{m_mutex};
        return m_completed_steps >= m_total_steps;
    }

    /**
     * @brief Mark the job as fully completed.
     *
     * Sets completed_steps to total_steps and notifies waiting threads.
     *
     * @note Thread-safe: protected by mutex.
     *
     * @code{.cpp}
     * // Early termination
     * if (shouldCancel) {
     *     tracker.MarkAsCompleted();
     * }
     * @endcode
     *
     * @since 10.0.0
     */
    void MarkAsCompleted() noexcept
    {
        std::lock_guard lock{m_mutex};
        m_completed_steps = m_total_steps;
        m_condition_variable.notify_all();
    }

    /**
     * @brief Reset the tracker for a new job.
     *
     * Resets completed_steps to 0 and optionally sets a new total.
     *
     * @param[in] total_steps New total number of steps (minimum 1).
     *
     * @note Thread-safe: protected by mutex.
     *
     * @code{.cpp}
     * tracker.Reset(200);  // Start new job with 200 steps
     * @endcode
     *
     * @since 10.0.0
     */
    void Reset(std::uint64_t total_steps) noexcept
    {
        std::lock_guard lock{m_mutex};
        m_total_steps     = std::max<std::uint64_t>(total_steps, 1u);
        m_completed_steps = 0u;
        m_condition_variable.notify_all();
    }

    /**
     * @brief Wait for completion with a timeout.
     *
     * Blocks the calling thread until the job completes or the timeout expires.
     *
     * @tparam RepresentationT Duration representation type.
     * @tparam PeriodT         Duration period ratio type.
     *
     * @param[in] timeout Maximum duration to wait.
     *
     * @returns `true` if the job completed before the timeout,
     *         `false` if the timeout expired.
     *
     * @code{.cpp}
     * using namespace std::chrono_literals;
     * if (tracker.TryWaitForCompletion(5s)) {
     *     std::println("Job completed!");
     * } else {
     *     std::println("Timed out.");
     * }
     * @endcode
     *
     * @since 10.0.0
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
     * @brief Wait for completion until a deadline.
     *
     * Blocks the calling thread until the job completes or the deadline passes.
     *
     * @tparam ClockT    Clock type for the time point.
     * @tparam DurationT Duration type for the time point.
     *
     * @param[in] deadline Absolute time point to wait until.
     *
     * @returns `true` if the job completed before the deadline,
     *         `false` if the deadline passed.
     *
     * @code{.cpp}
     * using namespace std::chrono_literals;
     * auto deadline = std::chrono::steady_clock::now() + 10s;
     * if (tracker.TryWaitForCompletionUntil(deadline)) {
     *     std::println("Completed on time!");
     * }
     * @endcode
     *
     * @since 10.0.0
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
     * @brief Wait indefinitely for the job to complete.
     *
     * Blocks the calling thread until the job is fully completed.
     *
     * @warning This blocks forever if the job never completes.
     *          Consider using TryWaitForCompletion() with a timeout instead.
     *
     * @code{.cpp}
     * // Start background job
     * auto future = std::async([tracker] { return doWork(tracker); });
     *
     * // Wait for completion (blocking)
     * tracker.WaitForCompletion();
     * auto result = future.get();
     * @endcode
     *
     * @since 10.0.0
     */
    void WaitForCompletion() const
    {
        std::unique_lock lock{m_mutex};
        m_condition_variable.wait(lock, [this] {
            return m_completed_steps >= m_total_steps;
        });
    }

private:
    mutable std::mutex m_mutex{}; /**< Mutex for thread-safe access. */
    mutable std::condition_variable
                  m_condition_variable{}; /**< For wait operations. */
    std::uint64_t m_total_steps;          /**< Total steps in the job. */
    std::uint64_t m_completed_steps{0u};  /**< Steps completed so far. */
};

/**
 * @typedef SharedProgressTrackerT
 * @ingroup magic_utility
 *
 * @brief Shared pointer type for ProgressTracker.
 *
 * This type alias is used throughout the library for thread-safe
 * sharing of progress trackers between threads.
 *
 * @see ProgressTracker
 * @see MakeSharedProgressTracker()
 *
 * @since 10.0.0
 */
using SharedProgressTrackerT = std::shared_ptr<ProgressTracker>;

/**
 * @brief Factory function to create a shared ProgressTracker.
 * @ingroup magic_utility
 *
 * Creates a ProgressTracker wrapped in a shared_ptr for thread-safe
 * sharing between the producer (worker) and consumer (monitor) threads.
 *
 * @param[in] total_steps Total number of steps (minimum 1, default 1).
 *
 * @returns A shared pointer to the newly created ProgressTracker.
 *
 * @code{.cpp}
 * auto tracker = MakeSharedProgressTracker(100);
 *
 * // Pass to worker thread
 * auto future = std::async([tracker] {
 *     for (int i = 0; i < 100; ++i) {
 *         DoStep();
 *         tracker->Advance();
 *     }
 * });
 *
 * // Monitor in main thread
 * while (!tracker->IsCompleted()) {
 *     std::println("{}", tracker->GetCompletionPercentage().ToString());
 * }
 * @endcode
 *
 * @see SharedProgressTrackerT
 * @see ProgressTracker
 *
 * @since 10.0.0
 */
inline SharedProgressTrackerT MakeSharedProgressTracker(
    std::uint64_t total_steps = 1u
) noexcept
{
    return std::make_shared<ProgressTracker>(total_steps);
}

/**
 * @class MarkTrackerAsCompleted
 * @ingroup magic_utility
 *
 * @brief RAII helper that marks a ProgressTracker as completed on destruction.
 *
 * This class ensures that a progress tracker is marked as completed
 * when the scope exits, regardless of how the scope is exited (normal
 * return, exception, early exit, etc.).
 *
 * @section mark_tracker_usage Usage Example
 *
 * @code{.cpp}
 * void ProcessFiles(SharedProgressTrackerT tracker) {
 *     MarkTrackerAsCompleted guard(tracker);
 *
 *     // Process files...
 *     // If an exception is thrown, tracker is still marked complete
 *
 * }  // tracker->MarkAsCompleted() called automatically
 * @endcode
 *
 * @see ProgressTracker::MarkAsCompleted()
 *
 * @since 10.0.0
 */
class MarkTrackerAsCompleted {
public:
    /**
     * @brief Construct with a shared progress tracker.
     *
     * @param[in] shared_progress_tracker Tracker to mark as completed on destruction.
     *                                    May be null (no-op in that case).
     *
     * @since 10.0.0
     */
    explicit MarkTrackerAsCompleted(
        SharedProgressTrackerT shared_progress_tracker
    ) noexcept
      : m_shared_progress_tracker(shared_progress_tracker)
    { }

    /**
     * @brief Destructor that marks the tracker as completed.
     *
     * If the tracker is not null, calls MarkAsCompleted() on it.
     *
     * @since 10.0.0
     */
    ~MarkTrackerAsCompleted()
    {
        if (!m_shared_progress_tracker) {
            return;
        }
        m_shared_progress_tracker->MarkAsCompleted();
    }

private:
    SharedProgressTrackerT
        m_shared_progress_tracker; /**< Tracker to complete on destruction. */
};

/**
 * @class AdvanceTracker
 * @ingroup magic_utility
 *
 * @brief RAII helper that advances a ProgressTracker on destruction.
 *
 * This class ensures that a progress tracker is advanced by a specified
 * number of steps when the scope exits, providing exception-safe progress
 * updates.
 *
 * @section advance_tracker_usage Usage Example
 *
 * @code{.cpp}
 * void ProcessFile(SharedProgressTrackerT tracker, const std::filesystem::path& file) {
 *     AdvanceTracker guard(tracker);  // Will advance by 1 on exit
 *
 *     // Process the file...
 *     // Even if an exception is thrown, progress is updated
 *
 * }  // tracker->Advance(1) called automatically
 * @endcode
 *
 * @see ProgressTracker::Advance()
 *
 * @since 10.0.0
 */
class AdvanceTracker {
public:
    /**
     * @brief Construct with a shared progress tracker and step count.
     *
     * @param[in] shared_progress_tracker Tracker to advance on destruction.
     *                                    May be null (no-op in that case).
     * @param[in] step_count              Number of steps to advance (default: 1).
     *
     * @since 10.0.0
     */
    explicit AdvanceTracker(
        SharedProgressTrackerT shared_progress_tracker,
        std::uint64_t          step_count = 1u
    ) noexcept
      : m_shared_progress_tracker(shared_progress_tracker)
      , m_step_count(step_count)
    { }

    /**
     * @brief Destructor that advances the tracker.
     *
     * If the tracker is not null, calls Advance() with the step count.
     *
     * @since 10.0.0
     */
    ~AdvanceTracker()
    {
        if (!m_shared_progress_tracker) {
            return;
        }
        m_shared_progress_tracker->Advance(m_step_count);
    }

private:
    SharedProgressTrackerT
        m_shared_progress_tracker; /**< Tracker to advance on destruction. */
    std::uint64_t m_step_count;    /**< Steps to advance. */
};
} /* namespace Utility */
} /* namespace Recognition */

#endif /* PROGRESS_TRACKER_HPP */
