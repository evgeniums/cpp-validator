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

/**
 * @brief Check if traits have suitable operator ()(const T&) for name transforming
 */
template <typename TraitsT, typename T>
struct can_member_name
{
    constexpr static const bool value=hana::is_valid([](auto v) -> decltype(
                                                     (void)std::declval<TraitsT>()(hana::traits::declval(v))){}
                                                  )(hana::type_c<T>);
};

/**
 * @brief Default traits with operator returning empty string
 */
template <typename TraitsT, typename T, typename =hana::when<true>>
struct member_name_traits_t
{
    std::string operator() (const TraitsT&, const T&) const
    {
        return std::string();
    }
};

/**
 * @brief Traits when operator can be used and argument can be converted to string.
 *
 * If traits return the same value with the argument then empty string will be returned as a result.
 */
template <typename TraitsT, typename T>
struct member_name_traits_t<TraitsT,T,hana::when<can_member_name<TraitsT,T>::value
                                      && std::is_constructible<std::string,T>::value>>
{
    std::string operator() (const TraitsT& traits, const T& id) const
    {
        auto str=traits(id);
        if (str==std::string(id))
        {
            return std::string();
        }
        return str;
    }
};

/**
 * @brief Traits when operator can be used but the argument can not be converted to string.
 *
 */
template <typename TraitsT, typename T>
struct member_name_traits_t<TraitsT,T,hana::when<can_member_name<TraitsT,T>::value
                                     && !std::is_constructible<std::string,T>::value>>
{
    std::string operator() (const TraitsT& traits, const T& id) const
    {
        return traits(id);
    }
};

template <typename TraitsT, typename T>
constexpr member_name_traits_t<TraitsT,T> member_name_traits{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAMES_TRAITS_HPP
