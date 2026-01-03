/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace Recognition;

TEST(MagicVersionTest, magic_get_version)
{
    EXPECT_EQ(Magic::GetVersion(), "5.46");
}
