/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prepend_super_member.ipp
*
*  Defines prepend_super_member.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_PREPEND_SUPER_MEMBER_IPP
#define HATN_VALIDATOR_PREPEND_SUPER_MEMBER_IPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/member_with_name_list.hpp>
#include <hatn/validator/prepend_super_member.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

template <typename SuperMemberT, typename MemberT>
auto prepend_super_member(SuperMemberT&& super, MemberT&& member)
{
    auto new_path=hana::concat(super.path(),member.path());

    return hana::eval_if(
        hana::or_(
            std::is_base_of<member_with_name_tag,std::decay_t<SuperMemberT>>{},
            std::is_base_of<member_with_name_tag,std::decay_t<MemberT>>{}
        ),
        [&](auto&& _)
        {
            return make_member_with_name_list(make_member(_(new_path)),
                                              hana::concat(member_name_list(_(super)),member_name_list(_(member)))
                                              );
        },
        [&](auto&& _)
        {
            return make_member(_(new_path));
        }
    );
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_PREPEND_SUPER_MEMBER_IPP
