/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <random>

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_parameters_test, closed_magic_set_parameter_get_parameter)
{
    magic m;
    EXPECT_THROW(m.set_parameter(magic::parameters::bytes_max, 1), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameter(magic::parameters::bytes_max), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameters(), magic_is_closed);
}

TEST(magic_parameters_test, opened_magic_set_parameter_get_parameter)
{
    magic m;
    m.open(magic::Flag::Mime);
    std::mt19937 eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{0, 100};
    magic::parameter_value_map_t expected_parameters{
        {magic::parameters::indir_max, dist(eng)},
        {magic::parameters::name_max, dist(eng)},
        {magic::parameters::elf_phnum_max, dist(eng)},
        {magic::parameters::elf_shnum_max, dist(eng)},
        {magic::parameters::elf_notes_max, dist(eng)},
        {magic::parameters::regex_max, dist(eng)},
        {magic::parameters::bytes_max, dist(eng)},
        {magic::parameters::encoding_max, dist(eng)},
        {magic::parameters::elf_shsize_max, dist(eng)}
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
        std::make_pair(magic::parameters::indir_max, "indir_max"),
        std::make_pair(magic::parameters::name_max, "name_max"),
        std::make_pair(magic::parameters::elf_phnum_max, "elf_phnum_max"),
        std::make_pair(magic::parameters::elf_shnum_max, "elf_shnum_max"),
        std::make_pair(magic::parameters::elf_notes_max, "elf_notes_max"),
        std::make_pair(magic::parameters::regex_max, "regex_max"),
        std::make_pair(magic::parameters::bytes_max, "bytes_max"),
        std::make_pair(magic::parameters::encoding_max, "encoding_max"),
        std::make_pair(magic::parameters::elf_shsize_max, "elf_shsize_max")
    };
    for (const auto& parameter_pair : magic_parameters_with_names){
        EXPECT_EQ(to_string(parameter_pair.first), parameter_pair.second);
    }
}

TEST(magic_parameters_test, parameters_to_string_conversion)
{
    EXPECT_EQ(to_string(magic::parameter_value_map_t{
        {magic::parameters::indir_max, 1},
        {magic::parameters::name_max, 2},
        {magic::parameters::elf_phnum_max, 3},
        {magic::parameters::elf_shnum_max, 4},
        {magic::parameters::elf_notes_max, 5},
        {magic::parameters::regex_max, 6},
        {magic::parameters::bytes_max, 7},
        {magic::parameters::encoding_max, 8},
        {magic::parameters::elf_shsize_max, 9}
    }),
        "indir_max: 1, "
        "name_max: 2, "
        "elf_phnum_max: 3, "
        "elf_shnum_max: 4, "
        "elf_notes_max: 5, "
        "regex_max: 6, "
        "bytes_max: 7, "
        "encoding_max: 8, "
        "elf_shsize_max: 9"
    );
}
