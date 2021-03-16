/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/can_get.hpp
*
*  Defines helpers for checking if a member can be obtained from the object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CAN_GET_HPP
#define DRACOSHA_VALIDATOR_CAN_GET_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/basic_property.hpp>
#include <dracosha/validator/detail/has_method.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct wrap_iterator_tag;

//-------------------------------------------------------------

/**
 *  @brief Default helper for checking if a member can be got from object by key.
 *
 *  Member is not gettable.
 */
template <typename T1, typename T2, typename =hana::when<true>>
struct can_get_t
{
};

/**
 *  @brief Helper for checking if a member can be got from object by key.
 *
 *  Member gettable if key is a wrapped iterator.
 */
template <typename T1, typename T2>
struct can_get_t<T1,T2,hana::when<hana::is_a<wrap_iterator_tag,T2>>>
{
    constexpr bool operator () () const
    {
        return true;
    }

    constexpr static bool iterator()
    {
        return true;
    }

    constexpr static bool property()
    {
        return false;
    }

    constexpr static bool brackets()
    {
        return false;
    }

    constexpr static bool at()
    {
        return false;
    }

    constexpr static bool find()
    {
        return false;
    }
};

/**
 *  @brief Helper to check if member can be got from object by key.
 *
 *  Member is gettable if object has either at(key) method or find(key) or brackets [key] operator
 *  and type of the key satisfies signature of the corresponding method/operator.
 */
template <typename T1, typename T2>
struct can_get_t<T1,T2,hana::when<
        (!hana::is_a<property_tag,T2>
        &&
        !hana::is_a<wrap_iterator_tag,T2>)
        >>
{
    constexpr bool operator () () const
    {
        return detail::has_at_c(hana::type_c<T1>,hana::type_c<T2>)
               ||
               detail::has_brackets_c(hana::type_c<T1>,hana::type_c<T2>)
               ||
               detail::has_find_it_c(hana::type_c<T1>,hana::type_c<T2>);
    }

    constexpr static bool iterator()
    {
        return false;
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

    constexpr static bool find()
    {
       return detail::has_find_it_c(hana::type_c<T1>,hana::type_c<T2>);
    }
};

/**
 *  @brief Helper to check if member can be got from object by key.
 *
 *  Member is gettable if object has the property.
 */
template <typename T1, typename T2>
struct can_get_t<T1,T2,hana::when<hana::is_a<property_tag,T2>>>
{
    constexpr bool operator () () const
    {
        return has_property<T1,T2>();
    }

    constexpr static bool iterator()
    {
        return false;
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

    constexpr static bool find()
    {
       return false;
    }
};

/**  
 * @brief Helper for checking if a member can be got from object of type T1 using key of type T2.
 */
template <typename T1, typename T2>
constexpr can_get_t<T1,T2> can_get{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CAN_GET_HPP
