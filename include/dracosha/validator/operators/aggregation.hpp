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

enum class aggregation : int
{
    AND,
    OR,
    NOT
};

struct aggregation_op_tag;

struct aggregation_op
{
    using hana_tag=aggregation_op_tag;

    aggregation id;
    const char* open_tag;
    const char* close_tag;
    const char* conjunction_tag;

    aggregation_op()=delete;
    ~aggregation_op()=default;
    aggregation_op(const aggregation_op& other)=default;
    aggregation_op(aggregation_op&& other)=default;
    aggregation_op& operator= (const aggregation_op& other)=default;
    aggregation_op& operator= (aggregation_op&& other)=default;

    template <typename T>
    aggregation_op(const T& other)
        : id(other.id),
          open_tag(other.open_tag),
          close_tag(other.close_tag),
          conjunction_tag(other.conjunction_tag)
    {}
};

//-------------------------------------------------------------

/**
 * @brief Pseudo logical operator
 */
template <typename DerivedT>
struct logical_op : public enable_to_string<DerivedT>
{
    using hana_tag=aggregation_op_tag;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_HPP
