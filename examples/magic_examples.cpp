/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <iostream>
#include <magic.hpp>
#include <print>

using namespace recognition;

void example_open_close()
{
    magic m;
    m.open(magic::flags::mime);
    if (m.is_open()) {
        std::println(std::cout, "Magic is open.");
    } else {
        std::println(std::cerr, "Failed to open magic.");
    }
    m.close();
    if (!m.is_open()) {
        std::println(std::cout, "Magic is closed.");
    } else {
        std::println(std::cerr, "Failed to close magic.");
    }
}

void example_load_database()
{
    magic m;
    m.open(magic::flags::mime);
    m.load_database_file();
    std::println(std::cout, "Loaded default database.");
}

void example_identify_file()
{
    magic m{magic::flags::mime, magic::default_database_file};
    if (!m.is_valid()) {
        std::println(std::cerr, "Magic is not valid.");
        return;
    }
    auto result = m.identify_file(magic::default_database_file);
    std::println(std::cout, "File type: {}", result);
}

void example_set_get_parameters()
{
    magic m;
    m.open(magic::flags::mime);
    m.set_parameter(magic::parameters::bytes_max, 1'024);
    auto value = m.get_parameter(magic::parameters::bytes_max);
    std::println(std::cout, "Bytes_max: {}", value);
}

void example_set_get_flags()
{
    magic m;
    m.open(magic::flags::none);
    m.set_flags(magic::flags::mime | magic::flags::compress);
    auto flags = m.get_flags();
    std::println(std::cout, "Flags: {}", to_string(flags));
}

void example_compile_check()
{
    magic m;
    m.open(magic::flags::mime);
    if (m.compile()) {
        std::println(std::cout, "Compiled successfully.");
    } else {
        std::println(std::cerr, "Failed to compile.");
    }
    if (magic::check()) {
        std::println(std::cout, "Check passed.");
    } else {
        std::println(std::cerr, "Check failed.");
    }
}

int main()
{
    example_open_close();
    example_load_database();
    example_identify_file();
    example_set_get_parameters();
    example_set_get_flags();
    example_compile_check();
    return 0;
}
