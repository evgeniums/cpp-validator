/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/element_aggregation.hpp
*
*  Defines helpers used for element aggregations.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_HPP
#define DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/aggregation.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

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

struct element_aggregation
{
    using hana_tag=element_aggregation_tag;

    template <typename PredicateT, typename EmptyFnT, typename StringT, typename PathT, typename AdapterT, typename HandlerT>
    static status invoke(PredicateT&& pred, EmptyFnT&& empt, StringT&& str, PathT&& path, AdapterT&& adapter, HandlerT&& handler);
};

struct is_element_aggregation_t
{
    template <typename PrevResultT, typename T>
    constexpr auto operator () (PrevResultT prev, T) const
    {
        using type=typename T::type;
        return hana::if_(
            prev,
            prev,
            hana::bool_
            <
                hana::is_a<element_aggregation_tag,type>
            >{}
        );
    }
};
constexpr is_element_aggregation_t is_element_aggregation{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_HPP
