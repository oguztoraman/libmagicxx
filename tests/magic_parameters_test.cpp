/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters_test.cpp
 * @brief Unit tests for MagicParameters and Magic parameter operations.
 *
 * Tests the MagicParameters value class and the parameter getting and
 * setting functionality of Magic, including:
 * - Every MagicParameters constructor (default, parameter/value, map) and
 *   the trivially noexcept special members
 * - ToString() name lookup, formats, ordering, separators, and the empty
 *   snapshot
 * - SetParameter()/SetParameters() and GetParameter()/GetParameters()
 * - Both throwing and noexcept overloads
 * - Behavior on closed and valid Magic instances
 *
 * @section params_test_strategy Test Strategy
 *
 * The MagicParametersConvertTest fixture covers the standalone conversions
 * directly; the MagicParametersTest fixture uses randomly generated
 * parameter values to verify:
 * - Parameters can be set on opened Magic instances
 * - Closed Magic throws appropriate exceptions
 * - Round-trip consistency (set then get), which also proves the internal
 *   libmagic constant mapping works for every parameter
 *
 * @section params_test_parameters Parameters Tested
 *
 * All Magic::ParametersT enum values are tested:
 * - IndirMax, NameMax, ElfPhnumMax, ElfShnumMax
 * - ElfNotesMax, RegexMax, BytesMax, EncodingMax
 * - ElfShsizeMax, MagWarnMax
 *
 * @see MagicParameters
 * @see Magic::ParametersT
 * @see Magic::SetParameter()
 * @see Magic::GetParameter()
 */

#include <gtest/gtest.h>

#include <cstddef>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "magic.hpp"

using namespace Recognition;

struct MagicParametersTest : testing::Test {
protected:
    MagicParametersTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database.Open(
            Magic::FlagsT::Mime,
            std::nothrow
        ));
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::error_code error_code;
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, error_code));
    }

    void SetUp() override
    {
        using enum Magic::ParametersT;
        m_test_parameters = {
            {IndirMax,     m_distribution(m_engine)},
            {NameMax,      m_distribution(m_engine)},
            {ElfPhnumMax,  m_distribution(m_engine)},
            {ElfShnumMax,  m_distribution(m_engine)},
            {ElfNotesMax,  m_distribution(m_engine)},
            {RegexMax,     m_distribution(m_engine)},
            {BytesMax,     m_distribution(m_engine)},
            {EncodingMax,  m_distribution(m_engine)},
            {ElfShsizeMax, m_distribution(m_engine)},
            {MagWarnMax,   m_distribution(m_engine)}
        };
    }

    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    Magic                 m_closed_magic{};
    Magic                 m_opened_magic_without_database;
    Magic m_valid_magic{Magic::FlagsT::Mime, std::nothrow, m_valid_database};
    Magic::ParameterValueMapT                  m_test_parameters{};
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
    EXPECT_FALSE(m_closed_magic.SetParameters(m_test_parameters, std::nothrow));
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
        EXPECT_TRUE(m_opened_magic_without_database.SetParameter(
            parameter,
            parameter_value,
            std::nothrow
        ));
        EXPECT_EQ(
            parameter_value,
            m_opened_magic_without_database.GetParameter(
                parameter,
                std::nothrow
            )
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
    EXPECT_TRUE(m_opened_magic_without_database.SetParameters(
        m_test_parameters,
        std::nothrow
    ));
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
        EXPECT_TRUE(
            m_valid_magic.SetParameter(parameter, parameter_value, std::nothrow)
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

TEST_F(MagicParametersTest, get_parameters_render_through_magic_parameters)
{
    m_valid_magic.SetParameters(m_test_parameters);
    const MagicParameters rendered{m_valid_magic.GetParameters()};
    for (const auto& [parameter, parameter_value] : m_test_parameters) {
        EXPECT_NE(
            std::string::npos,
            rendered.ToString(MagicParameters::StringFormat::Names, ": ", ",")
                .find((MagicParameters{parameter, parameter_value}.ToString(
                    MagicParameters::StringFormat::Names,
                    ": ",
                    ", "
                )))
        );
        EXPECT_EQ(parameter_value, m_valid_magic.GetParameter(parameter));
    }
}

TEST_F(MagicParametersTest, get_parameters_names_via_magic_parameters)
{
    const MagicParameters parameters{m_valid_magic.GetParameters()};
    EXPECT_EQ(
        "IndirMax,NameMax,ElfPhnumMax,ElfShnumMax,ElfNotesMax,RegexMax,"
        "BytesMax,EncodingMax,ElfShsizeMax,MagWarnMax",
        parameters.ToString(MagicParameters::StringFormat::Names, ": ", ",")
    );
}

struct MagicParametersConvertTest : testing::Test {
    static const inline std::vector<
        std::pair<MagicParameters::Parameters, std::string>
    >
        ALL_PARAMETERS{
            {MagicParameters::Parameters::IndirMax,     "IndirMax"    },
            {MagicParameters::Parameters::NameMax,      "NameMax"     },
            {MagicParameters::Parameters::ElfPhnumMax,  "ElfPhnumMax" },
            {MagicParameters::Parameters::ElfShnumMax,  "ElfShnumMax" },
            {MagicParameters::Parameters::ElfNotesMax,  "ElfNotesMax" },
            {MagicParameters::Parameters::RegexMax,     "RegexMax"    },
            {MagicParameters::Parameters::BytesMax,     "BytesMax"    },
            {MagicParameters::Parameters::EncodingMax,  "EncodingMax" },
            {MagicParameters::Parameters::ElfShsizeMax, "ElfShsizeMax"},
            {MagicParameters::Parameters::MagWarnMax,   "MagWarnMax"  }
    };

    static MagicParameters::ParameterValueMapT FullValueMap()
    {
        MagicParameters::ParameterValueMapT map;
        for (std::size_t i{}; i < ALL_PARAMETERS.size(); ++i) {
            map[ALL_PARAMETERS.at(i).first] = i + 1UZ;
        }
        return map;
    }
};

TEST(MagicParametersClassTest, satisfies_value_class_properties)
{
    static_assert(std::is_trivially_copyable_v<MagicParameters>);
    static_assert(std::is_nothrow_default_constructible_v<MagicParameters>);
    static_assert(std::is_nothrow_copy_constructible_v<MagicParameters>);
    static_assert(std::is_nothrow_move_constructible_v<MagicParameters>);
    static_assert(std::is_nothrow_copy_assignable_v<MagicParameters>);
    static_assert(std::is_nothrow_move_assignable_v<MagicParameters>);
    static_assert(std::is_nothrow_destructible_v<MagicParameters>);
    {
        const MagicParameters::ParameterValueMapT map{
            {MagicParameters::Parameters::BytesMax, 7UZ}
        };
        static_assert(
            std::bool_constant<noexcept(
                MagicParameters{MagicParameters::Parameters::BytesMax, 7UZ}
            )>::value
            && std::bool_constant<noexcept(MagicParameters{map})>::value
        );
        (void)map;
    }
}

TEST_F(MagicParametersConvertTest, name_of_every_parameter)
{
    for (const auto& [parameter, name] : ALL_PARAMETERS) {
        EXPECT_EQ(
            name,
            (MagicParameters{parameter, 0UZ}.ToString(
                MagicParameters::StringFormat::Names,
                ": ",
                ","
            ))
        );
        EXPECT_EQ(name + ": 0", (MagicParameters{parameter, 0UZ}.ToString()));
        EXPECT_EQ(name + ": 7", (MagicParameters{parameter, 7UZ}.ToString()));
    }
}

TEST_F(MagicParametersConvertTest, default_and_empty_map_are_empty)
{
    const MagicParameters default_parameters;
    EXPECT_TRUE(default_parameters.ToString().empty());
    EXPECT_TRUE(default_parameters
                    .ToString(MagicParameters::StringFormat::Names, ": ", ",")
                    .empty());
    const MagicParameters empty_parameters{
        MagicParameters::ParameterValueMapT{}
    };
    EXPECT_TRUE(empty_parameters.ToString().empty());
}

TEST_F(MagicParametersConvertTest, two_entry_map_orders_by_ordinal)
{
    const MagicParameters::ParameterValueMapT map{
        {MagicParameters::Parameters::BytesMax, 1'048'576UZ},
        {MagicParameters::Parameters::RegexMax, 8'192UZ    }
    };
    const MagicParameters parameters{map};
    EXPECT_EQ("RegexMax: 8192, BytesMax: 1048576", parameters.ToString());
}

TEST_F(MagicParametersConvertTest, to_string_renders_every_parameter)
{
    const MagicParameters parameters{FullValueMap()};
    EXPECT_EQ(
        "IndirMax: 1, NameMax: 2, ElfPhnumMax: 3, ElfShnumMax: 4, "
        "ElfNotesMax: 5, RegexMax: 6, BytesMax: 7, EncodingMax: 8, "
        "ElfShsizeMax: 9, MagWarnMax: 10",
        parameters.ToString()
    );
    EXPECT_EQ(
        "IndirMax,NameMax,ElfPhnumMax,ElfShnumMax,ElfNotesMax,RegexMax,"
        "BytesMax,EncodingMax,ElfShsizeMax,MagWarnMax",
        parameters.ToString(MagicParameters::StringFormat::Names, ": ", ",")
    );
}

TEST_F(MagicParametersConvertTest, to_string_custom_separators)
{
    const MagicParameters parameters{FullValueMap()};
    EXPECT_EQ(
        "IndirMax=1;NameMax=2;ElfPhnumMax=3;ElfShnumMax=4;ElfNotesMax=5;"
        "RegexMax=6;BytesMax=7;EncodingMax=8;ElfShsizeMax=9;MagWarnMax=10",
        parameters.ToString(
            MagicParameters::StringFormat::NamesAndValues,
            "=",
            ";"
        )
    );
    EXPECT_EQ(
        "IndirMax|NameMax|ElfPhnumMax|ElfShnumMax|ElfNotesMax|RegexMax|"
        "BytesMax|EncodingMax|ElfShsizeMax|MagWarnMax",
        parameters.ToString(MagicParameters::StringFormat::Names, "=", "|")
    );
    EXPECT_EQ(
        "IndirMax -> 1 and NameMax -> 2 and ElfPhnumMax -> 3 and "
        "ElfShnumMax -> 4 and ElfNotesMax -> 5 and RegexMax -> 6 and "
        "BytesMax -> 7 and EncodingMax -> 8 and ElfShsizeMax -> 9 and "
        "MagWarnMax -> 10",
        parameters.ToString(
            MagicParameters::StringFormat::NamesAndValues,
            " -> ",
            " and "
        )
    ); // multi-character separators
}

TEST_F(MagicParametersConvertTest, copy_and_move_preserve_snapshot)
{
    const MagicParameters::ParameterValueMapT map{
        {MagicParameters::Parameters::BytesMax, 7UZ    },
        {MagicParameters::Parameters::RegexMax, 8'192UZ}
    };
    MagicParameters       original{map};
    const MagicParameters copy{original};
    EXPECT_EQ(original.ToString(), copy.ToString());

    const MagicParameters moved{std::move(original)};
    EXPECT_EQ("RegexMax: 8192, BytesMax: 7", moved.ToString());
}
