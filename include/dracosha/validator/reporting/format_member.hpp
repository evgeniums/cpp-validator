/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/format_member.hpp
*
*  Defines member formatter
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_MEMBER_HPP
#define DRACOSHA_VALIDATOR_FORMAT_MEMBER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//template <typename DstT, typename MemberT, typename MemberNamesT>
//void format_member(DstT& dst, MemberT&& member, MemberNamesT&& mn,
//                  std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr)
//{

//}

//template <typename DstT, typename MemberT, typename MemberNamesT>
//void format_member(DstT& dst, MemberT&& member, MemberNamesT&& mn,
//                  std::enable_if_t<!hana::is_a<member_tag,MemberT>,void*> =nullptr)
//{
//    backend_formatter.append(
//        dst,
//        apply_ref(std::forward<MemberNamesT>(mn),std::forward<MemberT>(member))
//    );
//}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_MEMBER_HPP
