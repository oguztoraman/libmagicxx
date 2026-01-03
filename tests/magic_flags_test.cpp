/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

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

TEST_F(MagicFlagsTest, ClosedMagicSetFlagsMask)
{
    EXPECT_THROW(m_closed_magic.SetFlags(m_test_flags_mask), MagicIsClosed);
}

TEST_F(MagicFlagsTest, ClosedMagicSetFlagsMaskNoexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_mask, std::nothrow));
}

TEST_F(MagicFlagsTest, ClosedMagicSetFlagsContainer)
{
    EXPECT_THROW(
        m_closed_magic.SetFlags(m_test_flags_container),
        MagicIsClosed
    );
}

TEST_F(MagicFlagsTest, ClosedMagicSetFlagsContainerNoexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_container, std::nothrow)
    );
}

TEST_F(MagicFlagsTest, ClosedMagicGetFlags)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.GetFlags()),
        MagicIsClosed
    );
}

TEST_F(MagicFlagsTest, ClosedMagicGetFlagsNoexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_mask, std::nothrow));
}

TEST_F(MagicFlagsTest, OpenedMagicWithoutDatabaseFlagsMask)
{
    EXPECT_NO_THROW(m_opened_magic_without_database.SetFlags(m_test_flags_mask)
    );
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags()
    );
}

TEST_F(MagicFlagsTest, OpenedMagicWithoutDatabaseFlagsMaskNoexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .SetFlags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, OpenedMagicWithoutDatabaseFlagsContainer)
{
    EXPECT_NO_THROW(
        m_opened_magic_without_database.SetFlags(m_test_flags_container)
    );
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags()
    );
}

TEST_F(MagicFlagsTest, OpenedMagicWithoutDatabaseFlagsContainerNoexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .SetFlags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, ValidMagicFlagsMask)
{
    EXPECT_NO_THROW(m_valid_magic.SetFlags(m_test_flags_mask));
    EXPECT_EQ(m_test_flags_container, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, ValidMagicFlagsMaskNoexcept)
{
    EXPECT_TRUE(m_valid_magic.SetFlags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_valid_magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, ValidMagicFlagsContainer)
{
    EXPECT_NO_THROW(m_valid_magic.SetFlags(m_test_flags_container));
    EXPECT_EQ(m_test_flags_container, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, ValidMagicFlagsContainerNoexcept)
{
    EXPECT_TRUE(m_valid_magic.SetFlags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_valid_magic.GetFlags(std::nothrow).value()
    );
}
