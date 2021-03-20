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

template <typename SuperMemberT, typename MemberT>
auto prepend_super_member(SuperMemberT&& super, MemberT&& member);

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREPEND_SUPER_MEMBER_HPP
