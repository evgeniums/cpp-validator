/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/original_member_names.hpp
*
* Defines original member names formatter.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_ORIGINAL_MEMBER_NAMES_HPP
#define HATN_VALIDATOR_ORIGINAL_MEMBER_NAMES_HPP

#include <hatn/validator/reporting/member_names.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits to format nested members as original names decorated with brackets.
 *
 * For example: ["field1"]["subfield1_1"]["subfield1_1_1"] will be formatted as [field1][subfield1_1][subfield1_1_1]
 */
struct original_member_names_traits_t
{
    /**
     * @brief Get string for conjunction of nested member names.
     * @return String "." to use to join nested member names in the member's path.
     */
    static auto member_names_conjunction()
    {
        return "";
    }

    static auto member_property_conjunction()
    {
        return "";
    }

    std::string operator ()(size_t index) const
    {
        return std::to_string(index);
    }

    template <typename T>
    std::string format_aggregation(const T& aggregation) const
    {
        return std::string(aggregation.name);
    }

    /**
     * @brief Check if nested memebr names must be joined in reverse order.
     */
    constexpr static const bool is_reverse_member_names_order=false;

    /**
     * @brief Check if property of a memebr must be joined in reverse order.
     */
    constexpr static const bool is_reverse_member_property_order=false;

    /**
     * @brief Check if explicit names can be used.
     *
     * Default is allowed.
     */
    constexpr static const bool is_allow_explicit_member_names=false;

    brackets_decorator_t decorator;
};

/**
  @brief Formatter of member names in original format.

  For example: ["field1"]["subfield1_1"]["subfield1_1_1"] will be formatted as [field1][subfield1_1][subfield1_1_1]
*/
constexpr member_names<original_member_names_traits_t> original_member_names{original_member_names_traits_t{}};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_ORIGINAL_MEMBER_NAMES_HPP
