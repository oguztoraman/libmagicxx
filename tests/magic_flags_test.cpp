/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

#include "magic.hpp"

using namespace recognition;

struct magic_flags_test : testing::Test {
protected:
    magic_flags_test()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .open(magic::flags::mime, std::nothrow));
        EXPECT_TRUE(m_valid_magic.is_valid());
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
                return static_cast<magic::flags>(value);
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

    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    magic                 m_closed_magic{};
    magic                 m_opened_magic_without_database;
    magic m_valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
    magic::flags_container_t                   m_test_flags_container{};
    magic::flags_mask_t                        m_test_flags_mask{};
    std::mt19937                               m_eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_dist{
        0,
        magic::flags_mask_t{}.size() - 1
    };
};

TEST_F(magic_flags_test, closed_magic_set_flags_mask)
{
    EXPECT_THROW(m_closed_magic.set_flags(m_test_flags_mask), magic_is_closed);
}

TEST_F(magic_flags_test, closed_magic_set_flags_mask_noexcept)
{
    EXPECT_FALSE(m_closed_magic.set_flags(m_test_flags_mask, std::nothrow));
}

TEST_F(magic_flags_test, closed_magic_set_flags_container)
{
    EXPECT_THROW(
        m_closed_magic.set_flags(m_test_flags_container),
        magic_is_closed
    );
}

TEST_F(magic_flags_test, closed_magic_set_flags_container_noexcept)
{
    EXPECT_FALSE(m_closed_magic.set_flags(m_test_flags_container, std::nothrow)
    );
}

TEST_F(magic_flags_test, closed_magic_get_flags)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.get_flags()),
        magic_is_closed
    );
}

TEST_F(magic_flags_test, closed_magic_get_flags_noexcept)
{
    EXPECT_FALSE(m_closed_magic.set_flags(m_test_flags_mask, std::nothrow));
}

TEST_F(magic_flags_test, opened_magic_without_database_flags_mask)
{
    EXPECT_NO_THROW(m_opened_magic_without_database.set_flags(m_test_flags_mask)
    );
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.get_flags()
    );
}

TEST_F(magic_flags_test, opened_magic_without_database_flags_mask_noexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .set_flags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_flags_test, opened_magic_without_database_flags_container)
{
    EXPECT_NO_THROW(
        m_opened_magic_without_database.set_flags(m_test_flags_container)
    );
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.get_flags()
    );
}

TEST_F(magic_flags_test, opened_magic_without_database_flags_container_noexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .set_flags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_opened_magic_without_database.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_flags_test, valid_magic_flags_mask)
{
    EXPECT_NO_THROW(m_valid_magic.set_flags(m_test_flags_mask));
    EXPECT_EQ(m_test_flags_container, m_valid_magic.get_flags());
}

TEST_F(magic_flags_test, valid_magic_flags_mask_noexcept)
{
    EXPECT_TRUE(m_valid_magic.set_flags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_valid_magic.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_flags_test, valid_magic_flags_container)
{
    EXPECT_NO_THROW(m_valid_magic.set_flags(m_test_flags_container));
    EXPECT_EQ(m_test_flags_container, m_valid_magic.get_flags());
}

TEST_F(magic_flags_test, valid_magic_flags_container_noexcept)
{
    EXPECT_TRUE(m_valid_magic.set_flags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(
        m_test_flags_container,
        m_valid_magic.get_flags(std::nothrow).value()
    );
}
