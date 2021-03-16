/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/foreach_if.hpp
*
*  Defines helpers to iterate for each element of heterogeneous type with conditional predicate.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FOREACH_IF_HPP
#define DRACOSHA_VALIDATOR_FOREACH_IF_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/heterogeneous_property.hpp>
#include <dracosha/validator/utils/heterogeneous_size.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//---------------------------------------------------------------

template <typename IndexT>
struct foreach_if_impl
{
    template <typename T, typename HandlerT, typename PredicateT>
    static auto each(const T& obj, const PredicateT& pred, const HandlerT& fn)
    {
        auto index=IndexT{};
        auto&& val=get_heterogeneous_property(obj,index);
        auto res=fn(val,index);
        if (!pred(res))
        {
            return res;
        }
        auto next_index=hana::plus(index,hana::size_c<1>);
        return hana::eval_if(
            hana::greater_equal(next_index,heterogeneous_size(obj)),
            [&](auto&& _)
            {
                return _(res);
            },
            [&](auto&& _)
            {
                auto index=_(next_index);
                return foreach_if_impl<decltype(index)>::each(_(obj),_(pred),_(fn));
            }
        );
    }
};

struct foreach_if_t
{
    template <typename T, typename PredicateT, typename InitT, typename HandlerT>
    auto operator () (const T& obj, const PredicateT& pred, InitT&& init, const HandlerT& fn) const
    {
        return hana::eval_if(
            is_heterogeneous_container(obj),
            [&](auto&& _)
            {
                return foreach_if_impl<hana::size_t<0>>::each(_(obj),_(pred),_(fn));
            },
            [&](auto&& _)
            {
                return _(init);
            }
        );
    }

    template <typename T, typename PredicateT, typename HandlerT>
    auto operator () (const T& obj, const PredicateT& pred, const HandlerT& fn) const
    {
        return (*this)(obj,pred,false,fn);
    }
};
constexpr foreach_if_t foreach_if{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FOREACH_IF_HPP
