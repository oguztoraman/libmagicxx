/* SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <random>

#include "magic.hpp"

using namespace recognition;

TEST(magic_parameters_test, closed_magic_set_parameter_get_parameter)
{
    magic m;
    EXPECT_THROW(
        m.set_parameter(magic::parameters::bytes_max, 1),
        magic_is_closed
    );
    EXPECT_THROW(
        [[maybe_unused]] auto _ = m.get_parameter(magic::parameters::bytes_max),
        magic_is_closed
    );
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_parameters(), magic_is_closed);
}

TEST(magic_parameters_test, opened_magic_set_parameter_get_parameter)
{
    magic m;
    m.open(magic::flags::mime);
    std::mt19937                               eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{0, 100};
    using enum magic::parameters;
    magic::parameter_value_map_t expected_parameters{
        {indir_max,      dist(eng)},
        {name_max,       dist(eng)},
        {elf_phnum_max,  dist(eng)},
        {elf_shnum_max,  dist(eng)},
        {elf_notes_max,  dist(eng)},
        {regex_max,      dist(eng)},
        {bytes_max,      dist(eng)},
        {encoding_max,   dist(eng)},
        {elf_shsize_max, dist(eng)}
    };
    for (const auto& [parameter, value] : expected_parameters) {
        m.set_parameter(parameter, value);
        EXPECT_EQ(m.get_parameter(parameter), value);
    }
    EXPECT_EQ(expected_parameters, m.get_parameters());
    m.open(magic::flags::mime);
    m.set_parameters(expected_parameters);
    EXPECT_EQ(expected_parameters, m.get_parameters());
}

TEST(magic_parameters_test, parameter_to_string_conversion)
{
    using enum magic::parameters;
    std::vector magic_parameters_with_names{
        std::make_pair(indir_max, "indir_max"),
        std::make_pair(name_max, "name_max"),
        std::make_pair(elf_phnum_max, "elf_phnum_max"),
        std::make_pair(elf_shnum_max, "elf_shnum_max"),
        std::make_pair(elf_notes_max, "elf_notes_max"),
        std::make_pair(regex_max, "regex_max"),
        std::make_pair(bytes_max, "bytes_max"),
        std::make_pair(encoding_max, "encoding_max"),
        std::make_pair(elf_shsize_max, "elf_shsize_max")
    };
    for (const auto& [parameter, parameter_str] : magic_parameters_with_names) {
        EXPECT_EQ(to_string(parameter), parameter_str);
    }
}

TEST(magic_parameters_test, parameters_to_string_conversion)
{
    using enum magic::parameters;
    EXPECT_EQ(
        to_string(magic::parameter_value_map_t{
            {indir_max,      1},
            {name_max,       2},
            {elf_phnum_max,  3},
            {elf_shnum_max,  4},
            {elf_notes_max,  5},
            {regex_max,      6},
            {bytes_max,      7},
            {encoding_max,   8},
            {elf_shsize_max, 9}
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
