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
#include <dracosha/validator/reporting/translate.hpp>
#include <dracosha/validator/reporting/member_name.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
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
 * @brief Default helper when traits can not be used.
 *
 * Default helper converts ID to string, then translates it and after all decorates the result.
 */
template <typename T, typename TraitsT, typename =hana::when<true>>
struct single_member_name_t
{
    auto operator() (const T& id, const TraitsT& traits) const -> decltype(auto)
    {
        return decorate(traits,translate(traits,detail::to_string(id)));
    }
};

/**
 * @brief Helper when traits can be used.
 *
 * ID is forwarded to traits, then the result goes to decorator.
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,hana::when<can_single_member_name<T,TraitsT>::value>>
{
    auto operator() (const T& id, const TraitsT& traits) const -> decltype(auto)
    {
        return decorate(traits,translate(traits,traits(id)));
    }
};

/**
 * @brief Helper when traits can not be used and id is of inegral type.
 *
 * Integral ID is interpreted as index and formatted like "element #id", then it is translated and after all the result is decorated.
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,
                                hana::when<
                                    !can_single_member_name<T,TraitsT>::value
                                    && std::is_integral<std::decay_t<T>>::value
                                >
                            >
{
    auto operator() (const T& id, const TraitsT& traits) const
    {
        std::string dst;
        backend_formatter.append(dst,translate(traits,std::string(string_element)),id);
        return decorate(traits,dst);
    }
};

template <typename T, typename TraitsT>
constexpr single_member_name_t<T,TraitsT> single_member_name_inst{};

template <typename T, typename TraitsT>
constexpr auto single_member_name(const T& id, const TraitsT& traits) -> decltype(auto)
{
    return single_member_name_inst<T,TraitsT>(id,traits);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP
