/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <random>

#include "magic.hpp"

using namespace recognition;

struct magic_open_close_test : testing::Test {
protected:
    magic_open_close_test()
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

    std::filesystem::path    m_valid_database{DEFAULT_DATABASE_FILE};
    magic::flags_container_t m_test_flags_container{};
    magic::flags_mask_t      m_test_flags_mask{};
    std::mt19937             m_eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_dist{
        0,
        magic::flags_mask_t{}.size() - 1
    };
    const std::size_t m_multiple_test_count{5uz};
};

TEST_F(magic_open_close_test, closed_magic)
{
    magic closed_magic{};
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(magic_open_close_test, closed_magic_close)
{
    magic closed_magic{};
    closed_magic.close();
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(magic_open_close_test, closed_magic_close_multiple_times)
{
    magic closed_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        closed_magic.close();
    }
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(magic_open_close_test, open_via_flags_container)
{
    magic test_magic{};
    test_magic.open(m_test_flags_container);
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_open_close_test, open_via_flags_container_noexcept)
{
    magic test_magic{};
    EXPECT_TRUE(test_magic.open(m_test_flags_container, std::nothrow));
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_open_close_test, open_via_flags_mask)
{
    magic test_magic{};
    test_magic.open(m_test_flags_mask);
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_open_close_test, open_via_flags_mask_noexcept)
{
    magic test_magic{};
    EXPECT_TRUE(test_magic.open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_open_close_test, open_multiple_times)
{
    magic test_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        EXPECT_TRUE(test_magic.open(m_test_flags_mask, std::nothrow));
    }
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
}

TEST_F(magic_open_close_test, open_close_multiple_times)
{
    magic test_magic{};
    for (std::size_t i{}; i < m_multiple_test_count; ++i) {
        EXPECT_FALSE(test_magic.is_open());
        EXPECT_FALSE(test_magic.is_database_loaded());
        EXPECT_FALSE(test_magic.is_valid());
        EXPECT_FALSE(test_magic);
        EXPECT_TRUE(test_magic.open(m_test_flags_mask, std::nothrow));
        EXPECT_TRUE(test_magic.is_open());
        EXPECT_FALSE(test_magic.is_database_loaded());
        EXPECT_FALSE(test_magic.is_valid());
        EXPECT_FALSE(test_magic);
        EXPECT_EQ(
            m_test_flags_container,
            test_magic.get_flags(std::nothrow).value()
        );
        test_magic.close();
    }
}

TEST_F(magic_open_close_test, close_valid_magic)
{
    magic test_magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_TRUE(test_magic.is_database_loaded());
    EXPECT_TRUE(test_magic.is_valid());
    EXPECT_TRUE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
    test_magic.close();
    EXPECT_FALSE(test_magic.is_open());
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
}

TEST_F(magic_open_close_test, open_valid_magic)
{
    magic test_magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_TRUE(test_magic.is_database_loaded());
    EXPECT_TRUE(test_magic.is_valid());
    EXPECT_TRUE(test_magic);
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
    EXPECT_TRUE(test_magic.open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(test_magic.is_open());
    EXPECT_EQ(
        m_test_flags_container,
        test_magic.get_flags(std::nothrow).value()
    );
    EXPECT_FALSE(test_magic.is_database_loaded());
    EXPECT_FALSE(test_magic.is_valid());
    EXPECT_FALSE(test_magic);
}
