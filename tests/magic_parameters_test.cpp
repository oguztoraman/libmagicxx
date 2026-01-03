/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <random>

#include "magic.hpp"

using namespace Recognition;

struct MagicParametersTest : testing::Test {
protected:
    MagicParametersTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .Open(Magic::Flags::Mime, std::nothrow));
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::error_code error_code;
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, error_code));
    }

    void SetUp() override
    {
        using enum Magic::Parameters;
        m_test_parameters = {
            {IndirMax,      m_distribution(m_engine)},
            {NameMax,       m_distribution(m_engine)},
            {ElfPhnumMax,  m_distribution(m_engine)},
            {ElfShnumMax,  m_distribution(m_engine)},
            {ElfNotesMax,  m_distribution(m_engine)},
            {RegexMax,      m_distribution(m_engine)},
            {BytesMax,      m_distribution(m_engine)},
            {EncodingMax,   m_distribution(m_engine)},
            {ElfShsizeMax, m_distribution(m_engine)},
            {MagWarnMax,   m_distribution(m_engine)}
        };
    }

    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    Magic                 m_closed_magic{};
    Magic                 m_opened_magic_without_database;
    Magic m_valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
    Magic::ParameterValueMapT               m_test_parameters{};
    std::mt19937                               m_engine{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_distribution{0, 100};
};

TEST_F(MagicParametersTest, ClosedMagicSetParameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_THROW(
            m_closed_magic.SetParameter(parameter, parameter_value),
            MagicIsClosed
        );
    }
}

TEST_F(MagicParametersTest, ClosedMagicSetParameterNoexcept)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_FALSE(m_closed_magic.SetParameter(
            parameter,
            parameter_value,
            std::nothrow
        ));
    }
}

TEST_F(MagicParametersTest, ClosedMagicSetParameters)
{
    EXPECT_THROW(
        m_closed_magic.SetParameters(m_test_parameters),
        MagicIsClosed
    );
}

TEST_F(MagicParametersTest, ClosedMagicSetParametersNoexcept)
{
    EXPECT_FALSE(m_closed_magic.SetParameters(m_test_parameters, std::nothrow)
    );
}

TEST_F(MagicParametersTest, ClosedMagicGetParameters)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.GetParameters()),
        MagicIsClosed
    );
}

TEST_F(MagicParametersTest, ClosedMagicGetParametersNoexcept)
{
    EXPECT_FALSE(m_closed_magic.GetParameters(std::nothrow));
}

TEST_F(MagicParametersTest, OpenedMagicWithoutDatabaseSetParameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        m_opened_magic_without_database.SetParameter(
            parameter,
            parameter_value
        );
        EXPECT_EQ(
            parameter_value,
            m_opened_magic_without_database.GetParameter(parameter)
        );
    }
}

TEST_F(
    MagicParametersTest,
    OpenedMagicWithoutDatabaseSetParameterNoexcept
)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_TRUE(m_opened_magic_without_database
                        .SetParameter(parameter, parameter_value, std::nothrow)
        );
        EXPECT_EQ(
            parameter_value,
            m_opened_magic_without_database
                .GetParameter(parameter, std::nothrow)
        );
    }
}

TEST_F(MagicParametersTest, OpenedMagicWithoutDatabaseSetParameters)
{
    m_opened_magic_without_database.SetParameters(m_test_parameters);
    EXPECT_EQ(
        m_test_parameters,
        m_opened_magic_without_database.GetParameters()
    );
}

TEST_F(
    MagicParametersTest,
    OpenedMagicWithoutDatabaseSetParametersNoexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .SetParameters(m_test_parameters, std::nothrow));
    EXPECT_EQ(
        m_test_parameters,
        m_opened_magic_without_database.GetParameters(std::nothrow)
    );
}

TEST_F(MagicParametersTest, ValidMagicSetParameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        m_valid_magic.SetParameter(parameter, parameter_value);
        EXPECT_EQ(parameter_value, m_valid_magic.GetParameter(parameter));
    }
}

TEST_F(MagicParametersTest, ValidMagicSetParameterNoexcept)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_TRUE(m_valid_magic
                        .SetParameter(parameter, parameter_value, std::nothrow)
        );
        EXPECT_EQ(
            parameter_value,
            m_valid_magic.GetParameter(parameter, std::nothrow)
        );
    }
}

TEST_F(MagicParametersTest, ValidMagicSetParameters)
{
    m_valid_magic.SetParameters(m_test_parameters);
    EXPECT_EQ(m_test_parameters, m_valid_magic.GetParameters());
}

TEST_F(MagicParametersTest, ValidMagicSetParametersNoexcept)
{
    EXPECT_TRUE(m_valid_magic.SetParameters(m_test_parameters, std::nothrow));
    EXPECT_EQ(m_test_parameters, m_valid_magic.GetParameters(std::nothrow));
}
