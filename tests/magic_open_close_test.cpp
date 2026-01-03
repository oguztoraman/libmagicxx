/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <random>

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
        std::vector<unsigned long long> test_flags{
            1ULL << m_dist(m_eng),
            1ULL << m_dist(m_eng),
            1ULL << m_dist(m_eng),
            1ULL << m_dist(m_eng),
            1ULL << m_dist(m_eng),
            1ULL << m_dist(m_eng),
            1ULL << m_dist(m_eng)
        };
        std::ranges::sort(test_flags);
        m_test_flags_container.clear();
        std::ranges::transform(
            test_flags,
            std::back_inserter(m_test_flags_container),
            [](unsigned long long value) {
                return static_cast<Magic::Flags>(value);
            }
        );
        m_test_flags_container.erase(
            std::unique(
                m_test_flags_container.begin(),
                m_test_flags_container.end()
            ),
            m_test_flags_container.end()
        );
        m_test_flags_mask = std::ranges::fold_left(
            test_flags.begin(),
            test_flags.end(),
            test_flags.front(),
            std::bit_or<decltype(1ULL)>{}
        );
    }

    std::filesystem::path    m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    Magic::FlagsContainerT m_test_flags_container{};
    Magic::FlagsMaskT      m_test_flags_mask{};
    std::mt19937             m_eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_dist{
        0,
        Magic::FlagsMaskT{}.size() - 1
    };
    const std::size_t m_multiple_test_count{5uz};
};

TEST_F(MagicOpenCloseTest, ClosedMagic)
{
    Magic closed_magic{};
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicOpenCloseTest, ClosedMagicClose)
{
    Magic closed_magic{};
    closed_magic.Close();
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicOpenCloseTest, ClosedMagicCloseMultipleTimes)
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

TEST_F(MagicOpenCloseTest, OpenViaFlagsContainer)
{
    Magic test_magic{};
    test_magic.Open(m_test_flags_container);
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicOpenCloseTest, OpenViaFlagsContainerNoexcept)
{
    Magic test_magic{};
    EXPECT_TRUE(test_magic.Open(m_test_flags_container, std::nothrow));
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicOpenCloseTest, OpenViaFlagsMask)
{
    Magic test_magic{};
    test_magic.Open(m_test_flags_mask);
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicOpenCloseTest, OpenViaFlagsMaskNoexcept)
{
    Magic test_magic{};
    EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicOpenCloseTest, OpenMultipleTimes)
{
    Magic test_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
    }
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicOpenCloseTest, OpenCloseMultipleTimes)
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
        EXPECT_EQ(
            m_test_flags_container,
            test_magic.GetFlags(std::nothrow).value()
        );
        test_magic.Close();
    }
}

TEST_F(MagicOpenCloseTest, CloseValidMagic)
{
    Magic test_magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_TRUE(test_magic.IsDatabaseLoaded());
    EXPECT_TRUE(test_magic.IsValid());
    EXPECT_TRUE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
    test_magic.Close();
    EXPECT_FALSE(test_magic.IsOpen());
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
}

TEST_F(MagicOpenCloseTest, OpenValidMagic)
{
    Magic test_magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_TRUE(test_magic.IsDatabaseLoaded());
    EXPECT_TRUE(test_magic.IsValid());
    EXPECT_TRUE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
    EXPECT_TRUE(test_magic.Open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(test_magic.IsOpen());
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.GetFlags(std::nothrow).value()
    );
    EXPECT_FALSE(test_magic.IsDatabaseLoaded());
    EXPECT_FALSE(test_magic.IsValid());
    EXPECT_FALSE(test_magic);
}
