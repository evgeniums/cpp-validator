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

#ifndef HATN_VALIDATOR_OPERATORS_HPP
#define HATN_VALIDATOR_OPERATORS_HPP

#include <hatn/validator/operators/wrap_op.hpp>
#include <hatn/validator/operators/invert_op.hpp>
#include <hatn/validator/operators/flag.hpp>
#include <hatn/validator/operators/exists.hpp>
#include <hatn/validator/aggregation/and.hpp>
#include <hatn/validator/aggregation/or.hpp>
#include <hatn/validator/aggregation/not.hpp>
#include <hatn/validator/aggregation/any.hpp>
#include <hatn/validator/aggregation/all.hpp>
#include <hatn/validator/operators/comparison.hpp>
#include <hatn/validator/operators/contains.hpp>
#include <hatn/validator/operators/in.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_OPERATORS_HPP
