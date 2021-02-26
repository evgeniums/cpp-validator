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

#ifndef DRACOSHA_VALIDATOR_EMPTY_HPP
#define DRACOSHA_VALIDATOR_EMPTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

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

DRACOSHA_VALIDATOR_PROPERTY_FLAG(empty,string_empty_t::description,string_not_empty_t::description)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EMPTY_HPP
