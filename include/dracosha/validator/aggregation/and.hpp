/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/and.hpp
*
*  Defines logical pseudo operator AND.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AND_HPP
#define DRACOSHA_VALIDATOR_AND_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/aggregation/aggregation.hpp>
#include <dracosha/validator/detail/aggregate_and.hpp>
#include <dracosha/validator/base_validator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Logical pseudo operator AND.
 * @param xs Intermediate validators whose result must be forwarded to logical AND.
 * @return Logical "and" of intermediate validator results.
 *
 * Can be used both as function call notation AND(...) and as infix notation (... ^AND^ ...).
 */
DRACOSHA_VALIDATOR_INLINE_LAMBDA auto AND=hana::infix([](auto&& ...xs) -> decltype(auto)
{
    return make_validator(
                make_aggregation_validator(
                    detail::aggregate_and,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

/**
 * @brief Helper to create logical pseudo operator AND on heap.
 * @param xs Intermediate validators whose result must be forwarded to logical AND.
 * @return Logical "and" of intermediate validator results.
 *
 */
template <typename ... Args>
auto AND_on_heap(Args&& ...xs) -> decltype(auto)
{
    return make_validator_on_heap(
                make_aggregation_validator(
                    detail::aggregate_and,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
}

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator AND.
 */
struct string_and_t : public logical_op<string_and_t>
{
    constexpr static const aggregation_id id=aggregation_id::AND;
    constexpr static const char* open_token="(";
    constexpr static const char* close_token=")";
    constexpr static const char* conjunction_token=" AND ";
};

/**
  @brief Instance of string description helper for operator AND.
*/
constexpr string_and_t string_and{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AND_HPP
