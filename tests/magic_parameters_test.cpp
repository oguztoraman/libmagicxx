/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters_test.cpp
 * @brief Unit tests for Magic parameter operations.
 *
 * Tests the parameter getting and setting functionality including:
 * - SetParameter() for individual parameters
 * - SetParameters() for bulk parameter setting
 * - GetParameter() and GetParameters() retrieval
 * - Both throwing and noexcept overloads
 *
 * @section params_test_strategy Test Strategy
 *
 * Tests use randomly generated parameter values to verify:
 * - Parameters can be set on opened Magic instances
 * - Closed Magic throws appropriate exceptions
 * - Round-trip consistency (set then get)
 *
 * @section params_test_parameters Parameters Tested
 *
 * All Magic::Parameters enum values are tested:
 * - IndirMax, NameMax, ElfPhnumMax, ElfShnumMax
 * - ElfNotesMax, RegexMax, BytesMax, EncodingMax
 * - ElfShsizeMax, MagWarnMax
 *
 * @see Magic::Parameters
 * @see Magic::SetParameter()
 * @see Magic::GetParameter()
 */

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

TEST_F(MagicParametersTest, closed_magic_set_parameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_THROW(
            m_closed_magic.SetParameter(parameter, parameter_value),
            MagicIsClosed
        );
    }
}

TEST_F(MagicParametersTest, closed_magic_set_parameter_noexcept)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_FALSE(m_closed_magic.SetParameter(
            parameter,
            parameter_value,
            std::nothrow
        ));
    }
}

TEST_F(MagicParametersTest, closed_magic_set_parameters)
{
    EXPECT_THROW(
        m_closed_magic.SetParameters(m_test_parameters),
        MagicIsClosed
    );
}

TEST_F(MagicParametersTest, closed_magic_set_parameters_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetParameters(m_test_parameters, std::nothrow)
    );
}

TEST_F(MagicParametersTest, closed_magic_get_parameters)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.GetParameters()),
        MagicIsClosed
    );
}

TEST_F(MagicParametersTest, closed_magic_get_parameters_noexcept)
{
    EXPECT_FALSE(m_closed_magic.GetParameters(std::nothrow));
}

TEST_F(MagicParametersTest, opened_magic_without_database_set_parameter)
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
    opened_magic_without_database_set_parameter_noexcept
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

TEST_F(MagicParametersTest, opened_magic_without_database_set_parameters)
{
    m_opened_magic_without_database.SetParameters(m_test_parameters);
    EXPECT_EQ(
        m_test_parameters,
        m_opened_magic_without_database.GetParameters()
    );
}

TEST_F(
    MagicParametersTest,
    opened_magic_without_database_set_parameters_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .SetParameters(m_test_parameters, std::nothrow));
    EXPECT_EQ(
        m_test_parameters,
        m_opened_magic_without_database.GetParameters(std::nothrow)
    );
}

TEST_F(MagicParametersTest, valid_magic_set_parameter)
{
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        m_valid_magic.SetParameter(parameter, parameter_value);
        EXPECT_EQ(parameter_value, m_valid_magic.GetParameter(parameter));
    }
}

TEST_F(MagicParametersTest, valid_magic_set_parameter_noexcept)
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

TEST_F(MagicParametersTest, valid_magic_set_parameters)
{
    m_valid_magic.SetParameters(m_test_parameters);
    EXPECT_EQ(m_test_parameters, m_valid_magic.GetParameters());
}

TEST_F(MagicParametersTest, valid_magic_set_parameters_noexcept)
{
    EXPECT_TRUE(m_valid_magic.SetParameters(m_test_parameters, std::nothrow));
    EXPECT_EQ(m_test_parameters, m_valid_magic.GetParameters(std::nothrow));
}
