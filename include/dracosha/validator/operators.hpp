/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators.hpp
*
*  Includes all headers with operators
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATORS_HPP
#define DRACOSHA_VALIDATOR_OPERATORS_HPP

#include <dracosha/validator/operators/and.hpp>
#include <dracosha/validator/operators/or.hpp>
#include <dracosha/validator/operators/not.hpp>
#include <dracosha/validator/operators/comparison.hpp>
#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_must_be_t : public enable_to_string<string_must_be_t>
{
    constexpr static const char* description="must be";
};
constexpr string_must_be_t string_must_be{};

struct string_must_not_be_t : public enable_to_string<string_must_not_be_t>
{
    constexpr static const char* description="must not be";
};
constexpr string_must_not_be_t string_must_not_be{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATORS_HPP
