/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_progress_tracker_test.cpp
 * @brief Unit tests for Utility::ProgressTracker class.
 *
 * Tests the thread-safe progress tracking utility including:
 * - Construction with various step counts
 * - Advance() increments and clamping
 * - MarkAsCompleted() behavior
 * - Reset() for reuse
 * - Wait operations with timeouts
 * - Thread safety with concurrent access
 *
 * @section tracker_test_categories Test Categories
 *
 * | Category | Description |
 * |----------|-------------|
 * | Construction | Default and parameterized constructors |
 * | Advancement | Advance(), step counting, overflow prevention |
 * | Completion | MarkAsCompleted(), IsCompleted() |
 * | Reset | Reset() with new step counts |
 * | Waiting | TryWaitForCompletion(), WaitForCompletion() |
 * | Threading | Concurrent access from multiple threads |
 *
 * @see Utility::ProgressTracker
 * @see Utility::MakeSharedProgressTracker()
 */

#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "progress_tracker.hpp"

using namespace Recognition::Utility;

TEST(MagicProgressTrackerTest, default_constructor)
{
    ProgressTracker progress_tracker;
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 1);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, constructor_with_step_count)
{
    ProgressTracker progress_tracker{10};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 10);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, advance_increments)
{
    ProgressTracker progress_tracker{5};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 5);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance();
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 5);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 1);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 20);
    progress_tracker.Advance(2);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 5);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 3);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 60);
}

TEST(MagicProgressTrackerTest, advance_does_not_exceed_total)
{
    ProgressTracker progress_tracker{3};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 3);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance(5);
    EXPECT_TRUE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 3);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 3);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 100);
}

TEST(MagicProgressTrackerTest, get_incompleted_steps)
{
    ProgressTracker progress_tracker{10};
    EXPECT_EQ(progress_tracker.GetIncompletedSteps(), 10);
    progress_tracker.Advance(3);
    EXPECT_EQ(progress_tracker.GetIncompletedSteps(), 7);
    progress_tracker.Advance(2);
    EXPECT_EQ(progress_tracker.GetIncompletedSteps(), 5);
    progress_tracker.Advance(5);
    EXPECT_EQ(progress_tracker.GetIncompletedSteps(), 0);
}

TEST(MagicProgressTrackerTest, mark_as_completed_sets_to_100)
{
    ProgressTracker progress_tracker{7};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 7);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance(3);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 7);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 3);
    progress_tracker.MarkAsCompleted();
    EXPECT_TRUE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 7);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 7);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 100);
}

TEST(MagicProgressTrackerTest, reset_step_count_progress)
{
    ProgressTracker progress_tracker{5};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 5);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance(3);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 5);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 3);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 60);
    progress_tracker.Reset(10);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 10);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 0);
    progress_tracker.Advance(4);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 10);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 4);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 40);
}

TEST(MagicProgressTrackerTest, reset_step_count_to_zero)
{
    ProgressTracker progress_tracker{5};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 5);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Reset(0);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 1);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance();
    EXPECT_TRUE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 1);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 1);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 100);
    progress_tracker.Reset(1);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 1);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, reset_step_count_multiple_times)
{
    ProgressTracker progress_tracker{2};
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 2);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance();
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 2);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 1);
    progress_tracker.Reset(4);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 4);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    progress_tracker.Advance(2);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 4);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 2);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 50);
    progress_tracker.Reset(3);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    EXPECT_EQ(progress_tracker.GetTotalSteps(), 3);
    EXPECT_EQ(progress_tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, thread_safety)
{
    ProgressTracker progress_tracker{100};
    constexpr auto number_of_threads = 10;
    constexpr auto steps_per_thread = 10;

    std::vector<std::thread> threads;
    for (int thread_index = 0; thread_index < number_of_threads; ++thread_index) {
        threads.emplace_back([&progress_tracker]() {
            for (int step_index = 0; step_index < steps_per_thread; ++step_index) {
                progress_tracker.Advance();
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(progress_tracker.GetCompletionPercentage().Get(), 100);
    EXPECT_TRUE(progress_tracker.IsCompleted());
    SUCCEED();
}

TEST(MagicProgressTrackerTest, wait_for_completion)
{
    ProgressTracker progress_tracker{5};

    std::thread worker([&progress_tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        progress_tracker.Advance(5);
    });

    progress_tracker.WaitForCompletion();
    EXPECT_TRUE(progress_tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_for_completion_success)
{
    ProgressTracker progress_tracker{3};

    std::thread worker([&progress_tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        progress_tracker.Advance(3);
    });

    bool completed = progress_tracker.TryWaitForCompletion(
        std::chrono::milliseconds{500}
    );

    EXPECT_TRUE(completed);
    EXPECT_TRUE(progress_tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_for_completion_timeout)
{
    ProgressTracker progress_tracker{3};

    std::thread worker([&progress_tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        progress_tracker.Advance(3);
    });

    bool completed = progress_tracker.TryWaitForCompletion(
        std::chrono::milliseconds{100}
    );

    EXPECT_FALSE(completed);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_until_success)
{
    ProgressTracker progress_tracker{2};

    std::thread worker([&progress_tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        progress_tracker.Advance(2);
    });

    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds{300};
    bool completed = progress_tracker.TryWaitForCompletionUntil(deadline);

    EXPECT_TRUE(completed);
    EXPECT_TRUE(progress_tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_until_timeout)
{
    ProgressTracker progress_tracker{2};

    std::thread worker([&progress_tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{300});
        progress_tracker.Advance(2);
    });

    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds{100};
    bool completed = progress_tracker.TryWaitForCompletionUntil(deadline);

    EXPECT_FALSE(completed);
    EXPECT_FALSE(progress_tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, make_shared_progress_tracker_creates_tracker)
{
    auto shared_progress_tracker = MakeSharedProgressTracker(5);
    EXPECT_TRUE(shared_progress_tracker);
    EXPECT_EQ(shared_progress_tracker->GetTotalSteps(), 5);
    EXPECT_EQ(shared_progress_tracker->GetCompletedSteps(), 0);
    EXPECT_FALSE(shared_progress_tracker->IsCompleted());
}

TEST(MagicProgressTrackerTest, mark_tracker_as_completed_marks_on_scope_exit)
{
    auto shared_progress_tracker = MakeSharedProgressTracker(3);
    {
        MarkTrackerAsCompleted completion_marker(shared_progress_tracker);
        shared_progress_tracker->Advance(2);
        EXPECT_FALSE(shared_progress_tracker->IsCompleted());
    }
    EXPECT_TRUE(shared_progress_tracker->IsCompleted());
    EXPECT_EQ(shared_progress_tracker->GetCompletedSteps(), 3);
}

TEST(MagicProgressTrackerTest, advance_tracker_advances_on_scope_exit)
{
    auto shared_progress_tracker = MakeSharedProgressTracker(4);
    {
        AdvanceTracker tracker_advancer(shared_progress_tracker, 2);
        shared_progress_tracker->Advance(1);
        EXPECT_EQ(shared_progress_tracker->GetCompletedSteps(), 1);
    }
    EXPECT_EQ(shared_progress_tracker->GetCompletedSteps(), 3);
    EXPECT_FALSE(shared_progress_tracker->IsCompleted());
    {
        AdvanceTracker tracker_advancer(shared_progress_tracker, 1);
    }
    EXPECT_TRUE(shared_progress_tracker->IsCompleted());
    EXPECT_EQ(shared_progress_tracker->GetCompletedSteps(), 4);
}
