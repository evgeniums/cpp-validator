/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/unwrap_object.hpp
*
*  Defines helpers for extracting object from object wrapper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_UNWRAP_OBJECT_HPP
#define DRACOSHA_VALIDATOR_UNWRAP_OBJECT_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct object_wrapper_tag;

template <typename T, typename=hana::when<true>>
struct unwrapped_object_type
{
    using type=std::decay_t<T>;
};
template <typename T>
struct unwrapped_object_type<T,
            hana::when<hana::is_a<object_wrapper_tag,T>
            >
        >
{
    using type=std::decay_t<typename std::decay_t<T>::type>;
};
template <typename T>
using unwrap_object_t=typename unwrapped_object_type<T>::type;

struct unwrap_object_impl
{
    template <typename T>
    constexpr auto operator () (T&& val) const noexcept -> decltype(auto)
    {
        return hana::if_(
            hana::or_(
                        hana::is_a<object_wrapper_tag,T>
                       ),
            [](auto&& val) -> decltype(auto)
            {
                return val.get();
            },
            [](auto&& val) -> decltype(auto)
            {
                return hana::id(std::forward<decltype(val)>(val));
            }
        )(std::forward<T>(val));
    }
};
constexpr unwrap_object_impl unwrap_object{};

struct unwrap_object_type_c_t
{
    template <typename T>
    constexpr auto operator ()(T) const
    {
        return hana::type<unwrap_object_t<T>>{};
    }
};
constexpr unwrap_object_type_c_t unwrap_object_type_c{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_UNWRAP_OBJECT_HPP
