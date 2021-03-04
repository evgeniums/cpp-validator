/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/aggregation.hpp
*
*  Defines aggregation used for logical operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATION_HPP
#define DRACOSHA_VALIDATOR_AGGREGATION_HPP

#include <dracosha/validator/config.hpp>

#include <dracosha/validator/status.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Codes of aggregation operators.
 */
enum class aggregation_id : int
{
    AND,
    OR,
    NOT,
    ANY,
    ALL
};

struct aggregation_op_tag;
struct element_aggregation_tag
{
    template <typename T>
    constexpr bool operator == (T) const noexcept
    {
        return true;
    }
    template <typename T>
    constexpr bool operator != (T) const noexcept
    {
        return false;
    }

    template <typename T>
    constexpr friend bool operator == (T, const element_aggregation_tag&) noexcept
    {
        return true;
    }
    template <typename T>
    constexpr friend bool operator != (T, const element_aggregation_tag&) noexcept
    {
        return true;
    }
};


//! @todo Move to separate file
struct element_aggregation
{
    using hana_tag=element_aggregation_tag;

    template <typename PredicateT, typename EmptyFnT, typename StringT, typename PathT, typename AdapterT, typename HandlerT>
    static status invoke(PredicateT&& pred, EmptyFnT&& empt, StringT&& str, PathT&& path, AdapterT&& adapter, HandlerT&& handler);

    template <typename T>
    constexpr bool operator == (T) const noexcept
    {
        return true;
    }
    template <typename T>
    constexpr bool operator != (T) const noexcept
    {
        return false;
    }

    template <typename T>
    constexpr friend bool operator == (T, const element_aggregation&) noexcept
    {
        return true;
    }
    template <typename T>
    constexpr friend bool operator != (T, const element_aggregation&) noexcept
    {
        return true;
    }
};

/**
 * @brief Aggregation operator descriptor.
 */
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

    /**
     * @brief Constructor.
     * @param other Source descriptor.
     */
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
 * @brief Pseudo logical operator.
 */
template <typename DerivedT>
struct logical_op
{
    using hana_tag=aggregation_op_tag;

    constexpr static const char* open_str()
    {
        return DerivedT::open_token;
    }

    constexpr static const char* close_str()
    {
        return DerivedT::close_token;
    }

    constexpr static const char* conjunction_str()
    {
        return DerivedT::conjunction_token;
    }
};

/**
 * @brief Pseudo aggregation operator.
 */
template <typename DerivedT>
struct aggregate_op
{
    using hana_tag=aggregation_op_tag;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_HPP
