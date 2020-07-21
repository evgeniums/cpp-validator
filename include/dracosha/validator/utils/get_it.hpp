/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/get_it.hpp
*
*  Defines helper for getting value from container's iterator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GET_IT_HPP
#define DRACOSHA_VALIDATOR_GET_IT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/is_pair.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

template <typename T, typename=hana::when<true>>
struct get_it_t
{
    template <typename T1>
    constexpr auto operator() (T1&& it) const -> decltype(auto)
    {
        return *it;
    }
};
template <typename T>
struct get_it_t<T,
            hana::when<is_pair_t<decltype(*std::declval<T>())>::value>
        >
{
    template <typename T1>
    constexpr auto operator() (T1&& it) const -> decltype(auto)
    {
        return it->second;
    }
};
template <typename T>
constexpr get_it_t<T> get_it_impl{};

template <typename T>
constexpr auto get_it(T&& v) -> decltype(auto)
{
    return get_it_impl<T>(std::forward<T>(v));
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_IT_HPP
