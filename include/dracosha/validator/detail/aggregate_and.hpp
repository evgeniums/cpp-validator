/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/aggregate_and.hpp
*
*  Defines aggregation of intermediate validators or validation operators using logical AND.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATE_AND_HPP
#define DRACOSHA_VALIDATOR_AGGREGATE_AND_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Aggregation of intermediate validators using logical AND.
 */
struct aggregate_and_t
{
    /**
     * @brief Execute validators on object and aggregate their results using logical AND.
     * @param a Object to validate or adapter.
     * @param ops List of intermediate validators or validation operators.
     * @return Logical AND of results of intermediate validators.
     */
    template <typename T, typename OpsT>
    constexpr bool operator ()(T&& a,OpsT&& ops) const
    {
        return dispatcher.validate_and(std::forward<decltype(a)>(a),std::forward<decltype(ops)>(ops));
    }

    /**
     * @brief Execute validators on object's member and aggregate their results using logical AND.
     * @param a Object to validate or adapter.
     * @param member Member to process with validators.
     * @param ops List of intermediate validators or validation operators.
     * @return Logical AND of results of intermediate validators.
     */
    template <typename T, typename OpsT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpsT&& ops) const
    {
        return dispatcher.validate_and(std::forward<decltype(a)>(a),std::forward<decltype(member)>(member),std::forward<decltype(ops)>(ops));
    }
};
/**
  @brief Instance of aggregation of intermediate validators using logical AND.
  */
constexpr aggregate_and_t aggregate_and{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATE_AND_HPP
