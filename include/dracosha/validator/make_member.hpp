/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/make_member.hpp
*
*  Defines helpers for member creation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAKE_MEMBER_HPP
#define DRACOSHA_VALIDATOR_MAKE_MEMBER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/member_with_name.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Create plain member with one key in path.
 * @param key Member's key.
 * @return Created member.
 */
template <typename T>
constexpr auto make_plain_member(T&& key)
{
    using type=typename adjust_storable_type<T>::type;
    return member<type>{type(std::forward<T>(key))};
}

template <typename T>
constexpr auto make_typed_member(T&& key)
{
    return member<std::decay_t<T>>{std::forward<T>(key)};
}

/**
 * @brief Make member from path.
 * @param path Member's path.
 * @return Created member.
 */
template <typename Ts>
auto make_member(Ts&& path)
{
    static_assert(std::tuple_size<decltype(hana_to_std_tuple(path))>::value!=0,"Path can not be empty");

    return hana::fold(
        hana::drop_front(path),
        make_plain_member(std::move(hana::front(path))),
        [](auto&& parent_member, auto&& next_key)
        {
            return parent_member[std::forward<decltype(next_key)>(next_key)];
        }
    );
}

/**
 * @brief Make new member inherting given member.
 * @param path Path of new member.
 * @param member Member to inherit from.
 * @return New member.
 *
 * New member uses name() of original member if applicable.
 */
template <typename Ts, typename T>
auto inherit_member(Ts&& path, T&& member)
{
    return hana::eval_if(
        std::is_base_of<member_with_name_tag,std::decay_t<T>>{},
        [&](auto&& _)
        {
            return make_member_with_name(make_member(_(path)),_(member).name());
        },
        [&](auto&& _)
        {
            return make_member(_(path));
        }
    );
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAKE_MEMBER_HPP
