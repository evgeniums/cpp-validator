/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/get_it.hpp
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

/**
 * @brief Helper for getting value from container's iterator.
 */
template <typename T, typename=hana::when<true>>
struct get_it_t
{
     template <typename T1>
     constexpr auto operator() (T1&& it) const -> decltype(auto)
     {
        auto& val=*it;
        return val;
     }

     template <typename T1>
     constexpr static auto key(T1&& it) -> decltype(auto)
     {
        auto& val=*it;
        return val;
     }
};

/**
 * @brief Helper for getting value from container's iterator when iterator can be converted to std::pair.
 *
 * Used for std::map, etc.
 */
template <typename T>
struct get_it_t<T,
            hana::when<is_pair_t<decltype(*std::declval<T>())>::value>
        >
{
    template <typename T1>
    constexpr auto operator() (T1&& it) const -> decltype(auto)
    {
        auto& val=it->second;
        return val;
    }

    template <typename T1>
    constexpr static auto key(T1&& it) -> decltype(auto)
    {
       auto& val=it->first;
       return val;
    }
};
/**
 * @brief Instance of helper for getting value from container's iterator.
 */
template <typename T>
constexpr get_it_t<T> get_it_impl{};

/**
 * @brief Get value from iterator.
 * @param it Iterator to get value from.
 * @return Reference to iterator's value.
 */
template <typename T>
constexpr auto get_it(T&& it) -> decltype(auto)
{
    return get_it_impl<T>(std::forward<T>(it));
}

template <typename T>
constexpr auto key_it(T&& it) -> decltype(auto)
{
    return get_it_impl<T>.key(std::forward<T>(it));
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_IT_HPP
