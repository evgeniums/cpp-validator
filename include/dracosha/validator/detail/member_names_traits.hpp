/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/member_names_traits.hpp
*
* Defines helper for traits of member names formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_NAMES_TRAITS_HPP
#define DRACOSHA_VALIDATOR_MEMBER_NAMES_TRAITS_HPP

#include <string>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename TraitsT, typename T, typename=void>
struct can_single_member_name
{
};
/**
 * @brief Check if traits have suitable operator ()(const T&) for name transforming
 */
template <typename TraitsT, typename T>
struct can_single_member_name<TraitsT,T,
            decltype((void)std::declval<TraitsT>()(std::declval<T>()))
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Default traits with operator returning empty string
 */
template <typename TraitsT, typename T, typename =hana::when<true>>
struct single_member_name_t
{
    std::string operator() (const TraitsT&, const T&) const
    {
        return std::string();
    }
};

/**
 * @brief Helper when operator can be used and argument can be converted to string.
 *
 * If traits return the same value with the argument then empty string will be returned as a result.
 */
template <typename TraitsT, typename T>
struct single_member_name_t<TraitsT,T,hana::when<can_single_member_name<TraitsT,T>::value
                                      && std::is_constructible<std::string,T>::value>>
{
    std::string operator() (const TraitsT& traits, const T& id) const
    {
        auto str=traits.single(id);
        if (str==std::string(id))
        {
            return std::string();
        }
        return str;
    }
};

/**
 * @brief Helper when operator can be used but the argument can not be converted to string.
 *
 */
template <typename TraitsT, typename T>
struct single_member_name_t<TraitsT,T,hana::when<can_single_member_name<TraitsT,T>::value
                                     && !std::is_constructible<std::string,T>::value>>
{
    std::string operator() (const TraitsT& traits, const T& id) const
    {
        return traits.single(id);
    }
};

template <typename TraitsT, typename T>
constexpr single_member_name_t<TraitsT,T> single_member_name{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAMES_TRAITS_HPP
