/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/aggregation.hpp
*
*  Defines aggregation used for logical operators
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATION_HPP
#define DRACOSHA_VALIDATOR_AGGREGATION_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

enum class aggregation_id : int
{
    AND,
    OR,
    NOT
};

struct aggregation_op_tag;

struct aggregation_op
{
    using hana_tag=aggregation_op_tag;

    aggregation_id id;
    const char* open_token;
    const char* close_token;
    const char* conjunction_token;

    aggregation_op()=delete;
    ~aggregation_op()=default;
    aggregation_op(const aggregation_op& other)=default;
    aggregation_op(aggregation_op&& other)=default;
    aggregation_op& operator= (const aggregation_op& other)=default;
    aggregation_op& operator= (aggregation_op&& other)=default;

    template <typename T>
    aggregation_op(const T& other)
        : id(other.id),
          open_token(other.open_token),
          close_token(other.close_token),
          conjunction_token(other.conjunction_token)
    {}
};

//-------------------------------------------------------------

/**
 * @brief Pseudo logical operator
 */
template <typename DerivedT>
struct logical_op
{
    using hana_tag=aggregation_op_tag;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_HPP
