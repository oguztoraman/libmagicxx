/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_version_test.cpp
 * @brief Unit tests for Magic::GetVersion() static method.
 *
 * Tests the version query functionality that returns the
 * underlying libmagic library version string.
 *
 * @section version_test_purpose Purpose
 *
 * This test verifies that:
 * - GetVersion() is callable as a static method
 * - The returned version matches the expected bundled libmagic version
 *
 * @note The expected version ("5.47") corresponds to the bundled
 *       libmagic version in the external/file submodule.
 *
 * @see Magic::GetVersion()
 */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace Recognition;

TEST(MagicVersionTest, magic_get_version)
{
    EXPECT_EQ(Magic::GetVersion(), "5.47");
}
