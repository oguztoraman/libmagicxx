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
 * - Equality comparison via operator==
 * - Total ordering via operator<=> (entry-wise ordinal order, ordered
 *   containers)
 * - SetParameter()/SetParameters() and GetParameter()/GetParameters()
 * - Both throwing and noexcept overloads
 * - Behavior on closed and valid Magic instances
 * - Rejection of out-of-range `Parameters` ordinals, which must never read
 *   or modify another parameter
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
 * - Out-of-range ordinals fail explicitly, with the other parameters
 *   verified to stay untouched
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

#include <algorithm>
#include <compare>
#include <cstddef>
#include <map>
#include <random>
#include <set>
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

    static constexpr std::size_t INVALID_ORDINAL{99};
    static constexpr std::size_t SENTINEL_VALUE{42};
    static constexpr std::size_t SENTINEL_STEP{3};
    static constexpr std::size_t ORDINAL_COUNT{10};

    static constexpr Magic::ParametersT INVALID_PARAMETER{
        static_cast<Magic::ParametersT>(INVALID_ORDINAL)
    };

    static constexpr Magic::ParametersT FIRST_INVALID_PARAMETER{
        static_cast<Magic::ParametersT>(ORDINAL_COUNT)
    };

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
            std::string_view{
                rendered.ToString(
                    MagicParameters::StringFormat::Names,
                    ": ",
                    ","
                )
        }
                .find(
                    std::string_view{
                        MagicParameters{parameter, parameter_value}.ToString(
                            MagicParameters::StringFormat::Names,
                            ": ",
                            ", "
                        )
                    }
                )
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

TEST_F(MagicParametersTest, set_parameter_with_invalid_ordinal_throws)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    EXPECT_THROW(
        m_valid_magic.SetParameter(INVALID_PARAMETER, SENTINEL_STEP),
        MagicSetParametersError
    );
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
    );
}

TEST_F(MagicParametersTest, set_parameter_error_message_reports_ordinal)
{
    const auto report{[this]() -> std::string {
        try {
            m_valid_magic.SetParameter(INVALID_PARAMETER, SENTINEL_STEP);
        } catch (const MagicSetParametersError& error) {
            return error.what();
        }
        return {};
    }()};
    EXPECT_NE(
        std::string::npos,
        report.find(
            "Invalid parameter ordinal " + std::to_string(INVALID_ORDINAL)
        )
    );
}

TEST_F(
    MagicParametersTest,
    set_parameter_with_invalid_ordinal_noexcept_returns_false
)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    EXPECT_FALSE(m_valid_magic.SetParameter(
        INVALID_PARAMETER,
        SENTINEL_STEP,
        std::nothrow
    ));
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
    );
}

TEST_F(MagicParametersTest, get_parameter_with_invalid_ordinal_throws)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.GetParameter(INVALID_PARAMETER)),
        MagicGetParametersError
    );
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
    );
}

TEST_F(
    MagicParametersTest,
    get_parameter_with_invalid_ordinal_noexcept_returns_nullopt
)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    EXPECT_FALSE(
        m_valid_magic.GetParameter(INVALID_PARAMETER, std::nothrow).has_value()
    );
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax, std::nothrow)
    );
}

TEST_F(MagicParametersTest, get_parameter_nothrow_after_close_returns_nullopt)
{
    m_valid_magic.SetParameter(Magic::ParametersT::RegexMax, SENTINEL_VALUE);
    m_valid_magic.Close();
    EXPECT_FALSE(m_valid_magic
                     .GetParameter(Magic::ParametersT::RegexMax, std::nothrow)
                     .has_value());
}

TEST_F(MagicParametersTest, set_parameters_with_invalid_key_throws)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    const Magic::ParameterValueMapT parameters{
        {Magic::ParametersT::RegexMax, SENTINEL_STEP },
        {INVALID_PARAMETER,            SENTINEL_VALUE}
    };
    EXPECT_THROW(
        m_valid_magic.SetParameters(parameters),
        MagicSetParametersError
    );
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
    );
}

TEST_F(MagicParametersTest, set_parameters_with_invalid_key_noexcept)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    const Magic::ParameterValueMapT parameters{
        {Magic::ParametersT::RegexMax, SENTINEL_STEP },
        {INVALID_PARAMETER,            SENTINEL_VALUE}
    };
    EXPECT_FALSE(m_valid_magic.SetParameters(parameters, std::nothrow));
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
    );
}

TEST_F(MagicParametersTest, every_valid_ordinal_maps_to_own_libmagic_parameter)
{
    for (std::size_t ordinal{}; ordinal < ORDINAL_COUNT; ++ordinal) {
        const auto parameter{static_cast<Magic::ParametersT>(ordinal)};
        m_valid_magic.SetParameter(parameter, (ordinal + 1UZ) * SENTINEL_STEP);
    }
    for (std::size_t ordinal{}; ordinal < ORDINAL_COUNT; ++ordinal) {
        const auto parameter{static_cast<Magic::ParametersT>(ordinal)};
        EXPECT_EQ(
            (ordinal + 1UZ) * SENTINEL_STEP,
            m_valid_magic.GetParameter(parameter)
        );
    }
}

TEST_F(MagicParametersTest, get_parameter_error_message_reports_ordinal)
{
    const auto report{[this]() -> std::string {
        try {
            static_cast<void>(m_valid_magic.GetParameter(INVALID_PARAMETER));
        } catch (const MagicGetParametersError& error) {
            return error.what();
        }
        return {};
    }()};
    EXPECT_NE(
        std::string::npos,
        report.find(
            "Invalid parameter ordinal " + std::to_string(INVALID_ORDINAL)
        )
    );
}

TEST_F(MagicParametersTest, set_parameter_with_first_invalid_ordinal_throws)
{
    EXPECT_THROW(
        m_valid_magic.SetParameter(FIRST_INVALID_PARAMETER, SENTINEL_VALUE),
        MagicSetParametersError
    );
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.GetParameter(FIRST_INVALID_PARAMETER)),
        MagicGetParametersError
    );
}

TEST_F(MagicParametersTest, invalid_ordinal_on_closed_magic_throws_is_closed)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.GetParameter(INVALID_PARAMETER)),
        MagicIsClosed
    );
    EXPECT_THROW(
        m_closed_magic.SetParameter(INVALID_PARAMETER, SENTINEL_VALUE),
        MagicIsClosed
    );
    EXPECT_FALSE(
        m_closed_magic.GetParameter(INVALID_PARAMETER, std::nothrow).has_value()
    );
    EXPECT_FALSE(m_closed_magic.SetParameter(
        INVALID_PARAMETER,
        SENTINEL_VALUE,
        std::nothrow
    ));
}

TEST_F(
    MagicParametersTest,
    set_parameters_invalid_key_throws_after_valid_entries
)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    const Magic::ParameterValueMapT parameters{
        {Magic::ParametersT::RegexMax, SENTINEL_STEP },
        {INVALID_PARAMETER,            SENTINEL_VALUE}
    };
    EXPECT_THROW(
        m_valid_magic.SetParameters(parameters),
        MagicSetParametersError
    );
    EXPECT_EQ(
        SENTINEL_STEP,
        m_valid_magic.GetParameter(Magic::ParametersT::RegexMax)
    );
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
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

TEST(MagicParametersClassTest, supports_constexpr_composition)
{
    constexpr MagicParameters indir_15{
        MagicParameters::Parameters::IndirMax,
        15UZ
    };
    constexpr MagicParameters same{MagicParameters::Parameters::IndirMax, 15UZ};
    constexpr MagicParameters indir_16{
        MagicParameters::Parameters::IndirMax,
        16UZ
    };
    constexpr MagicParameters later{
        MagicParameters::Parameters::NameMax,
        999UZ
    };
    constexpr MagicParameters empty_snapshot{};

    static_assert(indir_15 == same);
    static_assert(indir_15 != indir_16);
    static_assert(indir_15 != empty_snapshot);
    static_assert((indir_15 <=> indir_16) == std::strong_ordering::less);
    static_assert((indir_16 <=> indir_15) == std::strong_ordering::greater);
    static_assert((indir_15 <=> same) == std::strong_ordering::equal);
    static_assert((indir_15 <=> later) == std::strong_ordering::greater);
    static_assert((empty_snapshot <=> indir_15) == std::strong_ordering::less);
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
    EXPECT_TRUE(default_parameters.ToString().Empty());
    EXPECT_TRUE(default_parameters
                    .ToString(MagicParameters::StringFormat::Names, ": ", ",")
                    .Empty());
    const MagicParameters empty_parameters{
        MagicParameters::ParameterValueMapT{}
    };
    EXPECT_TRUE(empty_parameters.ToString().Empty());
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
    );
}

TEST_F(MagicParametersConvertTest, invalid_ordinal_snapshot_is_empty)
{
    constexpr std::size_t                 INVALID_ORDINAL{99};
    constexpr MagicParameters::Parameters INVALID_PARAMETER{
        static_cast<MagicParameters::Parameters>(INVALID_ORDINAL)
    };
    const MagicParameters single{INVALID_PARAMETER, 1UZ};
    EXPECT_TRUE(single.ToString().Empty());

    const MagicParameters::ParameterValueMapT map{
        {INVALID_PARAMETER, 1UZ}
    };
    const MagicParameters from_map{map};
    EXPECT_TRUE(
        from_map.ToString(MagicParameters::StringFormat::Names).Empty()
    );
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

TEST_F(MagicParametersConvertTest, equality_operator_compares_snapshots)
{
    const MagicParameters default_parameters;
    EXPECT_EQ(default_parameters, MagicParameters{});
    EXPECT_EQ(
        default_parameters,
        MagicParameters{MagicParameters::ParameterValueMapT{}}
    );

    const MagicParameters single{
        MagicParameters::Parameters::RegexMax,
        8'192UZ
    };
    const MagicParameters from_map{MagicParameters::ParameterValueMapT{
        {MagicParameters::Parameters::RegexMax, 8'192UZ}
    }};
    EXPECT_EQ(single, from_map);

    const MagicParameters other_value{
        MagicParameters::Parameters::RegexMax,
        4'096UZ
    };
    EXPECT_NE(single, other_value);

    const MagicParameters superset{
        MagicParameters::ParameterValueMapT{
                                            {MagicParameters::Parameters::RegexMax, 8'192UZ},
                                            {MagicParameters::Parameters::BytesMax, 7UZ}
        }
    };
    EXPECT_NE(single, superset);
    EXPECT_NE(superset, single);

    constexpr MagicParameters::Parameters INVALID_PARAMETER{
        static_cast<MagicParameters::Parameters>(99)
    };
    const MagicParameters invalid_snapshot{INVALID_PARAMETER, 1UZ};
    EXPECT_EQ(invalid_snapshot, default_parameters);
}

TEST_F(MagicParametersConvertTest, spaceship_orders_snapshots_entrywise)
{
    const MagicParameters empty_parameters{};
    const MagicParameters indir_15{MagicParameters::Parameters::IndirMax, 15UZ};
    const MagicParameters indir_16{MagicParameters::Parameters::IndirMax, 16UZ};
    const MagicParameters fuller{
        MagicParameters::ParameterValueMapT{
                                            {MagicParameters::Parameters::IndirMax, 15UZ},
                                            {MagicParameters::Parameters::NameMax, 30UZ}
        }
    };

    EXPECT_LT(empty_parameters, indir_15);
    EXPECT_LT(indir_15, indir_16);
    EXPECT_LT(indir_15, fuller);
    EXPECT_GE(indir_15, indir_15);
    EXPECT_LE(indir_15, indir_15);
    EXPECT_GT(fuller, empty_parameters);

    EXPECT_EQ(indir_15 <=> indir_16, std::strong_ordering::less);
    EXPECT_EQ(indir_16 <=> indir_15, std::strong_ordering::greater);
    const MagicParameters indir_15_from_map{MagicParameters::ParameterValueMapT{
        {MagicParameters::Parameters::IndirMax, 15UZ}
    }};
    EXPECT_EQ(indir_15 <=> indir_15_from_map, std::strong_ordering::equal);

    const MagicParameters early_big{
        MagicParameters::ParameterValueMapT{
                                            {MagicParameters::Parameters::IndirMax, 999UZ},
                                            {MagicParameters::Parameters::MagWarnMax, 0UZ}
        }
    };
    const MagicParameters late_big{
        MagicParameters::ParameterValueMapT{
                                            {MagicParameters::Parameters::IndirMax, 1UZ},
                                            {MagicParameters::Parameters::MagWarnMax, 999UZ}
        }
    };
    EXPECT_GT(early_big, late_big);
}

TEST_F(MagicParametersConvertTest, spaceship_provides_strict_weak_ordering)
{
    std::vector<MagicParameters> snapshots{
        MagicParameters{},
        MagicParameters{MagicParameters::Parameters::IndirMax, 15UZ},
        MagicParameters{MagicParameters::Parameters::IndirMax, 16UZ},
        MagicParameters{MagicParameters::Parameters::MagWarnMax, 64UZ},
        MagicParameters{FullValueMap()},
        MagicParameters{FullValueMap()},
    };

    const std::set<MagicParameters> ordered_set{
        snapshots.begin(),
        snapshots.end()
    };
    EXPECT_EQ(5UZ, ordered_set.size());

    std::ranges::sort(snapshots);
    EXPECT_TRUE(std::ranges::is_sorted(snapshots));
    snapshots.erase(std::ranges::unique(snapshots).begin(), snapshots.end());
    EXPECT_EQ(5UZ, snapshots.size());

    const std::vector<MagicParameters> from_set{
        ordered_set.begin(),
        ordered_set.end()
    };
    EXPECT_EQ(from_set, snapshots);
}
