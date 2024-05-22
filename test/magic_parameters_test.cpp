/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <random>

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_parameters_test, closed_magic_set_parameter_get_parameter)
{
    magic m;
    EXPECT_THROW(m.set_parameter(magic::parameters::BytesMax, 1), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameter(magic::parameters::BytesMax), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameters(), magic_is_closed);
}

TEST(magic_parameters_test, opened_magic_set_parameter_get_parameter)
{
    magic m;
    m.open(magic::Flag::Mime);
    std::mt19937 eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{0, 100};
    magic::parameter_value_map_t expected_parameters{
        {magic::parameters::IndirMax, dist(eng)},
        {magic::parameters::NameMax, dist(eng)},
        {magic::parameters::ElfPhnumMax, dist(eng)},
        {magic::parameters::ElfShnumMax, dist(eng)},
        {magic::parameters::ElfNotesMax, dist(eng)},
        {magic::parameters::RegexMax, dist(eng)},
        {magic::parameters::BytesMax, dist(eng)},
        {magic::parameters::EncodingMax, dist(eng)},
        {magic::parameters::ElfShsizeMax, dist(eng)}
    };
    for (const auto& parameter_pair : expected_parameters){
        m.set_parameter(parameter_pair.first, parameter_pair.second);
        EXPECT_EQ(m.get_parameter(parameter_pair.first), parameter_pair.second);
    }
    EXPECT_EQ(expected_parameters, m.get_parameters());
}

TEST(magic_parameters_test, parameter_to_string_conversion)
{
    std::vector magic_parameters_with_names{
        std::make_pair(magic::parameters::IndirMax, "IndirMax"),
        std::make_pair(magic::parameters::NameMax, "NameMax"),
        std::make_pair(magic::parameters::ElfPhnumMax, "ElfPhnumMax"),
        std::make_pair(magic::parameters::ElfShnumMax, "ElfShnumMax"),
        std::make_pair(magic::parameters::ElfNotesMax, "ElfNotesMax"),
        std::make_pair(magic::parameters::RegexMax, "RegexMax"),
        std::make_pair(magic::parameters::BytesMax, "BytesMax"),
        std::make_pair(magic::parameters::EncodingMax, "EncodingMax"),
        std::make_pair(magic::parameters::ElfShsizeMax, "ElfShsizeMax")
    };
    for (const auto& parameter_pair : magic_parameters_with_names){
        EXPECT_EQ(to_string(parameter_pair.first), parameter_pair.second);
    }
}

TEST(magic_parameters_test, parameters_to_string_conversion)
{
    EXPECT_EQ(to_string(magic::parameter_value_map_t{
        {magic::parameters::IndirMax, 1},
        {magic::parameters::NameMax, 2},
        {magic::parameters::ElfPhnumMax, 3},
        {magic::parameters::ElfShnumMax, 4},
        {magic::parameters::ElfNotesMax, 5},
        {magic::parameters::RegexMax, 6},
        {magic::parameters::BytesMax, 7},
        {magic::parameters::EncodingMax, 8},
        {magic::parameters::ElfShsizeMax, 9}
    }),
        "IndirMax: 1, "
        "NameMax: 2, "
        "ElfPhnumMax: 3, "
        "ElfShnumMax: 4, "
        "ElfNotesMax: 5, "
        "RegexMax: 6, "
        "BytesMax: 7, "
        "EncodingMax: 8, "
        "ElfShsizeMax: 9"
    );
}
