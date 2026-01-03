/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "progress_tracker.hpp"

using namespace Recognition::Utility;

TEST(MagicProgressTrackerTest, DefaultConstructor)
{
    ProgressTracker tracker;
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 1);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, ConstructorWithStepCount)
{
    ProgressTracker tracker{10};
    EXPECT_FALSE(tracker.IsCompleted());
    EXPECT_EQ(tracker.GetTotalSteps(), 10);
    EXPECT_EQ(tracker.GetCompletedSteps(), 0);
    EXPECT_EQ(tracker.GetCompletionPercentage().Get(), 0);
}

TEST(MagicProgressTrackerTest, AdvanceIncrements)
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

TEST(MagicProgressTrackerTest, AdvanceDoesNotExceedTotal)
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

TEST(MagicProgressTrackerTest, GetIncompletedSteps)
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

TEST(MagicProgressTrackerTest, MarkAsCompletedSetsTo100)
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

TEST(MagicProgressTrackerTest, ResetStepCountProgress)
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

TEST(MagicProgressTrackerTest, ResetStepCountToZero)
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

TEST(MagicProgressTrackerTest, ResetStepCountMultipleTimes)
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

TEST(MagicProgressTrackerTest, ThreadSafety)
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

TEST(MagicProgressTrackerTest, WaitForCompletion)
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

TEST(MagicProgressTrackerTest, TryWaitForCompletionSuccess)
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

TEST(MagicProgressTrackerTest, TryWaitForCompletionTimeout)
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

TEST(MagicProgressTrackerTest, TryWaitUntilSuccess)
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

TEST(MagicProgressTrackerTest, TryWaitUntilTimeout)
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

TEST(MagicProgressTrackerTest, MakeSharedProgressTrackerCreatesTracker)
{
    auto tracker = MakeSharedProgressTracker(5);
    EXPECT_TRUE(tracker);
    EXPECT_EQ(tracker->GetTotalSteps(), 5);
    EXPECT_EQ(tracker->GetCompletedSteps(), 0);
    EXPECT_FALSE(tracker->IsCompleted());
}

TEST(MagicProgressTrackerTest, MarkTrackerAsCompletedMarksOnScopeExit)
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

TEST(MagicProgressTrackerTest, AdvanceTrackerAdvancesOnScopeExit)
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
