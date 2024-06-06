/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_check_test, closed_magic_check_default_database)
{
    EXPECT_FALSE(magic{}.check());
}

TEST(magic_check_test, opened_magic_check_empty_path)
{
    magic m;
    m.open(magic::flags::Mime);
    EXPECT_FALSE(m.check({}));
}

TEST(magic_check_test, opened_magic_check_default_database)
{
    magic m;
    m.open(magic::flags::Mime);
    EXPECT_TRUE(m.check());
}
