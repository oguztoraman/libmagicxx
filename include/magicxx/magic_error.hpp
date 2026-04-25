/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_error.hpp
 * @brief Compact enum-based errors for noexcept identification APIs.
 * @ingroup magic_errors
 *
 * This header defines `IdentifyError` — a small, non-allocating error payload
 * intended for use with `Magic::ExpectedFileTypeT` (an alias for
 * `std::expected<FileTypeT, IdentifyError>`). Use these error codes when you
 * prefer noexcept APIs over exceptions (for example, when calling
 * `Magic::IdentifyFile(path, std::nothrow)`).
 *
 * The `ToStringView()` helper provides a short, human-readable message for
 * diagnostics and logging without allocating.
 *
 * @section error_handling Error handling patterns
 *
 * Prefer the noexcept overloads when you want to avoid exceptions:
 *
 * @code{.cpp}
 * auto result = magic.IdentifyFile(path, std::nothrow);
 * if (!result) {
 *     std::cerr << "Error: " << Recognition::ToStringView(result.error()) << '\n';
 * }
 * @endcode
 *
 * @section mapping Mapping to throwing API
 *
 * The `IdentifyError` codes correspond to the exceptions thrown by the
 * throwing overloads of `Magic::IdentifyFile()`:
 * - `IdentifyError::MagicIsClosed` -> `Recognition::MagicIsClosed`
 * - `IdentifyError::MagicDatabaseNotLoaded` -> `Recognition::MagicDatabaseNotLoaded`
 * - `IdentifyError::EmptyPath` -> `Recognition::EmptyPath`
 * - `IdentifyError::PathDoesNotExist` -> `Recognition::PathDoesNotExist`
 * - `IdentifyError::IdentifyFailed` -> `Recognition::MagicIdentifyFileError`
 *
 * @see Magic
 * @see Magic::ExpectedFileTypeT
 * @see Magic::IdentifyFile
 * @see Magic::IdentifyFiles
 *
 * @since 11.0.0
 */

#ifndef MAGIC_ERROR_HPP
#define MAGIC_ERROR_HPP

#include <cstdint>
#include <string_view>

namespace Recognition {
/**
 * @defgroup magic_errors Error Codes
 * @ingroup magic_core
 * @brief Lightweight enum-based error codes for noexcept APIs.
 *
 * These values are returned by the `std::expected`-based noexcept overloads
 * used across the Magic API. They are intentionally compact (one byte)
 * and suitable for low-allocation code paths.
 *
 * @{
 */

/**
 * @enum IdentifyError
 * @brief Compact error codes returned by noexcept identification APIs.
 *
 * Use these values when calling the noexcept overloads of identification
 * APIs (for example: `Magic::IdentifyFile(path, std::nothrow)`). See the
 * mapping section above for correspondence with throwing exceptions.
 *
 * @since 11.0.0
 */
enum class IdentifyError : std::uint8_t {
    /* clang-format off */
    MagicIsClosed = 0,          /**< Magic instance is closed. */
    MagicDatabaseNotLoaded = 1, /**< Magic database not loaded. */
    EmptyPath = 2,              /**< Path is empty. */
    PathDoesNotExist = 3,       /**< Path does not exist. */
    IdentifyFailed = 4          /**< Identify operation failed. */
    /* clang-format on */
};

/**
 * @brief Get a short textual representation for an `IdentifyError`.
 *
 * Returns a `std::string_view` pointing to a static string literal
 * describing the error. This function does not allocate and is suitable for
 * use in noexcept or low-allocation code paths.
 *
 * @param identify_error The error code to describe.
 * @return A human-readable message describing the error.
 *
 * @code{.cpp}
 * auto result = magic.IdentifyFile(path, std::nothrow);
 * if (!result) {
 *     std::cerr << "Error: " << Recognition::ToStringView(result.error()) << '\n';
 * }
 * @endcode
 *
 * @since 11.0.0
 */
constexpr std::string_view ToStringView(IdentifyError identify_error) noexcept
{
    switch (identify_error) {
    case IdentifyError::MagicIsClosed: return "Magic instance is closed";
    case IdentifyError::MagicDatabaseNotLoaded:
        return "Magic database not loaded";
    case IdentifyError::EmptyPath       : return "Path is empty";
    case IdentifyError::PathDoesNotExist: return "Path does not exist";
    case IdentifyError::IdentifyFailed  : return "Identify operation failed";
    }
    return "Unknown identify error";
}

/** @} magic_errors */
} /* namespace Recognition */

#endif /* MAGIC_ERROR_HPP */
