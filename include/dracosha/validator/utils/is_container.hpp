/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/is_container.hpp
*
*  Defines helper to check of object is if container type.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_IS_CONTAINER_HPP
#define DRACOSHA_VALIDATOR_IS_CONTAINER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T, typename=void>
struct has_begin
{
    constexpr static const bool value=false;
};
template <typename T>
struct has_begin<T,
        decltype((void)std::declval<std::decay_t<T>>().begin())
        >
{
    constexpr static const bool value=true;
};

template <typename T, typename=void>
struct has_end
{
    constexpr static const bool value=false;
};
template <typename T>
struct has_end<T,
        decltype((void)std::declval<std::decay_t<T>>().end())>
{
    constexpr static const bool value=true;
};

template <typename T, typename =hana::when<true>>
struct is_container_t
{
    constexpr static const bool value=false;
};
template <typename T>
struct is_container_t<T,
            hana::when<has_begin<T>::value && has_end<T>::value>
        >
{
    constexpr static const bool value=true;
};

template <typename T>
constexpr auto is_container(T&&)
{
    return is_container_t<T>::value;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_IS_CONTAINER_HPP
