/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/operator.hpp
*
*  Defines helpers to apply validating.
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
  @brief Apply validating to object.
  @param a Object to validate.
  @param v Either validation operator or compound validator.
  @return Validation result.
  */
struct apply_t
{
    template <typename Ta, typename Tv>
    constexpr auto operator () (Ta&& a, Tv&& v) const -> decltype(auto)
    {
        return hana::if_(hana::is_a<validator_tag,decltype(v)>,
          [&a](auto&& x) -> decltype(auto) { return x.apply(a); },
          [&a](auto&& x) -> decltype(auto) { return x(a); }
        )(std::forward<decltype(v)>(v));
    }
};
/**
  @brief Callable for applying validation to object.
  */
constexpr apply_t apply{};

/**
  @brief Apply validating to object's member.
  @param a Object to validate.
  @param v Either validation operator or compound validator.
  @param member Object's member to validate.
  @return Validation result.
  */
struct apply_member_t
{
    template <typename Ta, typename Tv, typename Tm>
    constexpr auto operator () (Ta&& a, Tv&& v, Tm&& member) const -> decltype(auto)
    {
        return hana::if_(hana::is_a<validator_tag,decltype(v)>,
          [&a,&member](auto&& x) -> decltype(auto) { return x.apply(a,member); },
          [&a,&member](auto&& x) -> decltype(auto) { return x(a,member); }
        )(std::forward<decltype(v)>(v));
    }
};
/**
  @brief Callable for applying validation to object's member.
  */
constexpr apply_member_t apply_member{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_HPP
