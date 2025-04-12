/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

TEST(magic_check_test, closed_magic_check_default_database)
{
    EXPECT_FALSE(magic{}.check());
}

TEST(magic_check_test, opened_magic_check_empty_path)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_FALSE(m.check({}));
}

TEST(magic_check_test, opened_magic_check_default_database)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_TRUE(m.check(DEFAULT_DATABASE_FILE));
}
