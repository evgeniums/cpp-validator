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

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct variadic_arg_tag{};

template <typename T>
struct variadic_arg : public object_wrapper<T>,
                      public variadic_arg_tag
{
    using object_wrapper<T>::object_wrapper;
};
struct varg_t
{
    template <typename T>
    auto operator() (T&& v) const
    {
        return variadic_arg<T>(std::forward<T>(v));
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
