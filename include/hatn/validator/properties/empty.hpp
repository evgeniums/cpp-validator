/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/properties/size.hpp
*
*  Defines "empty" property.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_EMPTY_HPP
#define HATN_VALIDATOR_EMPTY_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/property.hpp>
#include <hatn/validator/utils/enable_to_string.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for property "empty".
 */
struct string_empty_t : public enable_to_string<string_empty_t>
{
    constexpr static const char* description="must be empty";
};
/**
 * @brief Instance of string descriptions helper for property "empty".
 */
constexpr string_empty_t string_empty{};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for property "not empty".
 */
struct string_not_empty_t : public enable_to_string<string_not_empty_t>
{
    constexpr static const char* description="must be not empty";
};

/**
 * @brief Instance of string descriptions helper for property "not empty".
 */
constexpr string_not_empty_t string_not_empty{};

HATN_VALIDATOR_PROPERTY_FLAG(empty,string_empty_t::description,string_not_empty_t::description)

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_EMPTY_HPP
