/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/member_property.hpp
*
* Defines pair of a member with the member's property.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_MEMBER_PROPERTY_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct member_property_tag;

/**
 * @brief Pair of a member and the member's property.
 */
template <typename MemberT, typename PropT>
struct member_property
{
    using hana_tag=member_property_tag;

    using member_type=MemberT;
    using property_type=PropT;

    MemberT member;
    PropT property;
};

/**
  @brief Make pair of a member and the member's property.
  @param b ID to wrap into member operand.
  @return Pair of a member and the member's property.
  */
template <typename MemberT, typename PropT>
auto make_member_property(MemberT&& member, PropT&& prop)
{
    return member_property<MemberT,PropT>{std::forward<MemberT>(member),std::forward<PropT>(prop)};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_PROPERTY_HPP
