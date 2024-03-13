/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/not.hpp
*
*  Defines logical pseudo operator NOT.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_NOT_HPP
#define HATN_VALIDATOR_NOT_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/make_validator.hpp>
#include <hatn/validator/aggregation/and.hpp>
#include <hatn/validator/detail/logical_not.hpp>
#include <hatn/validator/base_validator.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of logical pseudo operator NOT.
 * @param op Intermediate validator whose result must be negated.
 * @return Logical "not" of intermediate validator result.
 *
 */
struct not_t
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
                    make_aggregation_validator(
                        detail::logical_not,
                        std::forward<OpT>(op)
                    )
               );
    }
};
/**
  @brief Logical pseudo operator NOT
*/
constexpr not_t NOT{};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for operator NOT.
 */
struct string_not_t : public logical_op<string_not_t>
{
    constexpr static const aggregation_id id=aggregation_id::NOT;
    constexpr static const char* open_token="NOT ";
    constexpr static const char* close_token="";
    constexpr static const char* conjunction_token="";
};

/**
  @brief Instance of string description helper for operator NOT.
*/
constexpr string_not_t string_not{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_NOT_HPP
