/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

TEST(magic_check_test, check_empty_path)
{
    EXPECT_FALSE(magic::check({}));
}

TEST(magic_check_test, check_default_database)
{
    EXPECT_TRUE(magic::check(DEFAULT_DATABASE_FILE));
}
