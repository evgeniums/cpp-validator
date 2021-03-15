/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/operator.hpp
*
*  Defines helpers to apply validating.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_APPLY_HPP
#define DRACOSHA_VALIDATOR_APPLY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/filter_member.hpp>
#include <dracosha/validator/base_validator.hpp>

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
          [](auto&& a, auto&& x) -> decltype(auto) { return x.apply(std::forward<decltype(a)>(a)); },
          [](auto&& a, auto&& x) -> decltype(auto) { return x(std::forward<decltype(a)>(a)); }
        )(std::forward<decltype(a)>(a),std::forward<decltype(v)>(v));
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
        return hana::if_(
          hana::is_a<validator_tag,decltype(v)>,
          [&member](auto&& a, auto&& x) -> decltype(auto)
          {
            auto fn=[&x](auto&&... args) -> decltype(auto)
            {
                return x.apply(std::forward<decltype(args)>(args)...);
            };
            using x_type=std::decay_t<decltype(x)>;
            auto validator=base_validator<
                            decltype(fn),
                            typename x_type::with_check_exists,
                            std::decay_t<decltype(x.exists_operator)>
                        >{
                std::move(fn),
                x.check_exists_operand,
                x.exists_operator
            };
            return filter_member(validator,std::forward<decltype(a)>(a),std::forward<decltype(member)>(member));
          },
          [&member](auto&& a, auto&& x) -> decltype(auto)
          {
            return filter_member(std::forward<decltype(x)>(x),std::forward<decltype(a)>(a),std::forward<decltype(member)>(member));
          }
        )(std::forward<decltype(a)>(a),std::forward<decltype(v)>(v));
    }
};
/**
  @brief Callable for applying validation to object's member.
  */
constexpr apply_member_t apply_member{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_HPP
