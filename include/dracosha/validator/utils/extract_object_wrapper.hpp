/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/extract_object_wrapper.hpp
*
*  Defines helpers for extracting object from object wrapper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EXTRACT_OBJECT_WRAPPER_HPP
#define DRACOSHA_VALIDATOR_EXTRACT_OBJECT_WRAPPER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct object_wrapper_tag;

template <typename T, typename=hana::when<true>>
struct extract_object_wrapper_t
{
    using type=std::decay_t<T>;
};
template <typename T>
struct extract_object_wrapper_t<T,
            hana::when<hana::is_a<object_wrapper_tag,T>
            >
        >
{
    using type=std::decay_t<typename std::decay_t<T>::type>;
};

struct extract_object_wrapper_impl
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
constexpr extract_object_wrapper_impl extract_object_wrapper{};

template <typename T>
constexpr auto object_wrapper_value(T&& val) -> decltype(auto)
{
    return hana::if_(
        hana::is_a<object_wrapper_tag,T>,
        [](auto&& val) -> decltype(auto)
        {
            return val.value();
        },
        [](auto&& val) -> decltype(auto)
        {
            return hana::id(std::forward<decltype(val)>(val));
        }
    )(std::forward<T>(val));
}

struct extract_object_wrapper_type_c_t
{
    template <typename T>
    constexpr auto operator ()(T) const
    {
        return hana::type<typename extract_object_wrapper_t<T>::type>{};
    }
};
constexpr extract_object_wrapper_type_c_t extract_object_wrapper_type_c{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EXTRACT_OBJECT_WRAPPER_HPP
