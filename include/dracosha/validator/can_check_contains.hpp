/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/can_check_contains.hpp
*
*  Defines helper for checking if object can be queried if it contains a member.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CAN_CHECK_CONTAINS_HPP
#define DRACOSHA_VALIDATOR_CAN_CHECK_CONTAINS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/has_method.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>
#include <dracosha/validator/utils/pointer_as_reference.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct wrap_iterator_tag;

//-------------------------------------------------------------

namespace detail
{

template <typename T1, typename T2, typename = hana::when<true>>
struct can_check_at_or_brackets_t
{
    constexpr static const bool value = (detail::has_at_c(hana::type_c<T1>, hana::type_c<T2>)
                                ||
                                detail::has_brackets_c(hana::type_c<T1>, hana::type_c<T2>))
                                &&
                                detail::has_size_c(hana::type_c<T1>);
};

template <typename T1, typename T2>
struct can_check_at_or_brackets_t<T1,T2,
            hana::when<
                (hana::is_a<hana::tuple_tag,T1>
                ||
                 hana::is_a<hana::ext::std::tuple_tag,T1>
                 )
            >
        >
{
    constexpr static const bool value = false;
};

}

/**
 *  Helper for checking if object of type T1 can be queried if it contains a member accessible by key of type T2.
 */
template <typename T1, typename ArgT>
struct can_check_contains_t
{
    using T2=unwrap_object_t<ArgT>;
    constexpr static const bool value =
        hana::is_a<wrap_iterator_tag,T2>
        ||
        detail::has_find_it_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        detail::has_has_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        detail::has_contains_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        detail::has_isSet_c(hana::type_c<T1>, hana::type_c<T2>)
        ||
        (hana::is_a<property_tag, T2> && has_property<T1, T2>())
        ||
        detail::can_check_at_or_brackets_t<T1,T2>::value;

    constexpr bool operator () () const
    {
        return value;
    }
};

/**
 * @brief Callable for checking if object of type T1 can be queried if it contains a member accessible by key of type T2.
 */
template <typename T1, typename T2>
constexpr can_check_contains_t<decltype(as_reference(std::declval<T1>())),T2> can_check_contains{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CAN_CHECK_CONTAINS_HPP
