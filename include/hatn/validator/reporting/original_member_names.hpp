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

#include <hatn/validator/reporting/dotted_member_names.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits to format nested members as original names decorated with brackets.
 *
 * For example: ["field1"]["subfield1_1"]["subfield1_1_1"] will be formatted as [field1][subfield1_1][subfield1_1_1]
 */
struct original_member_names_traits_t : public dotted_member_names_traits_t
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
