/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "progress_tracker.hpp"

using namespace recognition::utility;

TEST(magic_progress_tracker_test, default_constructor)
{
    progress_tracker tracker;
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 1);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 0);
}

TEST(magic_progress_tracker_test, constructor_with_step_count)
{
    progress_tracker tracker{10};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 10);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 0);
}

TEST(magic_progress_tracker_test, advance_increments)
{
    progress_tracker tracker{5};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 5);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance();
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 5);
    EXPECT_EQ(tracker.get_completed_steps(), 1);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 20);
    tracker.advance(2);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 5);
    EXPECT_EQ(tracker.get_completed_steps(), 3);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 60);
}

TEST(magic_progress_tracker_test, advance_does_not_exceed_total)
{
    progress_tracker tracker{3};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 3);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance(5);
    EXPECT_TRUE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 3);
    EXPECT_EQ(tracker.get_completed_steps(), 3);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 100);
}

TEST(magic_progress_tracker_test, get_incompleted_steps)
{
    progress_tracker tracker{10};
    EXPECT_EQ(tracker.get_incompleted_steps(), 10);
    tracker.advance(3);
    EXPECT_EQ(tracker.get_incompleted_steps(), 7);
    tracker.advance(2);
    EXPECT_EQ(tracker.get_incompleted_steps(), 5);
    tracker.advance(5);
    EXPECT_EQ(tracker.get_incompleted_steps(), 0);
}

TEST(magic_progress_tracker_test, mark_as_completed_sets_to_100)
{
    progress_tracker tracker{7};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 7);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance(3);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 7);
    EXPECT_EQ(tracker.get_completed_steps(), 3);
    tracker.mark_as_completed();
    EXPECT_TRUE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 7);
    EXPECT_EQ(tracker.get_completed_steps(), 7);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 100);
}

TEST(magic_progress_tracker_test, reset_step_count_progress)
{
    progress_tracker tracker{5};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 5);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance(3);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 5);
    EXPECT_EQ(tracker.get_completed_steps(), 3);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 60);
    tracker.reset(10);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 10);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 0);
    tracker.advance(4);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 10);
    EXPECT_EQ(tracker.get_completed_steps(), 4);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 40);
}

TEST(magic_progress_tracker_test, reset_step_count_to_zero)
{
    progress_tracker tracker{5};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 5);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.reset(0);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 1);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance();
    EXPECT_TRUE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 1);
    EXPECT_EQ(tracker.get_completed_steps(), 1);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 100);
    tracker.reset(1);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 1);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 0);
}

TEST(magic_progress_tracker_test, reset_step_count_multiple_times)
{
    progress_tracker tracker{2};
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 2);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance();
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 2);
    EXPECT_EQ(tracker.get_completed_steps(), 1);
    tracker.reset(4);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 4);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    tracker.advance(2);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 4);
    EXPECT_EQ(tracker.get_completed_steps(), 2);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 50);
    tracker.reset(3);
    EXPECT_FALSE(tracker.is_completed());
    EXPECT_EQ(tracker.get_total_steps(), 3);
    EXPECT_EQ(tracker.get_completed_steps(), 0);
    EXPECT_EQ(tracker.get_completion_percentage().get(), 0);
}

TEST(magic_progress_tracker_test, thread_safety)
{
    progress_tracker tracker{100};
    constexpr auto   number_threads   = 10;
    constexpr auto   steps_per_thread = 10;

    std::vector<std::thread> threads;
    for (int t = 0; t < number_threads; ++t) {
        threads.emplace_back([&tracker]() {
            for (int i = 0; i < steps_per_thread; ++i) {
                tracker.advance();
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(tracker.get_completion_percentage().get(), 100);
    EXPECT_TRUE(tracker.is_completed());
    SUCCEED();
}

TEST(magic_progress_tracker_test, wait_for_completion)
{
    progress_tracker tracker{5};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        tracker.advance(5);
    });

    tracker.wait_for_completion();
    EXPECT_TRUE(tracker.is_completed());
    worker.join();
}

TEST(magic_progress_tracker_test, try_wait_for_completion_success)
{
    progress_tracker tracker{3};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        tracker.advance(3);
    });

    bool completed = tracker.try_wait_for_completion(
        std::chrono::milliseconds{500}
    );

    EXPECT_TRUE(completed);
    EXPECT_TRUE(tracker.is_completed());
    worker.join();
}

TEST(magic_progress_tracker_test, try_wait_for_completion_timeout)
{
    progress_tracker tracker{3};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        tracker.advance(3);
    });

    bool completed = tracker.try_wait_for_completion(
        std::chrono::milliseconds{100}
    );

    EXPECT_FALSE(completed);
    EXPECT_FALSE(tracker.is_completed());
    worker.join();
}

TEST(magic_progress_tracker_test, try_wait_until_success)
{
    progress_tracker tracker{2};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        tracker.advance(2);
    });

    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds{300};
    bool completed = tracker.try_wait_for_completion_until(deadline);

    EXPECT_TRUE(completed);
    EXPECT_TRUE(tracker.is_completed());
    worker.join();
}

TEST(magic_progress_tracker_test, try_wait_until_timeout)
{
    progress_tracker tracker{2};

    std::thread worker([&tracker]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{300});
        tracker.advance(2);
    });

    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds{100};
    bool completed = tracker.try_wait_for_completion_until(deadline);

    EXPECT_FALSE(completed);
    EXPECT_FALSE(tracker.is_completed());
    worker.join();
}

TEST(magic_progress_tracker_test, make_shared_progress_tracker_creates_tracker)
{
    auto tracker = make_shared_progress_tracker(5);
    EXPECT_TRUE(tracker);
    EXPECT_EQ(tracker->get_total_steps(), 5);
    EXPECT_EQ(tracker->get_completed_steps(), 0);
    EXPECT_FALSE(tracker->is_completed());
}

TEST(magic_progress_tracker_test, mark_tracker_as_completed_marks_on_scope_exit)
{
    auto tracker = make_shared_progress_tracker(3);
    {
        mark_tracker_as_completed marker(tracker);
        tracker->advance(2);
        EXPECT_FALSE(tracker->is_completed());
    }
    EXPECT_TRUE(tracker->is_completed());
    EXPECT_EQ(tracker->get_completed_steps(), 3);
}

TEST(magic_progress_tracker_test, advance_tracker_advances_on_scope_exit)
{
    auto tracker = make_shared_progress_tracker(4);
    {
        advance_tracker advancer(tracker, 2);
        tracker->advance(1);
        EXPECT_EQ(tracker->get_completed_steps(), 1);
    }
    EXPECT_EQ(tracker->get_completed_steps(), 3);
    EXPECT_FALSE(tracker->is_completed());
    {
        advance_tracker advancer(tracker, 1);
    }
    EXPECT_TRUE(tracker->is_completed());
    EXPECT_EQ(tracker->get_completed_steps(), 4);
}
