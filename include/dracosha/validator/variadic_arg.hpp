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
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/basic_property.hpp>
#include <dracosha/validator/variadic_arg_tag.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base template for variadic arguments.
 */
template <typename T>
struct variadic_arg : public object_wrapper<T>,
                      public variadic_arg_tag
{
    using object_wrapper<T>::object_wrapper;
};

/**
 * @brief Implementer of variadic argument with element aggregation.
 */
template <typename AggregationT, typename MaxArgT>
struct variadic_arg_aggregation_impl
{
    /**
     * @brief Get end "index" of variadic argument.
     * @param v Object/value variadic property is applied to.
     * @return End value of variadic "index".
     */
    template <typename T>
    auto end(T&& v) const -> decltype(auto)
    {
        auto&& arg=unwrap_object(max_arg);
        return hana::eval_if(
                    hana::is_a<property_tag,decltype(arg)>,
                    [&](auto&& _)
                    {
                        return property(_(v),_(arg));
                    },
                    [&](auto&& _)
                    {
                        return _(arg);
                    }
               );
    }

    /**
     * @brief Get begin "index" of variadic argument.
     * @param v Object/value variadic property is applied to.
     * @return Begin value of variadic "index".
     */
    template <typename T>
    auto begin(T&& v) const -> decltype(auto)
    {
        // currently begin is always 0
        // cast it to type of end
        return static_cast<std::decay_t<decltype(end(std::forward<T>(v)))>>(0);
    }

    /**
     * @brief Get next "index" of variadic argument.
     * @param v Object/value variadic property is applied to.
     * @param index Current index.
     * @return Next value of variadic "index".
     */
    template <typename T, typename IndexT>
    void next(T&&, IndexT& index) const
    {
        // currently just increment index
        ++index;
    }

    /**
     * @brief Check if "while" condition of variadic argument is satisfied.
     * @param v Object/value variadic property is applied to.
     * @param index Current index.
     * @return Boolean result.
     */
    template <typename T, typename IndexT>
    bool while_cond(T&& v, const IndexT& index) const
    {
        return index < end(std::forward<T>(v));
    }

    AggregationT aggregation;
    MaxArgT max_arg;
};

/**
 * @brief Variadic argument with element aggregation.
 */
template <typename AggregationT, typename MaxArgT>
struct variadic_arg_aggregation : public variadic_arg<variadic_arg_aggregation_impl<AggregationT,MaxArgT>>,
                                  public variadic_arg_aggregation_tag
{
    using aggregation_type=std::decay_t<AggregationT>;
    using variadic_arg<variadic_arg_aggregation_impl<AggregationT,MaxArgT>>::variadic_arg;
};

//-------------------------------------------------------------

/**
 * @brief Implementer of varg().
 */
struct varg_impl
{
    /**
     * @brief Create variadic argument from a variable.
     * @param v Variable.
     * @return Variadic argument.
     */
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

    /**
     * @brief Create variadic argument with element aggregation.
     * @param aggr Aggregation.
     * @param max_val Maximal value of variadic argument or property to figure out maximal value.
     * @return Variadic argument.
     */
    template <typename AggregationT, typename MaxT>
    auto operator() (AggregationT aggr, MaxT&& max_val) const
    {
        auto wrapped_max_val=object_wrapper<MaxT>{std::forward<MaxT>(max_val)};
        auto val=variadic_arg_aggregation_impl<AggregationT,decltype(wrapped_max_val)>{std::move(aggr),std::move(wrapped_max_val)};
        return variadic_arg_aggregation<AggregationT,decltype(wrapped_max_val)>{std::move(val)};
    }
};
/**
 * @brief Helper for creating variadic argument.
 */
constexpr varg_impl varg{};

//-------------------------------------------------------------

/**
 * @brief Implementer of is_varg().
 */
struct is_varg_impl
{
    /**
     * @brief Check if variable is a variadic argument.
     */
    template <typename T>
    constexpr auto operator() (T&&) const
    {
        return std::is_base_of<variadic_arg_tag,std::decay_t<T>>{};
    }

    /**
     * @brief Check if foldable contains at least one variadic argument.
     */
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
/**
 * @brief Helper to figure out if variable is a variadic argument or a foldable containing at least one variadic argument.
 */
constexpr is_varg_impl is_varg{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HPP
