/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags_test.cpp
 * @brief Unit tests for Magic flag operations.
 *
 * Tests the flag getting and setting functionality including:
 * - SetFlags() with bitmask and container variants
 * - GetFlags() retrieval
 * - Both throwing and noexcept overloads
 * - Behavior on closed, opened, and valid Magic instances
 *
 * @section flags_test_strategy Test Strategy
 *
 * Tests use randomly generated flag combinations to ensure
 * robust coverage of the flag space. Each test verifies:
 * - Correct exception throwing for closed Magic
 * - Proper return values for noexcept variants
 * - Round-trip consistency (set then get)
 *
 * @see Magic::Flags
 * @see Magic::SetFlags()
 * @see Magic::GetFlags()
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <utility>

#include "magic.hpp"

using namespace Recognition;

struct MagicFlagsTest : testing::Test {
protected:
    MagicFlagsTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .Open(Magic::Flags::Mime, std::nothrow));
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::error_code error_code;
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, error_code));
    }

    void SetUp() override
    {
        std::vector<Magic::Flags> test_flags{
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng)),
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng)),
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng)),
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng)),
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng)),
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng)),
            static_cast<Magic::Flags>(1ULL << m_dist(m_eng))
        };
        std::ranges::sort(
            test_flags,
            [](Magic::Flags a, Magic::Flags b) {
                return std::to_underlying(a) < std::to_underlying(b);
            }
        );
        m_test_flags_container.clear();
        m_test_flags_container.assign(test_flags.begin(), test_flags.end());
        m_test_flags_container.erase(
            std::unique(
                m_test_flags_container.begin(),
                m_test_flags_container.end()
            ),
            m_test_flags_container.end()
        );
        m_test_flags_mask = std::ranges::fold_left(
            m_test_flags_container,
            Magic::FlagsMaskT{},
            [](Magic::FlagsMaskT acc, Magic::Flags f) { return acc | f; }
        );
    }

    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    Magic                 m_closed_magic{};
    Magic                 m_opened_magic_without_database;
    Magic m_valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
    Magic::FlagsContainerT                   m_test_flags_container{};
    Magic::FlagsMaskT                        m_test_flags_mask{};
    std::mt19937                               m_eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_dist{
        0,
        Magic::FlagsMaskT{}.size() - 1
    };
};

TEST_F(MagicFlagsTest, closed_magic_set_flags_mask)
{
    EXPECT_THROW(m_closed_magic.SetFlags(m_test_flags_mask), MagicIsClosed);
}

TEST_F(MagicFlagsTest, closed_magic_set_flags_mask_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_mask, std::nothrow));
}

TEST_F(MagicFlagsTest, closed_magic_set_flags_container)
{
    EXPECT_THROW(
        m_closed_magic.SetFlags(m_test_flags_container),
        MagicIsClosed
    );
}

TEST_F(MagicFlagsTest, closed_magic_set_flags_container_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_container, std::nothrow)
    );
}

TEST_F(MagicFlagsTest, closed_magic_get_flags)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.GetFlags()),
        MagicIsClosed
    );
}

TEST_F(MagicFlagsTest, closed_magic_get_flags_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_mask, std::nothrow));
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_mask)
{
    EXPECT_NO_THROW(m_opened_magic_without_database.SetFlags(m_test_flags_mask)
    );
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags()
    );
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_mask_noexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .SetFlags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_container)
{
    EXPECT_NO_THROW(
        m_opened_magic_without_database.SetFlags(m_test_flags_container)
    );
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags()
    );
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_container_noexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .SetFlags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, valid_magic_flags_mask)
{
    EXPECT_NO_THROW(m_valid_magic.SetFlags(m_test_flags_mask));
    EXPECT_EQ(m_test_flags_container, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, valid_magic_flags_mask_noexcept)
{
    EXPECT_TRUE(m_valid_magic.SetFlags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_valid_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, valid_magic_flags_container)
{
    EXPECT_NO_THROW(m_valid_magic.SetFlags(m_test_flags_container));
    EXPECT_EQ(m_test_flags_container, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, valid_magic_flags_container_noexcept)
{
    EXPECT_TRUE(m_valid_magic.SetFlags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_valid_magic.GetFlags(std::nothrow).value()
    );
}
