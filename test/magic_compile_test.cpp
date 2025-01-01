/* SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_compile_test, closed_magic_compile_default_database)
{
    EXPECT_FALSE(magic{}.compile());
}

TEST(magic_compile_test, opened_magic_compile_empty_path)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_FALSE(m.compile({}));
}

TEST(magic_compile_test, opened_magic_compile_default_database)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_TRUE(m.compile());
}
