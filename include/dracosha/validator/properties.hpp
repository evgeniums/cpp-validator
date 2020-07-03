/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/properties.hpp
*
*  Includes all headers with properties
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PROPERTIES_HPP
#define DRACOSHA_VALIDATOR_PROPERTIES_HPP

#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/properties/length.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_conjunction_of_t : public enable_to_string<string_conjunction_of_t>
{
    constexpr static const char* description="of";
};
constexpr string_conjunction_of_t string_conjunction_of{};

struct string_conjunction_aggregate_t : public enable_to_string<string_conjunction_aggregate_t>
{
    constexpr static const char* description=": ";
};
constexpr string_conjunction_aggregate_t string_conjunction_aggregate{};

struct string_conjunction_for_t : public enable_to_string<string_conjunction_for_t>
{
    constexpr static const char* description="for";
};
constexpr string_conjunction_for_t string_conjunction_for{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTIES_HPP
