/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/is_container.hpp
*
*  Defines helper to check of object is if container type.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_IS_CONTAINER_HPP
#define DRACOSHA_VALIDATOR_IS_CONTAINER_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default helper for checking if type has begin() method.
 */
template <typename T, typename=void>
struct has_begin
{
    constexpr static const bool value=false;
};

/**
 * @brief Helper for checking if type has begin() method for case when it has.
 */
template <typename T>
struct has_begin<T,
        decltype((void)std::declval<std::decay_t<T>>().begin())
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Default helper for checking if type has end() method.
 */
template <typename T, typename=void>
struct has_end
{
    constexpr static const bool value=false;
};

/**
 * @brief Helper for checking if type has end() method for case when it has.
 */
template <typename T>
struct has_end<T,
        decltype((void)std::declval<std::decay_t<T>>().end())>
{
    constexpr static const bool value=true;
};

/**
 * @brief Default helper for checking if variable is a container.
 */
template <typename T, typename =hana::when<true>>
struct is_container_t
{
    constexpr static const bool value=false;
};

/**
 * @brief Helper for checking if variable is a container for case when type has begin() and end() methods.
 */
template <typename T>
struct is_container_t<T,
            hana::when<
                has_begin<T>::value && has_end<T>::value
                && !std::is_same<std::decay_t<T>,std::string>::value
                && !std::is_same<std::decay_t<T>,string_view>::value
            >
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Check if variable is a container.
 * @param v Variable to check.
 * @return True if variable has begin() and end() methods.
 */
template <typename T>
constexpr auto is_container(T&& v)
{
    std::ignore=v;
    return is_container_t<T>::value;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_IS_CONTAINER_HPP
