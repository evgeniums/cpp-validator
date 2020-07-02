/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/check_member.hpp
*
*  Defines helper to check if object can be queried if it contains a member and deduce type of that member
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_MEMBER_HPP
#define DRACOSHA_VALIDATOR_CHECK_MEMBER_HPP

#include <string>
#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/can_check_contains.hpp>
#include <dracosha/validator/can_get.hpp>
#include <dracosha/validator/detail/get_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Helper to check if object can be queried if it contains a member and deduce type of that member
 */
template <typename T1, typename T2, typename=hana::when<true>>
struct check_member_t
{
};
template <typename T1, typename T2>
struct check_member_t<T1,T2,hana::when<can_check_contains_t<T1,T2>::value
    && (detail::get_helpers::selector<T1, T2>::value == detail::get_helpers::getter::property)>>
{
    using type=typename std::decay<decltype(property(std::declval<T1>(),std::declval<T2>()))>::type;
};
template <typename T1, typename T2>
struct check_member_t<T1,T2,hana::when<can_check_contains_t<T1, T2>::value
    && (detail::get_helpers::selector<T1, T2>::value == detail::get_helpers::getter::at)>>
{
    using type=typename std::decay<decltype(std::declval<T1>().at(std::declval<T2>()))>::type;
};
template <typename T1, typename T2>
struct check_member_t<T1,T2,hana::when<can_check_contains_t<T1, T2>::value
    && (detail::get_helpers::selector<T1, T2>::value == detail::get_helpers::getter::brackets)>>
{
    using type=typename std::decay<decltype(std::declval<T1>()[std::declval<T2>()])>::type;
};

constexpr hana::metafunction_t<check_member_t> check_member{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_MEMBER_HPP
