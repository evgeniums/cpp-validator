/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prepend_super_member.hpp
*
*  Defines prepend_super_member.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREPEND_SUPER_MEMBER_HPP
#define DRACOSHA_VALIDATOR_PREPEND_SUPER_MEMBER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Prepend path of super member to path of current member.
 * @param super Super member.
 * @param member Current member.
 * @return Member with concatenated paths. If any of members have explicit names then new member is of member_with_name_list type.
 *
 * This helper is used with nested member validators.
 */
template <typename SuperMemberT, typename MemberT>
auto prepend_super_member(SuperMemberT&& super, MemberT&& member);

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREPEND_SUPER_MEMBER_HPP
