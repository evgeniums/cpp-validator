/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/member_helper.ipp
*
*  Defines helpers to invoke member methods.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_MEMBER_HELPER_IPP
#define HATN_VALIDATOR_MEMBER_HELPER_IPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/member.hpp>
#include <hatn/validator/member_with_name.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

namespace detail
{

template <typename T1, typename T2>
template <typename MemberT>
auto member_helper_2args_t<T1,T2,hana::when<std::is_enum<std::decay_t<T2>>::value>>::operator ()
        (
             MemberT&& member,
             T1&& name,
             T2&& grammar_category
         ) const
{
    return make_member_with_name(std::forward<MemberT>(member),concrete_phrase(std::forward<T1>(name),std::forward<T2>(grammar_category)));
}

template <typename ... Args>
template <typename MemberT>
auto member_helper_t<Args...>::operator ()
      (
           MemberT&& member,
           Args&&... args
       ) const
{
  return make_member_with_name(std::forward<MemberT>(member),concrete_phrase(std::forward<Args>(args)...));
}

}

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_MEMBER_HELPER_IPP
