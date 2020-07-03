/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/operator.hpp
*
*  Defines base operator
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATOR_HPP
#define DRACOSHA_VALIDATOR_OPERATOR_HPP

#include <string>
#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Tag for all operator clases
 */
struct operator_tag;

/**
 * @brief Base operator class
 */
template <typename DerivedT>
struct op : public enable_to_string<DerivedT>
{
    using hana_tag=operator_tag;
};

//-------------------------------------------------------------

struct logical_op_tag;

/**
 * @brief Pseudo logical operator
 */
template <typename DerivedT>
struct logical_op : public enable_to_string<DerivedT>
{
    using hana_tag=logical_op_tag;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATOR_HPP
