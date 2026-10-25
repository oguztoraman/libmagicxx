/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_parameters.hpp
 * @brief Header file for the MagicParameters class.
 *
 * This file contains the MagicParameters class, a C++23 value type that
 * owns the libmagicxx tuning parameters: the `Parameters` enum and the
 * operations that convert parameters to human-readable strings. Every
 * public operation is `constexpr` and `noexcept`. The authoritative
 * values live in libmagic and are reached through Magic::GetParameter()
 * and Magic::SetParameter().
 *
 * @author Oğuz Toraman
 * @copyright Copyright (c) 2022-2026 Oğuz Toraman. LGPL-3.0-only.
 *
 * @see https://github.com/oguztoraman/libmagicxx
 * @see https://github.com/file/file (underlying libmagic)
 */

#ifndef MAGIC_PARAMETERS_HPP
#define MAGIC_PARAMETERS_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>

#include "inplace_string.hpp"

/**
 * @namespace Recognition
 * @brief Root namespace for the libmagicxx library.
 *
 * @since 10.0.0
 */
namespace Recognition {
/**
 * @class MagicParameters
 * @ingroup magic_core
 *
 * @brief A modern C++23 value class rendering Magic tuning parameters.
 *
 * MagicParameters owns the `Parameters` enum and holds a snapshot of
 * parameter values, converting them to human-readable strings. The
 * authoritative values are kept by libmagic itself and reached through
 * Magic::GetParameter() and Magic::SetParameter().
 *
 * ### Key Features
 *
 * - **Value Semantics**: Trivially copyable and movable parameter snapshot.
 * - **Exception Safety**: Every class operation is `constexpr noexcept`.
 * - **Transport**: Used directly as the argument and result of
 *   Magic::GetParameters() and Magic::SetParameters().
 * - **Iteration**: Range-for yields the stored entries as
 *   `ParameterValueT` pairs in ascending parameter ordinal order, skipping
 *   the unset slots.
 * - **Keyed Lookup**: Read-only map-style access via `operator[]` and
 *   `Contains()`.
 * - **Size Queries**: `Count()` and `Empty()` report how many entries the
 *   snapshot holds.
 * - **Rendering**: `ToString()` renders the stored entries as names, with
 *   or without their values. A single `Parameters` value and a whole
 *   snapshot also render with `std::format`.
 * - **Comparison**: Snapshots are comparable with `operator==` and
 *   `operator<=>`.
 *
 * @note Snapshots are read-only views of the values taken at snapshot
 *       time; change the authoritative libmagic values with
 *       Magic::SetParameter() or Magic::SetParameters(), not the snapshot.
 *
 * ### Usage Examples
 *
 * @code{.cpp}
 * #include <magic.hpp>
 * #include <print>
 *
 * using namespace Recognition;
 *
 * // Name of a single parameter
 * std::println("{}", MagicParameters{MagicParameters::Parameters::RegexMax,
 *     1UZ}
 *     .ToString(MagicParameters::StringFormat::Names)); // "RegexMax"
 *
 * // Snapshot of all current values of a Magic instance
 * Magic magic{Magic::FlagsT::Mime};
 * auto parameters = magic.GetParameters();
 * std::println("{}", parameters.ToString());  // "IndirMax: 15, NameMax: 30,..."
 *
 * // Keyed lookup of a single parameter value
 * if (auto bytes = parameters[MagicParameters::Parameters::BytesMax]) {
 *     std::println("BytesMax is {}", *bytes);
 * }
 * @endcode
 *
 * @see Magic for the file type identification class using these parameters.
 *
 * @since 11.0.0
 */
class MagicParameters {
public:
    /**
     * @brief Parameters for tuning Magic behavior limits.
     * @ingroup magic_core
     *
     * The Parameters enum provides access to various internal limits that
     * control how deeply Magic analyzes files. Adjusting these can help
     * balance between thoroughness and performance.
     *
     * ### Common Parameter Usage
     *
     * @code{.cpp}
     * Magic magic{Magic::FlagsT::Mime};
     *
     * // Limit bytes scanned for faster performance
     * magic.SetParameter(Magic::ParametersT::BytesMax, 1024 * 1024);
     *
     * // Get current value
     * auto bytes = magic.GetParameter(Magic::ParametersT::BytesMax);
     *
     * // Get all parameters as "name: value" entries
     * auto parameters = magic.GetParameters();
     * std::println("{}", parameters.ToString());
     *
     * // Print a single parameter by name
     * std::println("{}", Magic::ParametersT::BytesMax);  // "BytesMax"
     * @endcode
     *
     * @see Magic::SetParameter() to modify a single parameter
     * @see Magic::SetParameters() to modify multiple parameters
     * @see Magic::GetParameter() to retrieve a single parameter value
     * @see Magic::GetParameters() to retrieve all parameter values
     *
     * @since 11.0.0
     */
    enum class Parameters : std::uint8_t {
        /* clang-format off */
        IndirMax     = 0UZ, /**< Maximum recursion depth for indirect magic (default: 15). */
        NameMax      = 1UZ, /**< Maximum use count for name/use magic entries (default: 30). */
        ElfPhnumMax  = 2UZ, /**< Maximum ELF program headers to process (default: 128). */
        ElfShnumMax  = 3UZ, /**< Maximum ELF section headers to process (default: 32768). */
        ElfNotesMax  = 4UZ, /**< Maximum ELF notes to process (default: 256). */
        RegexMax     = 5UZ, /**< Maximum regex search length in bytes (default: 8192). */
        BytesMax     = 6UZ, /**< Maximum bytes to read from file (default: 7340032 = 7MB). */
        EncodingMax  = 7UZ, /**< Maximum bytes to scan for encoding detection (default: 1048576 = 1MB). */
        ElfShsizeMax = 8UZ, /**< Maximum ELF section size to process (default: 134217728 = 128MB). */
        MagWarnMax   = 9UZ  /**< Maximum warnings to tolerate from magic file (default: 64). */
        /* clang-format on */
    };

    /**
     * @brief Format selector for MagicParameters::ToString().
     * @ingroup magic_core
     *
     * Chooses whether the string contains only parameter names or names
     * paired with their values.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     {{Parameters::RegexMax, 8192UZ}, {Parameters::BytesMax, 1048576UZ}}
     * };
     *
     * // "RegexMax, BytesMax"
     * parameters.ToString(StringFormat::Names);
     *
     * // "RegexMax: 8192, BytesMax: 1048576"
     * parameters.ToString(StringFormat::NamesAndValues);
     * @endcode
     *
     * @see MagicParameters::ToString()
     *
     * @since 11.0.0
     */
    enum class StringFormat : std::uint8_t {
        Names,         /**< Render only the parameter names. */
        NamesAndValues /**< Render each name followed by its value. */
    };

    /**
     * @typedef ParameterValueT
     *
     * @brief Key-value pair representing a single parameter and its value.
     *
     * Entry type of the snapshot: accepted by the initializer-list
     * constructor and yielded by iteration in ascending parameter ordinal
     * order.
     *
     * @since 11.0.0
     */
    using ParameterValueT = std::pair<Parameters, std::size_t>;

    /**
     * @brief Default capacity of the string returned by ToString().
     *
     * The smallest capacity that fits every parameter with its maximum
     * value and the default separators.
     *
     * @since 11.0.0
     */
    static constexpr std::size_t TO_STRING_CAPACITY{337UZ};

    /**
     * @class Iterator
     * @brief Forward iterator over the stored entries of a snapshot.
     *
     * The Iterator walks the stored entries of a MagicParameters snapshot
     * in ascending parameter ordinal order, skipping the parameters that
     * hold no value. Dereferencing yields the entry by value as a
     * `ParameterValueT` pair, so the type models
     * `std::forward_iterator`.
     *
     * @warning The iterator holds a pointer to the snapshot it walks, not
     *          a copy of its contents. Iterators taken from a temporary
     *          snapshot — for example the result of
     *          Magic::GetParameters() — dangle once that temporary is
     *          destroyed. Bind the snapshot to a named variable first when
     *          storing iterators across statements.
     *
     * @see MagicParameters::begin()
     * @see MagicParameters::end()
     *
     * @since 11.0.0
     */
    class Iterator {
    public:
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using value_type
            = ParameterValueT; /**< Entry yielded by dereference. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using difference_type = std::ptrdiff_t; /**< Signed distance type. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using reference = ParameterValueT; /**< Dereference yields by value. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using iterator_category = std::
            forward_iterator_tag; /**< Iterator category. */
        /* NOLINTNEXTLINE(readability-identifier-naming) */
        using iterator_concept = std::
            forward_iterator_tag; /**< Iterator concept. */

        /**
         * @brief Construct a past-the-end iterator.
         *
         * A default-constructed iterator compares equal to the end of any
         * snapshot, as required for forward iterators.
         *
         * @since 11.0.0
         */
        constexpr Iterator() noexcept
          : m_index{END_INDEX}
        { }

        /**
         * @brief Advance to the next stored entry, skipping unset slots.
         *
         * @returns Reference to this iterator.
         *
         * @since 11.0.0
         */
        constexpr Iterator& operator++() noexcept
        {
            ++m_index;
            SkipUnset();
            return *this;
        }

        /**
         * @brief Advance to the next stored entry, returning the previous
         *        position.
         *
         * @returns A copy of this iterator taken before the increment.
         *
         * @since 11.0.0
         */
        constexpr Iterator operator++(int) noexcept
        {
            const Iterator previous{*this};
            ++*this;
            return previous;
        }

        /**
         * @brief Dereference the iterator.
         *
         * Iteration only ever rests on slots that hold a value, so the
         * entry is always present at a dereferenced position.
         *
         * @returns The parameter-value pair at the current ordinal. The
         *          behavior is undefined at the end position.
         *
         * @since 11.0.0
         */
        [[nodiscard]] constexpr ParameterValueT operator*() const noexcept
        {
            return {
                static_cast<Parameters>(m_index),
                m_owner->Slot(m_index).value_or(0UZ)
            };
        }

        /**
         * @brief Compare two iterators by ordinal position.
         *
         * Comparing iterators over different snapshots is supported only
         * against their end positions; comparing two stored positions
         * across snapshots is unspecified.
         *
         * @param[in] lhs Left-hand side iterator.
         * @param[in] rhs Right-hand side iterator.
         *
         * @returns `true` if both iterators sit at the same parameter
         *          ordinal.
         *
         * @since 11.0.0
         */
        [[nodiscard]] friend constexpr bool operator==(
            const Iterator& lhs,
            const Iterator& rhs
        ) noexcept
        {
            return lhs.m_index == rhs.m_index;
        }

    private:
        /**
         * @brief Past-the-end ordinal of the inline value container,
         *        mirroring the enclosing `PARAMETER_COUNT`.
         */
        static constexpr std::size_t END_INDEX{
            static_cast<std::size_t>(Parameters::MagWarnMax) + 1UZ
        };

        /**
         * @brief Construct over a snapshot at an ordinal, skipping forward
         *        to the next stored entry.
         *
         * @param[in] owner The snapshot being iterated.
         * @param[in] index Starting parameter ordinal.
         */
        constexpr Iterator(
            const MagicParameters* owner,
            std::size_t            index
        ) noexcept
          : m_owner{owner}
          , m_index{index}
        {
            SkipUnset();
        }

        /** @brief Advance the ordinal past slots that hold no value. */
        constexpr void SkipUnset() noexcept
        {
            while (
                m_index < PARAMETER_COUNT && !m_owner->Slot(m_index).has_value()
            ) {
                ++m_index;
            }
        }

        const MagicParameters* m_owner{}; /**< Snapshot the iterator walks. */
        std::size_t m_index{}; /**< Current ordinal, END_INDEX at end. */

        friend class MagicParameters; /**< Grants snapshot construction. */
    };

    /**
     * @brief Default constructor. Stores no parameter values.
     *
     * @code{.cpp}
     * MagicParameters parameters;  // No values stored
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicParameters() noexcept = default;

    /**
     * @brief Construct from a single parameter and its value.
     *
     * The resulting snapshot holds exactly the given parameter-value entry.
     * A parameter's name alone renders with `StringFormat::Names`, which
     * ignores the value. Use Magic::GetParameter() to read the parameter's
     * actual libmagic value.
     *
     * @note A `parameter` that is not a valid ordinal is silently
     *       discarded, leaving an empty snapshot.
     *
     * @param[in] parameter The parameter to store.
     * @param[in] value     The value for the parameter.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     MagicParameters::Parameters::BytesMax, 1048576UZ
     * };
     * std::println("{}", parameters.ToString());  // "BytesMax: 1048576"
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicParameters(Parameters parameter, std::size_t value) noexcept
    {
        Store(parameter, value);
    }

    /**
     * @brief Construct from a list of parameter-value pairs.
     *
     * The resulting snapshot holds exactly the given entries; a repeated
     * parameter keeps its last value.
     *
     * @note Entries whose key is not a valid ordinal are silently
     *       discarded; a list of only invalid keys leaves an empty
     *       snapshot.
     *
     * @warning This replaces the map-based transport API, where an
     *          out-of-range key was reported through
     *          Magic::SetParameters(). Snapshots now drop invalid ordinals
     *          at build time, so Magic::SetParameters() can no longer
     *          detect them and applies only the surviving entries.
     *
     * @param[in] entries Pairs of a parameter and its value.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     {{MagicParameters::Parameters::BytesMax, 1048576UZ},
     *      {MagicParameters::Parameters::RegexMax, 8192UZ}}
     * };
     * @endcode
     *
     * @since 11.0.0
     */
    constexpr MagicParameters(
        std::initializer_list<ParameterValueT> entries
    ) noexcept
    {
        for (const auto& [parameter, value] : entries) {
            Store(parameter, value);
        }
    }

    /**
     * @brief Copy constructor.
     *
     * @param[in] other The MagicParameters instance to copy.
     *
     * @since 11.0.0
     */
    constexpr MagicParameters(const MagicParameters& other) noexcept = default;

    /**
     * @brief Move constructor.
     *
     * @param[in,out] other The MagicParameters instance to move from.
     *
     * @since 11.0.0
     */
    constexpr MagicParameters(MagicParameters&& other) noexcept = default;

    /**
     * @brief Destructor.
     *
     * @since 11.0.0
     */
    ~MagicParameters() = default;

    /**
     * @brief Copy assignment operator.
     *
     * @param[in] other The MagicParameters instance to copy.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    constexpr MagicParameters& operator=(
        const MagicParameters& other
    ) noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * @param[in,out] other The MagicParameters instance to move from.
     *
     * @returns Reference to this instance.
     *
     * @since 11.0.0
     */
    constexpr MagicParameters& operator=(
        MagicParameters&& other
    ) noexcept = default;

    /**
     * @brief Return an iterator to the first stored entry.
     *
     * Leading slots that hold no value are skipped, so iteration yields
     * exactly the stored entries in ascending parameter ordinal order.
     *
     * @code{.cpp}
     * for (const auto [parameter, value] : parameters) {
     *     std::println("{}: {}", parameter, value);
     * }
     * @endcode
     *
     * @returns Iterator at the stored entry with the lowest ordinal.
     *
     * @see Iterator
     *
     * @since 11.0.0
     */
    /* NOLINTNEXTLINE(readability-identifier-naming) */
    [[nodiscard]] constexpr Iterator begin() const noexcept
    {
        return Iterator{this, 0UZ};
    }

    /**
     * @brief Return a past-the-end iterator.
     *
     * @returns Iterator that compares equal once iteration has consumed
     *          every stored entry.
     *
     * @see Iterator
     *
     * @since 11.0.0
     */
    /* NOLINTNEXTLINE(readability-identifier-naming) */
    [[nodiscard]] constexpr Iterator end() const noexcept
    {
        return Iterator{this, PARAMETER_COUNT};
    }

    /**
     * @brief Report whether a parameter holds a value in the snapshot.
     *
     * @param[in] parameter The parameter to look up.
     *
     * @returns `true` if `parameter` holds a value, `false` if it is unset
     *          or is not a valid ordinal.
     *
     * @see operator[]()
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool Contains(Parameters parameter) const noexcept
    {
        const auto ordinal{
            static_cast<std::size_t>(std::to_underlying(parameter))
        };
        return ordinal < PARAMETER_COUNT && Slot(ordinal).has_value();
    }

    /**
     * @brief Count the stored entries.
     *
     * @returns The number of parameters that hold a value in this
     *          snapshot.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     {{MagicParameters::Parameters::BytesMax, 1024UZ},
     *      {MagicParameters::Parameters::RegexMax, 8192UZ}}
     * };
     * parameters.Count();  // 2
     * @endcode
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr std::size_t Count() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count_if(
            m_values,
            [](const std::optional<std::size_t>& slot) noexcept {
                return slot.has_value();
            }
        ));
    }

    /**
     * @brief Check whether the snapshot stores no entries.
     *
     * @returns `true` if `Count() == 0`, `false` otherwise.
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr bool Empty() const noexcept
    {
        return Count() == 0UZ;
    }

    /**
     * @brief Convert the stored parameter values to a string.
     *
     * The result is a fixed-capacity Utility::InplaceString, so rendering
     * never throws; text beyond `CapacityV` characters
     * is truncated (see InplaceString::IsTruncated()).
     *
     * @tparam CapacityV Capacity, in characters, of the returned
     *                   InplaceString (default: `TO_STRING_CAPACITY`).
     *
     * @param[in] format              Whether to render only the parameter
     *                                names or the names with their values
     *                                (default: `StringFormat::NamesAndValues`).
     * @param[in] value_separator     String separating each name and value
     *                                (default: `: `). Used only with
     *                                `StringFormat::NamesAndValues`.
     * @param[in] parameter_separator String separating the entries
     *                                (default: `, `).
     *
     * @returns The stored entries in parameter order — e.g.
     *          "RegexMax: 8192, BytesMax: 1048576" with
     *          `StringFormat::NamesAndValues`, or "RegexMax, BytesMax" with
     *          `StringFormat::Names`. Empty when no value is stored.
     *
     * @code{.cpp}
     * MagicParameters parameters{
     *     {{MagicParameters::Parameters::BytesMax, 1048576UZ},
     *      {MagicParameters::Parameters::RegexMax, 8192UZ}}
     * };
     * auto text = parameters.ToString();  // "RegexMax: 8192, BytesMax: 1048576"
     * auto names = parameters.ToString(
     *     MagicParameters::StringFormat::Names);  // "RegexMax, BytesMax"
     * @endcode
     *
     * @since 11.0.0
     */
    template <std::size_t CapacityV = TO_STRING_CAPACITY>
    [[nodiscard]] constexpr Utility::InplaceString<CapacityV> ToString(
        StringFormat format = StringFormat::NamesAndValues,
        /* NOLINTNEXTLINE(bugprone-easily-swappable-parameters) */
        std::string_view value_separator     = ": ",
        std::string_view parameter_separator = ", "
    ) const noexcept
    {
        using Utility::InplaceString;
        InplaceString<CapacityV> result{};
        for (
            const auto [value, name] :
            std::views::zip(m_values, TO_STRING_NAMES)
        ) {
            if (value.has_value()) {
                if (!result.Empty()) {
                    result.Append(parameter_separator);
                }
                result.Append(name);
                if (format == StringFormat::NamesAndValues) {
                    result.Append(value_separator);
                    result.Append(
                        std::string_view{
                            InplaceString<CapacityV>::Format("{}", *value)
                        }
                    );
                }
            }
        }
        return result;
    }

    /**
     * @brief Return the stored value of a parameter, if any.
     *
     * Map-like keyed access mirroring the C++23 const `std::map::operator[]`
     * overload: reports a missing entry as `std::nullopt`.
     *
     * @param[in] parameter The parameter to look up.
     *
     * @returns The stored value, or `std::nullopt` if `parameter` holds no
     *          value in this snapshot or is not a valid ordinal.
     *
     * @code{.cpp}
     * if (auto bytes = parameters[
     *         MagicParameters::Parameters::BytesMax
     *     ]) {
     *     std::println("BytesMax is {}", *bytes);
     * }
     * @endcode
     *
     * @see Contains()
     *
     * @since 11.0.0
     */
    [[nodiscard]] constexpr std::optional<std::size_t> operator[](
        Parameters parameter
    ) const noexcept
    {
        const auto ordinal{
            static_cast<std::size_t>(std::to_underlying(parameter))
        };
        if (ordinal < PARAMETER_COUNT) {
            return Slot(ordinal);
        }
        return std::nullopt;
    }

    /**
     * @brief Compare two parameter snapshots for equality.
     *
     * @param[in] lhs Left-hand side snapshot.
     * @param[in] rhs Right-hand side snapshot.
     *
     * @returns `true` if both snapshots store identical entries.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr bool operator==(
        const MagicParameters& lhs,
        const MagicParameters& rhs
    ) noexcept
    {
        return lhs.m_values == rhs.m_values;
    }

    /**
     * @brief Compare two parameter snapshots entry by entry.
     *
     * @param[in] lhs Left-hand side snapshot.
     * @param[in] rhs Right-hand side snapshot.
     *
     * @returns A `std::strong_ordering` result comparing the stored entries
     *          in parameter ordinal order.
     *
     * @since 11.0.0
     */
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(
        const MagicParameters& lhs,
        const MagicParameters& rhs
    ) noexcept
    {
        return lhs.m_values <=> rhs.m_values;
    }

private:
    /**
     * @brief Number of Parameters ordinals, and capacity of the inline
     *        value container.
     */
    static constexpr std::size_t PARAMETER_COUNT{
        static_cast<std::size_t>(Parameters::MagWarnMax) + 1UZ
    };

    static_assert(
        Iterator::END_INDEX == PARAMETER_COUNT,
        "Iterator::END_INDEX must stay the past-the-end ordinal of the "
        "inline container"
    );

    /**
     * @brief Return the inline storage slot for an ordinal.
     *
     * @param[in] ordinal The parameter ordinal to locate; must be below
     *                    `PARAMETER_COUNT`, which every caller checks
     *                    first.
     *
     * @returns Reference to the slot holding the parameter's value.
     */
    /* NOLINTNEXTLINE(bugprone-exception-escape) */
    [[nodiscard]] constexpr const std::optional<std::size_t>& Slot(
        std::size_t ordinal
    ) const noexcept
    {
        return m_values.at(ordinal);
    }

    /**
     * @brief Human-readable name of each Parameters ordinal.
     *
     * Indexed by ordinal; consumed by ToString() and the formatter
     * specializations.
     */
    static constexpr std::array<std::string_view, PARAMETER_COUNT>
        TO_STRING_NAMES{
            "IndirMax",
            "NameMax",
            "ElfPhnumMax",
            "ElfShnumMax",
            "ElfNotesMax",
            "RegexMax",
            "BytesMax",
            "EncodingMax",
            "ElfShsizeMax",
            "MagWarnMax"
        };

    static_assert(
        TO_STRING_NAMES.size() == PARAMETER_COUNT,
        "TO_STRING_NAMES must cover every Parameters ordinal"
    );

    static_assert(
        []() noexcept {
            constexpr std::size_t MAX_DIGITS{
                std::numeric_limits<std::size_t>::digits10 + 1
            };
            std::size_t required{};
            for (const auto name : TO_STRING_NAMES) {
                required += name.size()
                          + 2UZ /* ": " */
                          + MAX_DIGITS
                          + 2UZ /* ", " */;
            }
            return required - 2UZ; /* no trailing separator */
        }() <= TO_STRING_CAPACITY,
        "TO_STRING_CAPACITY must fit every parameter with the default "
        "separators"
    );

    static_assert(
        std::ranges::all_of(
            TO_STRING_NAMES,
            [](const std::string_view name) {
                return !name.empty();
            }
        ),
        "TO_STRING_NAMES must carry a non-empty name for every ordinal"
    );

    /**
     * @brief Convert a parameter to its libmagic constant.
     *
     * @param[in] parameter The parameter to convert.
     *
     * @returns Libmagic-compatible `MAGIC_PARAM_*` constant, or
     *          `std::nullopt` if `parameter` is not a valid ordinal.
     */
    [[nodiscard]] static std::optional<int> ToUnderlying(
        Parameters parameter
    ) noexcept;

    /**
     * @brief Store a parameter value in the inline container.
     *
     * Resolves the slot directly by ordinal; an invalid ordinal has no
     * slot and the entry is silently discarded.
     *
     * @param[in] parameter The parameter to store.
     * @param[in] value     The value for the parameter.
     */
    /* NOLINTNEXTLINE(bugprone-exception-escape) */
    constexpr void Store(Parameters parameter, std::size_t value) noexcept
    {
        const auto ordinal{
            static_cast<std::size_t>(std::to_underlying(parameter))
        };
        if (ordinal < PARAMETER_COUNT) {
            m_values.at(ordinal) = value;
        }
    }

    /**
     * @brief Grants Magic access to the libmagic bridge and, through
     *        Magic::MagicPrivate, to the snapshot storage (`Store()`,
     *        `PARAMETER_COUNT`).
     */
    friend class Magic;

    /**
     * @brief Grants the std::formatter specializations declared in this
     *        header direct access to the name table (`TO_STRING_NAMES`)
     *        and the ordinal bound (`PARAMETER_COUNT`).
     */
    template <typename FormattableT, typename CharT>
    /* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
    friend struct std::formatter;

    std::array<std::optional<std::size_t>, PARAMETER_COUNT>
        m_values{}; /**< Stored value of each parameter, empty when absent. */
};

static_assert(
    std::is_trivially_copyable_v<MagicParameters>,
    "MagicParameters must stay trivially copyable for value semantics"
);

static_assert(
    std::forward_iterator<MagicParameters::Iterator>,
    "Iterator must stay a forward iterator so snapshots support multi-pass"
    " algorithms"
);
} /* namespace Recognition */

namespace std {
/**
 * @brief Format specialization rendering a `MagicParameters::Parameters`
 *        value as its parameter name.
 *
 * Makes the enum usable directly with `std::format` / `std::print` /
 * `std::println`, rendering the name of the parameter (e.g. "BytesMax"),
 * or "Invalid(N)" with the ordinal when no enumerator covers the value,
 * consistent with `MagicParameters::ToString()`. Format specs (fill,
 * alignment, width) are applied to the rendered name.
 *
 * @code{.cpp}
 * std::println(
 *     "{}", Recognition::MagicParameters::Parameters::BytesMax
 * );  // "BytesMax"
 * @endcode
 *
 * @since 11.0.0
 */
template <>
/* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
struct formatter<Recognition::MagicParameters::Parameters, char>
  : formatter<std::string_view> {
    /**
     * @brief Format `parameter` into `ctx` as its parameter name.
     *
     * @param[in] parameter The parameter to render.
     * @param[in] ctx       The output format context.
     *
     * @returns Iterator past the last formatted character.
     */
    template <typename FormatContextT>
    auto format(
        const Recognition::MagicParameters::Parameters parameter,
        FormatContextT&                                ctx
    ) const
    {
        const auto ordinal{
            static_cast<std::size_t>(std::to_underlying(parameter))
        };
        if (ordinal < Recognition::MagicParameters::PARAMETER_COUNT) {
            return formatter<std::string_view>::format(
                Recognition::MagicParameters::TO_STRING_NAMES.at(ordinal),
                ctx
            );
        }
        const auto invalid = Recognition::Utility::InplaceString<32UZ>::Format(
            "Invalid({})",
            std::to_underlying(parameter)
        );
        return formatter<std::string_view>::format(
            std::string_view{invalid},
            ctx
        );
    }
};

/**
 * @brief Format specialization rendering a `MagicParameters` snapshot as
 *        its "name: value" entries.
 *
 * Makes the snapshot usable directly with `std::format` / `std::print` /
 * `std::println`, rendering the stored entries in parameter order (e.g.
 * "RegexMax: 8192, BytesMax: 1048576"), consistent with
 * `MagicParameters::ToString()`. Format specs (fill, alignment, width)
 * are applied to the rendered entries.
 *
 * @code{.cpp}
 * std::println("{}", parameters);  // "IndirMax: 15, NameMax: 30,..."
 * @endcode
 *
 * @since 11.0.0
 */
template <>
/* NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp) */
struct formatter<Recognition::MagicParameters, char>
  : formatter<std::string_view> {
    /**
     * @brief Format `parameters` into `ctx` as their entries.
     *
     * @param[in] parameters The snapshot to render.
     * @param[in] ctx        The output format context.
     *
     * @returns Iterator past the last formatted character.
     */
    template <typename FormatContextT>
    auto format(
        const Recognition::MagicParameters& parameters,
        FormatContextT&                     ctx
    ) const
    {
        return formatter<std::string_view>::format(
            std::string_view{parameters.ToString()},
            ctx
        );
    }
};
} /* namespace std */

#endif /* MAGIC_PARAMETERS_HPP */
