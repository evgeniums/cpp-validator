/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/member_path.hpp
*
*  Defines helper for extracting member path.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_PATH_HPP
#define DRACOSHA_VALIDATOR_MEMBER_PATH_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct member_tag;

/**
* @brief Extract path from a member.
* @param member Member.
* @return Member's path.
*/
struct member_path_t
{
    template <typename MemberT>
    auto operator () (const MemberT& member) const -> decltype(auto)
    {
        return hana::eval_if(
            hana::is_a<member_tag,MemberT>,
            [&](auto&& _) -> decltype(auto)
            {
                return _(member).path();
            },
            [&](auto&& _)
            {
                return hana::make_tuple(_(member));
            }
        );
    }
};
constexpr member_path_t member_path{};

struct path_of_impl
{
    template <typename ArgT>
    auto operator () (ArgT&& arg) const -> decltype(auto)
    {
        return hana::if_(
            hana::is_a<hana::tuple_tag,ArgT>,
            [](auto&& path) -> decltype(auto)
            {
                // arg is a path (aka hana::tuple)
                return hana::id(std::forward<decltype(path)>(path));
            },
            [](auto&& member) -> decltype(auto)
            {
                // arg is a member
                return member.path();
            }
        )(std::forward<ArgT>(arg));
    }
};
constexpr path_of_impl path_of{};

struct member_path_list_t
{
    template <typename ...Args>
    constexpr auto operator () (Args&&... args) const
    {
        return hana::transform(hana::make_tuple(std::forward<Args>(args)...),member_path);
    }
};
constexpr member_path_list_t member_path_list{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_PATH_HPP
