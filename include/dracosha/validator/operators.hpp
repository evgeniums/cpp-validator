/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators.hpp
*
*  Includes headers with operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATORS_HPP
#define DRACOSHA_VALIDATOR_OPERATORS_HPP

#include <dracosha/validator/operators/wrap_op.hpp>
#include <dracosha/validator/operators/invert_op.hpp>
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/aggregation/and.hpp>
#include <dracosha/validator/aggregation/or.hpp>
#include <dracosha/validator/aggregation/not.hpp>
#include <dracosha/validator/aggregation/any.hpp>
#include <dracosha/validator/aggregation/all.hpp>
#include <dracosha/validator/operators/comparison.hpp>
#include <dracosha/validator/operators/contains.hpp>
#include <dracosha/validator/operators/in.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATORS_HPP
