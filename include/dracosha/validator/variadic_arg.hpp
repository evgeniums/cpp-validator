/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/variadic_arg.hpp
*
* Defines helpers to work with arguments of variadic properties.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VARIADIC_ARG_HPP
#define DRACOSHA_VALIDATOR_VARIADIC_ARG_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct variadic_arg_tag{};

template <typename T>
struct variadic_arg : public object_wrapper<T>,
                      public variadic_arg_tag
{
    using object_wrapper<T>::object_wrapper;
};

struct variadic_arg_aggregation_tag{};

template <typename AggregationT, typename MaxArgT>
struct variadic_arg_aggregation_impl
{
    template <typename T>
    auto end(T&& v) const -> decltype(auto)
    {
        auto self=this;
        return hana::eval_if(
                    hana::is_a<property_tag,MaxArgT>,
                    [&](auto&& _)
                    {
                        return property(_(v),_(self)->max_arg);
                    },
                    [&](auto&& _)
                    {
                        return _(self)->max_arg;
                    }
               );
    }

    template <typename T>
    auto begin(T&& v) const -> decltype(auto)
    {
        // currently begin is always 0
        // cast it to type of end
        return static_cast<std::decay_t<decltype(end(std::forward<T>(v)))>>(0);
    }

    template <typename T, typename IndexT>
    void next(T&&, IndexT& index) const
    {
        // currently just increment index
        ++index;
    }

    template <typename T, typename IndexT>
    bool while_cond(T&& v, const IndexT& index) const
    {
        return index<end(std::forward<T>(v));
    }

    AggregationT aggregation;
    MaxArgT max_arg;
};

template <typename AggregationT, typename MaxArgT>
struct variadic_arg_aggregation : public variadic_arg<variadic_arg_aggregation_impl<AggregationT,MaxArgT>>,
                                  public variadic_arg_aggregation_tag
{
    using aggregation_type=std::decay_t<AggregationT>;
    using variadic_arg<variadic_arg_aggregation_impl<AggregationT,MaxArgT>>::variadic_arg;
};

struct varg_t
{
    template <typename T>
    auto operator() (T&& v) const
    {
        using type=typename adjust_storable_type<T>::type;
        return hana::if_(
            hana::and_(
                hana::not_(std::is_same<std::decay_t<T>,std::string>{}),
                std::is_same<type,std::string>{}
            ),
            [](auto&& v)
            {
                return variadic_arg<std::string>(std::string(std::forward<decltype(v)>(v)));
            },
            [](auto&& v)
            {
                return variadic_arg<T>(std::forward<decltype(v)>(v));
            }
        )(std::forward<T>(v));
    }

    template <typename AggregationT, typename MaxT>
    auto operator() (AggregationT aggr, MaxT max_val) const
    {
        auto val=variadic_arg_aggregation_impl<AggregationT,MaxT>{std::move(aggr),std::move(max_val)};
        return variadic_arg_aggregation<AggregationT,MaxT>{std::move(val)};
    }
};
constexpr varg_t varg{};

struct is_varg_t
{
    template <typename T>
    constexpr auto operator() (T&&) const
    {
        return std::is_base_of<variadic_arg_tag,std::decay_t<T>>{};
    }

    template <typename FoldStateT, typename T>
    constexpr auto operator() (FoldStateT&& prev, T&&) const
    {
        using type=typename std::decay_t<T>::type;
        return hana::if_(
            prev,
            prev,
            hana::bool_
            <
                std::is_base_of<variadic_arg_tag,type>::value
            >{}
        );
    }
};
constexpr is_varg_t is_varg{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HPP
