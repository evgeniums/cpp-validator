/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/or.hpp
*
*  Defines logical pseudo operator OR.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_OR_HPP
#define HATN_VALIDATOR_OR_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/make_validator.hpp>
#include <hatn/validator/detail/aggregate_or.hpp>
#include <hatn/validator/aggregation/aggregation.hpp>
#include <hatn/validator/base_validator.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Logical pseudo operator OR.
 * @param xs Intermediate validators whose result must be forwarded to logical OR.
 * @return Logical "or" of intermediate validator results.
 *
 * Can be used both in functioin call notation OR(...) and in infix notation (... ^OR^ ...).
 */
HATN_VALIDATOR_INLINE_LAMBDA auto OR=hana::infix([](auto&& ...xs) -> decltype(auto)
{
    return make_validator(
                make_aggregation_validator(
                    detail::aggregate_or,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

template <typename HandlerT, typename ExistsOperatorT>
template <typename T>
auto validator_t<HandlerT,ExistsOperatorT>::operator || (T&& v)
{
    return OR(std::move(*this),std::forward<T>(v));
}

template <typename ValidatorT, typename HintT>
template <typename T>
auto validator_with_hint_t<ValidatorT,HintT>::operator || (T&& v)
{
    return OR(std::move(*this),std::forward<T>(v));
}

template <typename MemberT, typename ValidatorT, typename ExistsOperatorT>
template <typename T>
auto validator_with_member_t<MemberT,ValidatorT,ExistsOperatorT>::operator || (T&& v)
{
    return OR(std::move(*this),std::forward<T>(v));
}

template <typename HandlerT, typename WithCheckExistsT, typename ExistsOperatorT>
template <typename T>
auto base_validator<HandlerT,WithCheckExistsT,ExistsOperatorT>::operator || (T&& v)
{
    return OR(std::move(*this),std::forward<T>(v));
}

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator OR.
 */
struct string_or_t : public logical_op<string_or_t>
{
    constexpr static const aggregation_id id=aggregation_id::OR;
    constexpr static const char* open_token="(";
    constexpr static const char* close_token=")";
    constexpr static const char* conjunction_token=" OR ";
};

/**
  @brief Instance of string description helper for operator OR.
*/
constexpr string_or_t string_or{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_OR_HPP
