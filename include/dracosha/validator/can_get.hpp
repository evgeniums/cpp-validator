/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/can_get.hpp
*
*  Defines utilities to check if member can be obtained from the object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CAN_GET_HPP
#define DRACOSHA_VALIDATOR_CAN_GET_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/has_method.hpp>
#include <dracosha/validator/property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T1, typename T2, typename =hana::when<true>>
struct can_get_t
{
};

/**
 *  Member is assumed to be gettable if object has either at(key) method or brackets [key] operator
 *  and type of the key satisfies signature of the corresponding method/operator.
 */
template <typename T1, typename T2>
struct can_get_t<T1,T2,hana::when<!hana::is_a<property_tag,T2>>>
{
    constexpr bool operator () () const
    {
        return detail::has_at_c(hana::type_c<T1>,hana::type_c<T2>)
               ||
               detail::has_brackets_c(hana::type_c<T1>,hana::type_c<T2>);
    }

    constexpr static bool property()
    {
        return false;
    }

    constexpr static bool brackets()
    {
        return detail::has_brackets_c(hana::type_c<T1>,hana::type_c<T2>);
    }

    constexpr static bool at()
    {
        return detail::has_at_c(hana::type_c<T1>,hana::type_c<T2>);
    }
};

/**
 *  Member is assumed to be gettable if object has the property.
 */
template <typename T1, typename T2>
struct can_get_t<T1,T2,hana::when<hana::is_a<property_tag,T2>>>
{
    constexpr bool operator () () const
    {
        return has_property<T1,T2>();
    }

    constexpr static bool property()
    {
        return has_property<T1,T2>();
    }

    constexpr static bool brackets()
    {
        return false;
    }

    constexpr static bool at()
    {
        return false;
    }
};

/**
 * Traits to check if member can be got from object of type T1 using key of type T2.
 */
template <typename T1, typename T2>
constexpr can_get_t<T1,T2> can_get{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CAN_GET_HPP
