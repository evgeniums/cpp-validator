/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/can_check_contains.hpp
*
*  Defines helper for checking if object can be queried if it contains a member
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CAN_CHECK_CONTAINS_HPP
#define DRACOSHA_VALIDATOR_CAN_CHECK_CONTAINS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/has_method.hpp>
#include <dracosha/validator/property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  Helper to check if object of type T1 can be queried if it contains a member accessible by key of type T2
 */
template <typename T1, typename T2>
struct can_check_contains_t
{
    constexpr static const bool value = detail::has_find_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        detail::has_has_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        detail::has_contains_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        detail::has_isSet_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        (hana::is_a<property_tag, T2> && has_property<T1, T2>());

    constexpr bool operator () () const
    {
        return value;
    }
};

template <typename T1, typename T2>
constexpr can_check_contains_t<T1,T2> can_check_contains{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CAN_CHECK_CONTAINS_HPP
