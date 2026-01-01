/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <random>

#include "magic.hpp"

using namespace recognition;

struct magic_parameters_test : testing::Test {
protected:
    magic_parameters_test()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .open(magic::flags::mime, std::nothrow));
        EXPECT_TRUE(m_valid_magic.is_valid());
        std::error_code error_code;
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, error_code));
    }

    void SetUp() override
    {
        using enum magic::parameters;
        m_test_parameters = {
            {indir_max,      m_distribution(m_engine)},
            {name_max,       m_distribution(m_engine)},
            {elf_phnum_max,  m_distribution(m_engine)},
            {elf_shnum_max,  m_distribution(m_engine)},
            {elf_notes_max,  m_distribution(m_engine)},
            {regex_max,      m_distribution(m_engine)},
            {bytes_max,      m_distribution(m_engine)},
            {encoding_max,   m_distribution(m_engine)},
            {elf_shsize_max, m_distribution(m_engine)},
            {mag_warn_max,   m_distribution(m_engine)}
        };
    }

    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    magic                 m_closed_magic{};
    magic                 m_opened_magic_without_database;
    magic m_valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
    magic::parameter_value_map_t               m_test_parameters{};
    std::mt19937                               m_engine{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_distribution{0, 100};
};

TEST_F(magic_parameters_test, closed_magic_set_parameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_THROW(
            m_closed_magic.set_parameter(parameter, parameter_value),
            magic_is_closed
        );
    }
}

TEST_F(magic_parameters_test, closed_magic_set_parameter_noexcept)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_FALSE(m_closed_magic.set_parameter(
            parameter,
            parameter_value,
            std::nothrow
        ));
    }
}

TEST_F(magic_parameters_test, closed_magic_set_parameters)
{
    EXPECT_THROW(
        m_closed_magic.set_parameters(m_test_parameters),
        magic_is_closed
    );
}

TEST_F(magic_parameters_test, closed_magic_set_parameters_noexcept)
{
    EXPECT_FALSE(m_closed_magic.set_parameters(m_test_parameters, std::nothrow)
    );
}

TEST_F(magic_parameters_test, closed_magic_get_parameters)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.get_parameters()),
        magic_is_closed
    );
}

TEST_F(magic_parameters_test, closed_magic_get_parameters_noexcept)
{
    EXPECT_FALSE(m_closed_magic.get_parameters(std::nothrow));
}

TEST_F(magic_parameters_test, opened_magic_without_database_set_parameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        m_opened_magic_without_database.set_parameter(
            parameter,
            parameter_value
        );
        EXPECT_EQ(
            parameter_value,
            m_opened_magic_without_database.get_parameter(parameter)
        );
    }
}

TEST_F(
    magic_parameters_test,
    opened_magic_without_database_set_parameter_noexcept
)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_TRUE(m_opened_magic_without_database
                        .set_parameter(parameter, parameter_value, std::nothrow)
        );
        EXPECT_EQ(
            parameter_value,
            m_opened_magic_without_database
                .get_parameter(parameter, std::nothrow)
        );
    }
}

TEST_F(magic_parameters_test, opened_magic_without_database_set_parameters)
{
    m_opened_magic_without_database.set_parameters(m_test_parameters);
    EXPECT_EQ(
        m_test_parameters,
        m_opened_magic_without_database.get_parameters()
    );
}

TEST_F(
    magic_parameters_test,
    opened_magic_without_database_set_parameters_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .set_parameters(m_test_parameters, std::nothrow));
    EXPECT_EQ(
        m_test_parameters,
        m_opened_magic_without_database.get_parameters(std::nothrow)
    );
}

TEST_F(magic_parameters_test, valid_magic_set_parameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        m_valid_magic.set_parameter(parameter, parameter_value);
        EXPECT_EQ(parameter_value, m_valid_magic.get_parameter(parameter));
    }
}

TEST_F(magic_parameters_test, valid_magic_set_parameter_noexcept)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_TRUE(m_valid_magic
                        .set_parameter(parameter, parameter_value, std::nothrow)
        );
        EXPECT_EQ(
            parameter_value,
            m_valid_magic.get_parameter(parameter, std::nothrow)
        );
    }
}

TEST_F(magic_parameters_test, valid_magic_set_parameters)
{
    m_valid_magic.set_parameters(m_test_parameters);
    EXPECT_EQ(m_test_parameters, m_valid_magic.get_parameters());
}

TEST_F(magic_parameters_test, valid_magic_set_parameters_noexcept)
{
    EXPECT_TRUE(m_valid_magic.set_parameters(m_test_parameters, std::nothrow));
    EXPECT_EQ(m_test_parameters, m_valid_magic.get_parameters(std::nothrow));
}
