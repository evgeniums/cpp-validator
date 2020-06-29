/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/operator.hpp
*
*  Defines helpers to apply validating
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_APPLY_HPP
#define DRACOSHA_VALIDATOR_APPLY_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct validator_tag;

/**
  @brief Apply validating to object
  @param a Object to validate
  @param v Either validation operator or compound validator
  @return Validation result
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto apply = [](auto&& a,auto&& v) -> decltype(auto)
{
    return hana::if_(hana::is_a<validator_tag,decltype(v)>,
      [&a](auto&& x) -> decltype(auto) { return x.apply(a); },
      [&a](auto&& x) -> decltype(auto) { return x(a); }
    )(std::forward<decltype(v)>(v));
};

/**
  @brief Apply validating to object's member
  @param a Object to validate
  @param v Either validation operator or compound validator
  @param member Object's member to validate
  @return Validation result
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto apply_member = [](auto&& a,auto&& v,auto&& member) -> decltype(auto)
{
    return hana::if_(hana::is_a<validator_tag,decltype(v)>,
      [&a,&member](auto&& x) -> decltype(auto) { return x.apply(a,member); },
      [&a,&member](auto&& x) -> decltype(auto) { return x(a,member); }
    )(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_HPP
