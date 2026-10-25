/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters_test.cpp
 * @brief Unit tests for MagicParameters and Magic parameter operations.
 *
 * Tests the MagicParameters value class and the parameter getting and
 * setting functionality of Magic, including:
 * - Every MagicParameters constructor (default, parameter/value,
 *   initializer list) and the trivially noexcept special members
 * - The allocation-free transport interface: range iteration in ordinal
 *   order (forward iterators, including postfix increment and the
 *   default-constructed end position) and the read-only map-like lookup
 *   API (operator[], Contains()), with Count()/Empty() size queries
 * - ToString() name lookup, formats, ordering, separators, capacity
 *   truncation, and the empty snapshot
 * - Equality comparison via operator==
 * - Total ordering via operator<=> (entry-wise ordinal order, ordered
 *   containers)
 * - std::format rendering of snapshots
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
#include <format>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <type_traits>
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
    MagicParameters                            m_test_parameters{};
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
    const auto snapshot{
        m_opened_magic_without_database.GetParameters(std::nothrow)
    };
    ASSERT_TRUE(snapshot.has_value());
    EXPECT_EQ(m_test_parameters, *snapshot);
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
    const auto snapshot{m_valid_magic.GetParameters(std::nothrow)};
    ASSERT_TRUE(snapshot.has_value());
    EXPECT_EQ(m_test_parameters, *snapshot);
}

TEST_F(MagicParametersTest, get_parameters_render_through_magic_parameters)
{
    m_valid_magic.SetParameters(m_test_parameters);
    const auto rendered = m_valid_magic.GetParameters();
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
    const auto parameters = m_valid_magic.GetParameters();
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

TEST_F(
    MagicParametersTest,
    set_parameters_discards_invalid_key_and_applies_valid_entries
)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    const MagicParameters parameters{
        {Magic::ParametersT::RegexMax, SENTINEL_STEP },
        {INVALID_PARAMETER,            SENTINEL_VALUE}
    };
    EXPECT_FALSE(parameters.Contains(INVALID_PARAMETER));
    EXPECT_NO_THROW(m_valid_magic.SetParameters(parameters));
    EXPECT_EQ(
        SENTINEL_STEP,
        m_valid_magic.GetParameter(Magic::ParametersT::RegexMax)
    );
    EXPECT_EQ(
        SENTINEL_VALUE,
        m_valid_magic.GetParameter(Magic::ParametersT::IndirMax)
    );
}

TEST_F(MagicParametersTest, set_parameters_with_invalid_key_noexcept)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    const MagicParameters parameters{
        {Magic::ParametersT::RegexMax, SENTINEL_STEP },
        {INVALID_PARAMETER,            SENTINEL_VALUE}
    };
    EXPECT_TRUE(m_valid_magic.SetParameters(parameters, std::nothrow));
    EXPECT_EQ(
        SENTINEL_STEP,
        m_valid_magic.GetParameter(Magic::ParametersT::RegexMax)
    );
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
    set_parameters_discards_invalid_key_after_valid_entries
)
{
    m_valid_magic.SetParameter(Magic::ParametersT::IndirMax, SENTINEL_VALUE);
    const MagicParameters parameters{
        {INVALID_PARAMETER,              SENTINEL_VALUE},
        {Magic::ParametersT::MagWarnMax, SENTINEL_STEP },
        {Magic::ParametersT::RegexMax,   SENTINEL_STEP }
    };
    EXPECT_TRUE(parameters.Contains(Magic::ParametersT::RegexMax));
    EXPECT_FALSE(parameters.Contains(INVALID_PARAMETER));
    m_valid_magic.SetParameters(parameters);
    EXPECT_EQ(
        SENTINEL_STEP,
        m_valid_magic.GetParameter(Magic::ParametersT::RegexMax)
    );
    EXPECT_EQ(
        SENTINEL_STEP,
        m_valid_magic.GetParameter(Magic::ParametersT::MagWarnMax)
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

    static MagicParameters FullValueSnapshot()
    {
        return MagicParameters{
            {MagicParameters::Parameters::IndirMax,     1UZ },
            {MagicParameters::Parameters::NameMax,      2UZ },
            {MagicParameters::Parameters::ElfPhnumMax,  3UZ },
            {MagicParameters::Parameters::ElfShnumMax,  4UZ },
            {MagicParameters::Parameters::ElfNotesMax,  5UZ },
            {MagicParameters::Parameters::RegexMax,     6UZ },
            {MagicParameters::Parameters::BytesMax,     7UZ },
            {MagicParameters::Parameters::EncodingMax,  8UZ },
            {MagicParameters::Parameters::ElfShsizeMax, 9UZ },
            {MagicParameters::Parameters::MagWarnMax,   10UZ}
        };
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
        const MagicParameters snapshot{
            {MagicParameters::Parameters::BytesMax, 7UZ}
        };
        static_assert(
            std::bool_constant<noexcept(MagicParameters{
                MagicParameters::Parameters::BytesMax,
                7UZ
        })>::value
            && std::bool_constant<noexcept(
                MagicParameters{{MagicParameters::Parameters::BytesMax, 7UZ}}
            )>::value
            && std::bool_constant<noexcept(MagicParameters{snapshot})>::value
        );
        static_assert(std::is_nothrow_constructible_v<
                      MagicParameters,
                      std::initializer_list<MagicParameters::ParameterValueT>
        >);
        (void)snapshot;
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

TEST_F(MagicParametersConvertTest, default_and_empty_snapshot_are_empty)
{
    const MagicParameters default_parameters;
    EXPECT_TRUE(default_parameters.ToString().Empty());
    EXPECT_TRUE(default_parameters
                    .ToString(MagicParameters::StringFormat::Names, ": ", ",")
                    .Empty());
    const MagicParameters empty_parameters{
        std::initializer_list<MagicParameters::ParameterValueT>{}
    };
    EXPECT_TRUE(empty_parameters.ToString().Empty());
}

TEST_F(MagicParametersConvertTest, two_entry_snapshot_orders_by_ordinal)
{
    const MagicParameters parameters{
        {MagicParameters::Parameters::BytesMax, 1'048'576UZ},
        {MagicParameters::Parameters::RegexMax, 8'192UZ    }
    };
    EXPECT_EQ("RegexMax: 8192, BytesMax: 1048576", parameters.ToString());
}

TEST_F(MagicParametersConvertTest, to_string_renders_every_parameter)
{
    const MagicParameters parameters{FullValueSnapshot()};
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
    const MagicParameters parameters{FullValueSnapshot()};
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

    const MagicParameters from_list{
        {INVALID_PARAMETER, 1UZ}
    };
    EXPECT_TRUE(
        from_list.ToString(MagicParameters::StringFormat::Names).Empty()
    );
}

TEST_F(MagicParametersConvertTest, copy_and_move_preserve_snapshot)
{
    const MagicParameters snapshot{
        {MagicParameters::Parameters::BytesMax, 7UZ    },
        {MagicParameters::Parameters::RegexMax, 8'192UZ}
    };
    MagicParameters       original{snapshot};
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
        MagicParameters{
            std::initializer_list<MagicParameters::ParameterValueT>{}
        }
    );

    const MagicParameters single{
        MagicParameters::Parameters::RegexMax,
        8'192UZ
    };
    const MagicParameters from_list{
        {MagicParameters::Parameters::RegexMax, 8'192UZ}
    };
    EXPECT_EQ(single, from_list);

    const MagicParameters other_value{
        MagicParameters::Parameters::RegexMax,
        4'096UZ
    };
    EXPECT_NE(single, other_value);

    const MagicParameters superset{
        {MagicParameters::Parameters::RegexMax, 8'192UZ},
        {MagicParameters::Parameters::BytesMax, 7UZ    }
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
        {MagicParameters::Parameters::IndirMax, 15UZ},
        {MagicParameters::Parameters::NameMax,  30UZ}
    };

    EXPECT_LT(empty_parameters, indir_15);
    EXPECT_LT(indir_15, indir_16);
    EXPECT_LT(indir_15, fuller);
    EXPECT_GE(indir_15, indir_15);
    EXPECT_LE(indir_15, indir_15);
    EXPECT_GT(fuller, empty_parameters);

    EXPECT_EQ(indir_15 <=> indir_16, std::strong_ordering::less);
    EXPECT_EQ(indir_16 <=> indir_15, std::strong_ordering::greater);
    const MagicParameters indir_15_from_list{
        {MagicParameters::Parameters::IndirMax, 15UZ}
    };
    EXPECT_EQ(indir_15 <=> indir_15_from_list, std::strong_ordering::equal);

    const MagicParameters early_big{
        {MagicParameters::Parameters::IndirMax,   999UZ},
        {MagicParameters::Parameters::MagWarnMax, 0UZ  }
    };
    const MagicParameters late_big{
        {MagicParameters::Parameters::IndirMax,   1UZ  },
        {MagicParameters::Parameters::MagWarnMax, 999UZ}
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
        MagicParameters{FullValueSnapshot()},
        MagicParameters{FullValueSnapshot()},
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

TEST(MagicParametersTransportTest, satisfies_transport_properties)
{
    static_assert(std::ranges::input_range<MagicParameters>);
    static_assert(std::input_iterator<MagicParameters::Iterator>);
    static_assert(
        std::sentinel_for<MagicParameters::Iterator, MagicParameters::Iterator>
    );
    static_assert(std::is_nothrow_constructible_v<
                  MagicParameters,
                  std::initializer_list<MagicParameters::ParameterValueT>
    >);
    static_assert(std::is_nothrow_invocable_v<
                  decltype(&MagicParameters::operator[]),
                  const MagicParameters&,
                  MagicParameters::Parameters
    >);
    static_assert(std::is_nothrow_invocable_v<
                  decltype(&MagicParameters::Contains),
                  const MagicParameters&,
                  MagicParameters::Parameters
    >);
}

TEST(
    MagicParametersTransportTest,
    construct_from_initializer_list_stores_valid_and_discards_invalid_entries
)
{
    constexpr MagicParameters::Parameters INVALID_PARAMETER{
        static_cast<MagicParameters::Parameters>(99)
    };
    const MagicParameters parameters{
        {MagicParameters::Parameters::BytesMax, 1'048'576UZ},
        {INVALID_PARAMETER,                     42UZ       }
    };
    EXPECT_EQ(
        (std::optional<std::size_t>{1'048'576UZ}),
        parameters[MagicParameters::Parameters::BytesMax]
    );
    EXPECT_FALSE(parameters.Contains(INVALID_PARAMETER));
    EXPECT_TRUE((MagicParameters{
                     {INVALID_PARAMETER, 1UZ}
    })
                    .ToString()
                    .Empty());

    const MagicParameters repeated{
        {MagicParameters::Parameters::NameMax, 1UZ},
        {MagicParameters::Parameters::NameMax, 2UZ}
    };
    EXPECT_EQ(2UZ, repeated[MagicParameters::Parameters::NameMax].value());
}

TEST(
    MagicParametersTransportTest,
    iteration_yields_entries_in_ordinal_order_skipping_unset
)
{
    const MagicParameters parameters{
        {MagicParameters::Parameters::MagWarnMax, 64UZ   },
        {MagicParameters::Parameters::RegexMax,   8'192UZ},
        {MagicParameters::Parameters::IndirMax,   15UZ   }
    };
    const std::vector<MagicParameters::ParameterValueT> entries{
        parameters.begin(),
        parameters.end()
    };
    const std::vector<MagicParameters::ParameterValueT> expected{
        {MagicParameters::Parameters::IndirMax,   15UZ   },
        {MagicParameters::Parameters::RegexMax,   8'192UZ},
        {MagicParameters::Parameters::MagWarnMax, 64UZ   }
    };
    EXPECT_EQ(expected, entries);
    EXPECT_EQ(3, std::ranges::distance(parameters));

    std::size_t iterated{};
    for (const auto [parameter, value] : parameters) {
        EXPECT_TRUE(parameters.Contains(parameter));
        EXPECT_EQ(value, parameters[parameter].value());
        EXPECT_EQ(
            std::format("{}: {}", parameter, value),
            std::format(
                "{}: {}",
                MagicParameters{parameter, 0UZ}.ToString(
                    MagicParameters::StringFormat::Names
                ),
                value
            )
        );
        ++iterated;
    }
    EXPECT_EQ(3UZ, iterated);
}

TEST(MagicParametersTransportTest, lookup_api_matches_map_semantics)
{
    constexpr MagicParameters::Parameters INVALID_PARAMETER{
        static_cast<MagicParameters::Parameters>(99)
    };
    const MagicParameters parameters{
        {MagicParameters::Parameters::BytesMax, 1'048'576UZ},
        {MagicParameters::Parameters::RegexMax, 8'192UZ    }
    };

    EXPECT_EQ(
        (std::optional<std::size_t>{1'048'576UZ}),
        parameters[MagicParameters::Parameters::BytesMax]
    );
    EXPECT_EQ(
        (std::optional<std::size_t>{8'192UZ}),
        parameters[MagicParameters::Parameters::RegexMax]
    );
    EXPECT_FALSE(parameters[MagicParameters::Parameters::IndirMax].has_value());
    EXPECT_FALSE(parameters[INVALID_PARAMETER].has_value());

    EXPECT_TRUE(parameters.Contains(MagicParameters::Parameters::RegexMax));
    EXPECT_FALSE(parameters.Contains(MagicParameters::Parameters::IndirMax));
    EXPECT_FALSE(parameters.Contains(INVALID_PARAMETER));
}

TEST_F(
    MagicParametersTest,
    get_parameters_returns_full_snapshot_equal_to_stored_values
)
{
    m_valid_magic.SetParameters(m_test_parameters);
    const auto snapshot = m_valid_magic.GetParameters();
    EXPECT_EQ(m_test_parameters, snapshot);
    EXPECT_EQ(
        static_cast<std::ptrdiff_t>(ORDINAL_COUNT),
        std::ranges::distance(snapshot)
    );
}

TEST_F(
    MagicParametersTest,
    get_parameters_nothrow_round_trip_in_noexcept_context
)
{
    m_valid_magic.SetParameters(m_test_parameters);
    const auto result{[this]() noexcept -> std::size_t {
        const auto snapshot{m_valid_magic.GetParameters(std::nothrow)};
        if (!snapshot.has_value()) {
            return 0UZ;
        }
        std::size_t matches{};
        for (const auto [parameter, value] : *snapshot) {
            matches += (*snapshot)[parameter] == value ? 1UZ : 0UZ;
        }
        return matches;
    }()};
    EXPECT_EQ(ORDINAL_COUNT, result);
}

TEST_F(MagicParametersConvertTest, count_and_empty_report_size)
{
    EXPECT_EQ(0UZ, MagicParameters{}.Count());
    EXPECT_TRUE(MagicParameters{}.Empty());

    const MagicParameters single{MagicParameters::Parameters::BytesMax, 7UZ};
    EXPECT_EQ(1UZ, single.Count());
    EXPECT_FALSE(single.Empty());

    constexpr MagicParameters::Parameters invalid_parameter{
        static_cast<MagicParameters::Parameters>(99)
    };
    const MagicParameters with_invalid{
        {MagicParameters::Parameters::BytesMax, 7UZ},
        {invalid_parameter,                     8UZ}
    };
    EXPECT_EQ(1UZ, with_invalid.Count());
    EXPECT_FALSE(with_invalid.Empty());

    EXPECT_EQ(ALL_PARAMETERS.size(), FullValueSnapshot().Count());

    static_assert(MagicParameters{}.Empty());
    static_assert(
        !MagicParameters{MagicParameters::Parameters::NameMax, 0UZ}.Empty()
    );
}

TEST_F(MagicParametersConvertTest, entries_model_forward_iterator)
{
    static_assert(std::forward_iterator<MagicParameters::Iterator>);

    const MagicParameters snapshot{
        {MagicParameters::Parameters::RegexMax, 6UZ},
        {MagicParameters::Parameters::IndirMax, 1UZ}
    };

    /* Multi-pass: two concurrent iterators over the same snapshot advance
     * independently and must stay in lock-step, as forward iterators
     * guarantee. */
    auto        first{snapshot.begin()};
    auto        second{snapshot.begin()};
    std::size_t passes{};
    for (int pass{}; pass < 2; ++pass) {
        while (first != snapshot.end()) {
            ASSERT_TRUE(second != snapshot.end());
            EXPECT_EQ(*first, *second);
            ++first;
            ++second;
            ++passes;
        }
        first  = snapshot.begin();
        second = snapshot.begin();
    }
    EXPECT_EQ(2UZ * snapshot.Count(), passes);

    EXPECT_EQ(static_cast<std::ptrdiff_t>(2), std::ranges::distance(snapshot));
    const auto found{std::ranges::find_if(
        snapshot,
        [](const MagicParameters::ParameterValueT& entry) {
            return entry.first == MagicParameters::Parameters::RegexMax;
        }
    )};
    ASSERT_TRUE(found != snapshot.end());
    EXPECT_EQ(6UZ, (*found).second);
}

TEST_F(
    MagicParametersConvertTest,
    postfix_increment_returns_the_previous_position
)
{
    const MagicParameters snapshot{
        {MagicParameters::Parameters::RegexMax, 6UZ},
        {MagicParameters::Parameters::IndirMax, 1UZ}
    };

    auto       iterator{snapshot.begin()};
    const auto previous{iterator++};
    EXPECT_EQ(
        (MagicParameters::ParameterValueT{
            MagicParameters::Parameters::IndirMax,
            1UZ
        }),
        *previous
    );
    EXPECT_EQ(
        (MagicParameters::ParameterValueT{
            MagicParameters::Parameters::RegexMax,
            6UZ
        }),
        *iterator
    );
    EXPECT_TRUE(previous != iterator);

    const auto at_end{iterator++};
    EXPECT_EQ(
        (MagicParameters::ParameterValueT{
            MagicParameters::Parameters::RegexMax,
            6UZ
        }),
        *at_end
    );
    EXPECT_TRUE(iterator == snapshot.end());
}

TEST_F(MagicParametersConvertTest, default_iterator_equals_any_end)
{
    static_assert(
        std::is_nothrow_default_constructible_v<MagicParameters::Iterator>
    );

    const MagicParameters empty_snapshot;
    const MagicParameters snapshot{MagicParameters::Parameters::RegexMax, 6UZ};

    EXPECT_TRUE(MagicParameters::Iterator{} == empty_snapshot.end());
    EXPECT_TRUE(MagicParameters::Iterator{} == snapshot.end());
    EXPECT_TRUE(MagicParameters::Iterator{} != snapshot.begin());
}

TEST_F(MagicParametersConvertTest, to_string_truncates_to_smaller_capacity)
{
    const MagicParameters snapshot{
        {MagicParameters::Parameters::RegexMax, 6UZ},
        {MagicParameters::Parameters::BytesMax, 7UZ}
    };

    const auto truncated{snapshot.ToString<10UZ>()};
    EXPECT_TRUE(truncated.IsTruncated());
    EXPECT_EQ("RegexMax: ", truncated);

    const auto full{FullValueSnapshot().ToString()};
    EXPECT_FALSE(full.IsTruncated());

    const auto tiny{
        MagicParameters{MagicParameters::Parameters::RegexMax, 6UZ}
            .ToString<3UZ>()
    };
    EXPECT_TRUE(tiny.IsTruncated());
    EXPECT_EQ("Reg", tiny);
}

TEST_F(MagicParametersConvertTest, snapshot_formatter_prints_entries)
{
    const MagicParameters snapshot{
        {MagicParameters::Parameters::RegexMax, 6UZ},
        {MagicParameters::Parameters::BytesMax, 7UZ}
    };
    EXPECT_EQ(std::format("{}", snapshot), "RegexMax: 6, BytesMax: 7");
    EXPECT_EQ(std::format("{}", MagicParameters{}), "");
    EXPECT_EQ(std::format("{:*<25}", snapshot), "RegexMax: 6, BytesMax: 7*");

    EXPECT_NO_THROW(static_cast<void>(std::format("{}", snapshot)));
}
