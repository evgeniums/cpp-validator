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

#ifndef HATN_VALIDATOR_FOREACH_IF_HPP
#define HATN_VALIDATOR_FOREACH_IF_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/heterogeneous_property.hpp>
#include <hatn/validator/utils/heterogeneous_size.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//---------------------------------------------------------------

namespace detail {

/**
 * @brief Implementations of foreach_if().
 */
template <typename IndexT>
struct foreach_if_t
{
    template <typename T, typename HandlerT, typename PredicateT>
    static auto each(T&& obj, const PredicateT& pred, const HandlerT& fn)
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
                return foreach_if_t<decltype(index)>::each(_(obj),_(pred),_(fn));
            }
        );
    }
};
}

/**
 * @brief Implementer of foreach_if().
 */
struct foreach_if_impl
{
    /**
     * @brief Invoke a handler on each element of an object till predicate is satisfied, with initial value.
     * @param obj Object.
     * @param pred Predicate.
     * @param defaultRet Default value to return if object is not a heterogeneous container.
     * @param handler Handler to invoke.
     * @return Accumulated result of handler invocations.
     */
    template <typename T, typename PredicateT, typename DefaultT, typename HandlerT>
    auto operator () (T&& obj, const PredicateT& pred, DefaultT&& defaultRet, const HandlerT& handler) const
    {
        return hana::eval_if(
            is_heterogeneous_container(obj),
            [&](auto&& _)
            {
                return detail::foreach_if_t<hana::size_t<0>>::each(_(obj),_(pred),_(handler));
            },
            [&](auto&& _)
            {
                return _(defaultRet);
            }
        );
    }

    /**
     * @brief Invoke a handler on each element of an object till predicate is satisfied.
     * @param obj Object.
     * @param pred Predicate.
     * @param handler Handler to invoke.
     * @return Accumulated result of handler invocations or false if object is not a heterogeneous container.
     */
    template <typename T, typename PredicateT, typename HandlerT>
    auto operator () (T&& obj, const PredicateT& pred, const HandlerT& handler) const
    {
        return (*this)(obj,pred,false,handler);
    }
};
/**
 * @brief Invoke a handler on each element of a foldable container while predicate is satisfied.
 *
 * Three versions are available:
 *  - with init;
 *  - without init;
 *
 *  See comments to respective operators above.
 *
 * Handler signature must be: "RetT (auto&& element, auto&& element_index)".
 */
constexpr foreach_if_impl foreach_if{};

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_FOREACH_IF_HPP
