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
        using type=typename adjust_storable_type<T>::type;
        return hana::if_(
            hana::and_(
                hana::not_(std::is_same<std::decay_t<T>,std::string>{}),
                std::is_same<type,std::string>{}
            ),
            [](auto&& v)
            {
                // special case for const char* that should be explicitly converted to string
                return variadic_arg<std::string>(std::string(std::forward<decltype(v)>(v)));
            },
            [](auto&& v)
            {
                return variadic_arg<T>(std::forward<decltype(v)>(v));
            }
        )(std::forward<T>(v));
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
