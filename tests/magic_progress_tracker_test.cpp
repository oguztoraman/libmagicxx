/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "progress_tracker.hpp"

using namespace Recognition::Utility;

TEST(MagicProgressTrackerTest, default_constructor)
{
    ProgressTracker tracker;
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 1);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, constructor_with_step_count)
{
    ProgressTracker tracker{10};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 10);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, advance_increments)
{
    ProgressTracker tracker{5};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 5);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance();
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 5);
    EXPECT_EQ(tracker.GetCompletedSteps(), 1);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 20);
    tracker.Advance(2);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 5);
    EXPECT_EQ(tracker.GetCompletedSteps(), 3);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 60);
}

TEST(MagicProgressTrackerTest, advance_does_not_exceed_total)
{
    ProgressTracker tracker{3};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 3);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance(5);
    EXPECT_TRUE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 3);
    EXPECT_EQ(tracker.GetCompletedSteps(), 3);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 100);
}

TEST(MagicProgressTrackerTest, get_incompleted_steps)
{
    ProgressTracker tracker{10};
    EXPECT_EQ(tracker.GetIncompletedSteps(), 10);
    tracker.Advance(3);
    EXPECT_EQ(tracker.GetIncompletedSteps(), 7);
    tracker.Advance(2);
    EXPECT_EQ(tracker.GetIncompletedSteps(), 5);
    tracker.Advance(5);
    EXPECT_EQ(tracker.GetIncompletedSteps(), 0);
}

TEST(MagicProgressTrackerTest, mark_as_completed_sets_to_100)
{
    ProgressTracker tracker{7};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 7);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance(3);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 7);
    EXPECT_EQ(tracker.GetCompletedSteps(), 3);
    tracker.MarkAsCompleted();
    EXPECT_TRUE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 7);
    EXPECT_EQ(tracker.GetCompletedSteps(), 7);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 100);
}

TEST(MagicProgressTrackerTest, reset_step_count_progress)
{
    ProgressTracker tracker{5};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 5);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance(3);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 5);
    EXPECT_EQ(tracker.GetCompletedSteps(), 3);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 60);
    tracker.Reset(10);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 10);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
    tracker.Advance(4);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 10);
    EXPECT_EQ(tracker.GetCompletedSteps(), 4);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 40);
}

TEST(MagicProgressTrackerTest, reset_step_count_to_zero)
{
    ProgressTracker tracker{5};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 5);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Reset(0);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 1);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance();
    EXPECT_TRUE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 1);
    EXPECT_EQ(tracker.GetCompletedSteps(), 1);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 100);
    tracker.Reset(1);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 1);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, reset_step_count_multiple_times)
{
    ProgressTracker tracker{2};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 2);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance();
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 2);
    EXPECT_EQ(tracker.GetCompletedSteps(), 1);
    tracker.Reset(4);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 4);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    tracker.Advance(2);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 4);
    EXPECT_EQ(tracker.GetCompletedSteps(), 2);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 50);
    tracker.Reset(3);
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 3);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, thread_safety)
{
    ProgressTracker tracker{100};
    constexpr auto   number_threads   = 10;
    constexpr auto   steps_per_thread = 10;

    std::vector<std::thread> threads;
    for (int t = 0; t < number_threads; ++t) {
        threads.emplace_back([&tracker]() {
            for (int i = 0; i < steps_per_thread; ++i) {
                tracker.Advance();
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 100);
    EXPECT_TRUE(tracker.IsCompleted());
    SUCCEED();
}

TEST(MagicProgressTrackerTest, wait_for_completion)
{
    ProgressTracker tracker{5};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        tracker.Advance(5);
    });

    tracker.WaitForCompletion();
    EXPECT_TRUE(tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_for_completion_success)
{
    ProgressTracker tracker{3};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        tracker.Advance(3);
    });

    bool completed = tracker.TryWaitForCompletion(
        std::chrono::milliseconds{500}
    );

    EXPECT_TRUE(completed);
    EXPECT_TRUE(tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_for_completion_timeout)
{
    ProgressTracker tracker{3};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        tracker.Advance(3);
    });

    bool completed = tracker.TryWaitForCompletion(
        std::chrono::milliseconds{100}
    );

    EXPECT_FALSE(completed);
    EXPECT_FALSE(tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_until_success)
{
    ProgressTracker tracker{2};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        tracker.Advance(2);
    });

    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds{300};
    bool completed = tracker.TryWaitForCompletionUntil(deadline);

    EXPECT_TRUE(completed);
    EXPECT_TRUE(tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, try_wait_until_timeout)
{
    ProgressTracker tracker{2};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{300});
        tracker.Advance(2);
    });

    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds{100};
    bool completed = tracker.TryWaitForCompletionUntil(deadline);

    EXPECT_FALSE(completed);
    EXPECT_FALSE(tracker.IsCompleted());
    worker.join();
}

TEST(MagicProgressTrackerTest, make_shared_progress_tracker_creates_tracker)
{
    auto tracker = MakeSharedProgressTracker(5);
    EXPECT_TRUE(tracker);
    EXPECT_EQ(tracker->GetTotalSteps(), 5);
    EXPECT_EQ(tracker->GetCompletedSteps(), 0);
    EXPECT_FALSE(tracker->IsCompleted());
}

TEST(MagicProgressTrackerTest, mark_tracker_as_completed_marks_on_scope_exit)
{
    auto tracker = MakeSharedProgressTracker(3);
    {
        MarkTrackerAsCompleted marker(tracker);
        tracker->Advance(2);
        EXPECT_FALSE(tracker->IsCompleted());
    }
    EXPECT_TRUE(tracker->IsCompleted());
    EXPECT_EQ(tracker->GetCompletedSteps(), 3);
}

TEST(MagicProgressTrackerTest, advance_tracker_advances_on_scope_exit)
{
    auto tracker = MakeSharedProgressTracker(4);
    {
        AdvanceTracker advancer(tracker, 2);
        tracker->Advance(1);
        EXPECT_EQ(tracker->GetCompletedSteps(), 1);
    }
    EXPECT_EQ(tracker->GetCompletedSteps(), 3);
    EXPECT_FALSE(tracker->IsCompleted());
    {
        AdvanceTracker advancer(tracker, 1);
    }
    EXPECT_TRUE(tracker->IsCompleted());
    EXPECT_EQ(tracker->GetCompletedSteps(), 4);
}
