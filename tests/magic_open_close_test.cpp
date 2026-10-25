/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_open_close_test.cpp
 * @brief Unit tests for Magic::Open() and Magic::Close() methods.
 *
 * Tests the Magic instance lifecycle including:
 * - Opening with flag masks
 * - Closing and reopening instances
 * - State queries (IsOpen, IsValid, IsDatabaseLoaded)
 * - Throwing and noexcept variants
 *
 * @section open_close_test_strategy Test Strategy
 *
 * Tests use randomly generated flag combinations to verify:
 * - Opening succeeds with arbitrary valid flags
 * - Close() is idempotent (can be called multiple times)
 * - State transitions are consistent
 *
 * @section open_close_test_lifecycle Magic Lifecycle
 *
 * @code
 * [Closed] --Open()--> [Opened] --LoadDatabaseFile()--> [Valid]
 *    ^                    |                                |
 *    |                    v                                v
 *    +<-----Close()-------+<-----------Close()--------------+
 * @endcode
 *
 * @see Magic::Open()
 * @see Magic::Close()
 * @see Magic::IsOpen()
 */

#include <gtest/gtest.h>

#include <bit>
#include <cstddef>
#include <random>
#include <utility>

#include "magic.hpp"

using namespace Recognition;

struct MagicOpenCloseTest : testing::Test {
protected:
    MagicOpenCloseTest()
    {
        std::error_code error_code;
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, error_code));
    }

    void SetUp() override
    {
        m_test_flags_mask = Magic::FlagsMaskT{};
        for (std::size_t i{}; i < RANDOM_FLAG_COUNT; ++i) {
            m_test_flags_mask = m_test_flags_mask
                              | static_cast<Magic::FlagsT>(1U << m_dist(m_eng));
        }
    }

    static constexpr std::size_t FLAG_BIT_COUNT{
        std::bit_width(std::to_underlying(Magic::FlagsT::NoCheckBuiltin))
    };

    static constexpr std::size_t RANDOM_FLAG_COUNT{7UZ};

    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    Magic::FlagsMaskT     m_test_flags_mask{};
    std::mt19937          m_eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_dist{0, FLAG_BIT_COUNT - 1};
    const std::size_t                          m_multiple_test_count{5UZ};
};

TEST_F(MagicOpenCloseTest, closed_magic)
{
    Magic closed_magic{};
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicOpenCloseTest, closed_magic_close)
{
    Magic closed_magic{};
    closed_magic.Close();
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicOpenCloseTest, closed_magic_close_multiple_times)
{
    Magic closed_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        closed_magic.Close();
    }
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicOpenCloseTest, open_via_flags_mask)
{
    Magic test_magic{};
    test_magic.Open(m_test_flags_mask);
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicOpenCloseTest, open_via_flags_mask_noexcept)
{
    Magic test_magic{};
    EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicOpenCloseTest, open_multiple_times)
{
    Magic test_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
    }
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicOpenCloseTest, open_close_multiple_times)
{
    Magic test_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        EXPECT_FALSE(test_magic.IsOpen());
        EXPECT_FALSE(test_magic.IsDatabaseLoaded());
        EXPECT_FALSE(test_magic.IsValid());
        EXPECT_FALSE(test_magic);
        EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
        EXPECT_TRUE(test_magic.IsOpen());
        EXPECT_FALSE(test_magic.IsDatabaseLoaded());
        EXPECT_FALSE(test_magic.IsValid());
        EXPECT_FALSE(test_magic);
        EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
        test_magic.Close();
    }
}

TEST_F(MagicOpenCloseTest, close_valid_magic)
{
    Magic test_magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_TRUE(test_magic.IsDatabaseLoaded());
    EXPECT_TRUE(test_magic.IsValid());
    EXPECT_TRUE(test_magic);
    EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
    test_magic.Close();
    EXPECT_FALSE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
}

TEST_F(MagicOpenCloseTest, open_valid_magic)
{
    Magic test_magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_TRUE(test_magic.IsDatabaseLoaded());
    EXPECT_TRUE(test_magic.IsValid());
    EXPECT_TRUE(test_magic);
    EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
    EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_EQ(m_test_flags_mask, test_magic.GetFlags(std::nothrow).value());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
}
