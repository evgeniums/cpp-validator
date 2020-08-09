/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/any.hpp
*
*  Defines aggregation pseudo operator ANY.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ANY_HPP
#define DRACOSHA_VALIDATOR_ANY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/operators/and.hpp>
#include <dracosha/validator/detail/aggregate_any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of aggregation pseudo operator ANY to check if any element of a container satisfies condition.
 * @param op Validator to apply to elements of the container.
 * @return Success if any element of the container passed validator.
 */
struct any_t
{
    template <typename ... Ops>
    constexpr auto operator() (Ops&&... ops) const
    {
        return (*this)(AND(std::forward<Ops>(ops)...));
    }

    template <typename OpT>
    constexpr auto operator() (OpT&& op) const
    {
        return make_validator(
                    hana::reverse_partial(
                        detail::aggregate_any,
                        std::forward<OpT>(op)
                    )
               );
    }
};

/**
  @brief Aggregation operator ANY that requires for at least one of container elements to satisfy a condition.
*/
constexpr any_t ANY{};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator ANY.
 */
struct string_any_t : public aggregate_op<string_any_t>,
                      public enable_to_string<string_any_t>
{
    constexpr static const aggregation_id id=aggregation_id::ANY;
    constexpr static const char* open_token="";
    constexpr static const char* close_token="";
    constexpr static const char* conjunction_token="";

    constexpr static const char* description="at least one element";
};

/**
  @brief Instance of string description helper for operator ANY.
*/
constexpr string_any_t string_any{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ANY_HPP
