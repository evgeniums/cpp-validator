/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/properties/size.hpp
*
*  Defines "empty" property
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EMPTY_HPP
#define DRACOSHA_VALIDATOR_EMPTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/prepare_dispatcher.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_PROPERTY(empty)

//-------------------------------------------------------------
struct string_empty_t : public enable_to_string<string_empty_t>
{
    constexpr static const char* description="must be empty";
};
constexpr string_empty_t string_empty{};

//-------------------------------------------------------------
struct string_not_empty_t : public enable_to_string<string_not_empty_t>
{
    constexpr static const char* description="must be not empty";
};
constexpr string_not_empty_t string_not_empty{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EMPTY_HPP
