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
#include <dracosha/validator/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_of_conjunction_t : public enable_to_string<string_of_conjunction_t>
{
    constexpr static const char* description="of";
};
constexpr string_of_conjunction_t string_of_conjunction{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTIES_HPP
