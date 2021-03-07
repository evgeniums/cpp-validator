/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/is_pair.hpp
*
*  Defines helper to check if object is of std::pair type.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_IS_PAIR_HPP
#define DRACOSHA_VALIDATOR_IS_PAIR_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail {

/**
 * @brief Default helper to check if object has property "first".
 */
template <typename T, typename=void>
struct has_first
{
    constexpr static const bool value=false;
};

/**
 * @brief Helper to check if object has property "first" when it has.
 */
template <typename T>
struct has_first<T,
        decltype((void)std::declval<std::decay_t<T>>().first)
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Default helper to check if object has property "second".
 */
template <typename T, typename=void>
struct has_second
{
    constexpr static const bool value=false;
};

/**
 * @brief Helper to check if object has property "second" for the case when it has indeed.
 */
template <typename T>
struct has_second<T,
        decltype((void)std::declval<std::decay_t<T>>().second)>
{
    constexpr static const bool value=true;
};
}

/**
 * @brief Default helper to check if object is of std::pair type. i.e it has "first" and "second" properties.
 */
template <typename T, typename =hana::when<true>>
struct is_pair_t
{
    constexpr static const bool value=false;
};

/**
 * @brief Helper to check if object is of std::pair type. i.e it has "first" and "second" properties.
 */
template <typename T>
struct is_pair_t<T,
            hana::when<detail::has_first<T>::value && detail::has_second<T>::value>
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Check if object is a std::pair.
 * @param v Variable to check.
 * @return true if the object has first and second member variables defined.
 */
template <typename T>
constexpr auto is_pair(T&& v)
{
    std::ignore=v;
    return is_pair_t<T>::value;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_IS_PAIR_HPP
