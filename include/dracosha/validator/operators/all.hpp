/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/all.hpp
*
*  Defines aggregation pseudo operator ALL.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ALL_HPP
#define DRACOSHA_VALIDATOR_ALL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/operators/and.hpp>
#include <dracosha/validator/detail/aggregate_all.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of aggregation pseudo operator ALL for checking if all elements of a container satisfy condition.
 * @param op Validator to apply to each element of the container.
 * @return Success if all elements of the container passed validator.
 */
struct all_t
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
                        detail::aggregate_all,
                        std::forward<OpT>(op)
                    )
               );
    }
};
/**
  @brief Aggregation operator ALL that requires for all container elements to satisfy a condition.
*/
constexpr all_t ALL{};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator ALL.
 */
struct string_all_t : public aggregate_op<string_all_t>,
                      public enable_to_string<string_all_t>
{
    constexpr static const aggregation_id id=aggregation_id::ALL;
    constexpr static const char* open_token="";
    constexpr static const char* close_token="";
    constexpr static const char* conjunction_token="";

    constexpr static const char* description="each element";
};

/**
  @brief Instance of string description helper for operator ALL.
*/
constexpr string_all_t string_all{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ALL_HPP
