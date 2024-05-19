/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <random>
#include <sstream>

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_parameters_test, closed_magic_set_parameter_get_parameter)
{
    magic m;
    EXPECT_THROW(m.set_parameter(magic::Parameter::BytesMax, 1), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameter(magic::Parameter::BytesMax), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameters(), magic_is_closed);
}

TEST(magic_parameters_test, opened_magic_set_parameter_get_parameter)
{
    magic m;
    m.open(magic::Flag::Mime);
    std::mt19937 eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{0, 100};
    magic::Parameters expected_parameters{
        {magic::Parameter::IndirMax, dist(eng)},
        {magic::Parameter::NameMax, dist(eng)},
        {magic::Parameter::ElfPhnumMax, dist(eng)},
        {magic::Parameter::ElfShnumMax, dist(eng)},
        {magic::Parameter::ElfNotesMax, dist(eng)},
        {magic::Parameter::RegexMax, dist(eng)},
        {magic::Parameter::BytesMax, dist(eng)},
        {magic::Parameter::EncodingMax, dist(eng)},
        {magic::Parameter::ElfShsizeMax, dist(eng)}
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
        std::make_pair(magic::Parameter::IndirMax, "IndirMax"),
        std::make_pair(magic::Parameter::NameMax, "NameMax"),
        std::make_pair(magic::Parameter::ElfPhnumMax, "ElfPhnumMax"),
        std::make_pair(magic::Parameter::ElfShnumMax, "ElfShnumMax"),
        std::make_pair(magic::Parameter::ElfNotesMax, "ElfNotesMax"),
        std::make_pair(magic::Parameter::RegexMax, "RegexMax"),
        std::make_pair(magic::Parameter::BytesMax, "BytesMax"),
        std::make_pair(magic::Parameter::EncodingMax, "EncodingMax"),
        std::make_pair(magic::Parameter::ElfShsizeMax, "ElfShsizeMax")
    };
    for (const auto& parameter_pair : magic_parameters_with_names){
        EXPECT_EQ(to_string(parameter_pair.first), parameter_pair.second);
    }
}

TEST(magic_parameters_test, parameters_to_string_conversion)
{
    std::ostringstream oss;
    oss << magic::Parameters{
        {magic::Parameter::IndirMax, 1},
        {magic::Parameter::NameMax, 2},
        {magic::Parameter::ElfPhnumMax, 3},
        {magic::Parameter::ElfShnumMax, 4},
        {magic::Parameter::ElfNotesMax, 5},
        {magic::Parameter::RegexMax, 6},
        {magic::Parameter::BytesMax, 7},
        {magic::Parameter::EncodingMax, 8},
        {magic::Parameter::ElfShsizeMax, 9}
    };
    EXPECT_EQ(oss.str(),
        "IndirMax: 1\n"
        "NameMax: 2\n"
        "ElfPhnumMax: 3\n"
        "ElfShnumMax: 4\n"
        "ElfNotesMax: 5\n"
        "RegexMax: 6\n"
        "BytesMax: 7\n"
        "EncodingMax: 8\n"
        "ElfShsizeMax: 9\n"
    );
}
