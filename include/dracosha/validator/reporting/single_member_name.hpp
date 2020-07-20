/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/single_member_name.hpp
*
* Defines helper to format single member names.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP
#define DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/member_name.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>
#include <dracosha/validator/detail/to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T, typename TraitsT, typename=void>
struct can_single_member_name
{
    constexpr static const bool value=false;
};
/**
 * @brief Check if traits have suitable operator ()(const T&) for name transforming
 */
template <typename T, typename TraitsT>
struct can_single_member_name<T,TraitsT,
            decltype((void)std::declval<TraitsT>()(std::declval<T>()))
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Default helper that forwards ID to strings and then decorates the result
 */
template <typename T, typename TraitsT, typename =hana::when<true>>
struct single_member_name_t
{
    template <typename StringsT>
    std::string operator() (const T& id, const TraitsT& traits, const StringsT& strings) const
    {
        return decorate<TraitsT,decltype(strings(id))>(traits,strings(id));
    }
};

/**
 * @brief Helper when traits can not be used and id is of inegral type.
 *
 * Integral ID is interpreted as index and formatted like "element #id".
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,
                                hana::when<!can_single_member_name<T,TraitsT>::value
                                            && std::is_integral<std::decay_t<T>>::value
                                           >
                            >
{
    template <typename StringsT>
    std::string operator() (const T& id, const TraitsT& traits, const StringsT& strings) const
    {
        std::string dst;
        backend_formatter.append(dst,strings(string_element),id);
        return decorate<TraitsT,decltype(dst)>(traits,dst);
    }
};

/**
 * @brief Helper when traits can be used and id can be converted to string.
 *
 * If the result of traits is either empty or the same as id then the id is forwarded to strings.
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,hana::when<can_single_member_name<T,TraitsT>::value
                                      && std::is_constructible<std::string,T>::value>>
{
    template <typename StringsT>
    std::string operator() (const T& id, const TraitsT& traits, const StringsT& strings) const
    {
        auto str=traits(id);
        if (!str.empty() && str!=std::string(id))
        {
            return decorate<TraitsT,decltype(str)>(traits,str);
        }
        return decorate<TraitsT,decltype(strings(id))>(traits,strings(id));
    }
};

/**
 * @brief Helper when traits can be used but the id can not be converted to string.
 *
 * If the result of traits is empty then the id is forwarded to strings.
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,hana::when<can_single_member_name<T,TraitsT>::value
                                     && !std::is_constructible<std::string,T>::value>>
{
    template <typename StringsT>
    std::string operator() (const T& id, const TraitsT& traits, const StringsT& strings) const
    {
        auto str=traits(id);
        if (!str.empty())
        {
            return decorate<TraitsT,decltype(str)>(traits,str);
        }
        return decorate<TraitsT,decltype(strings(id))>(traits,strings(id));
    }
};

template <typename T, typename TraitsT>
constexpr single_member_name_t<T,TraitsT> single_member_name{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP
