/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

TEST(magic_compile_test, compile_empty_path)
{
    EXPECT_FALSE(magic::compile({}));
}

TEST(magic_compile_test, compile_default_database)
{
    EXPECT_TRUE(magic::compile(DEFAULT_DATABASE_FILE));
}
