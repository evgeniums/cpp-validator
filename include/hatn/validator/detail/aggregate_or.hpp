/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/aggregate_or.hpp
*
*  Defines aggregation of intermediate validators or validation operators using logical AND.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_AGGREGATE_OR_HPP
#define HATN_VALIDATOR_AGGREGATE_OR_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/dispatcher.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
/**
 * @brief Aggregation of intermediate validators using logical OR.
 */
struct aggregate_or_t
{
    /**
     * @brief Execute validators on object and aggregate their results using logical OR.
     * @param a Object to validate or adapter.
     * @param ops List of intermediate validators or validation operators.
     * @return Logical OR of results of intermediate validators.
     */
    template <typename T, typename OpsT>
    status operator ()(T&& a,OpsT&& ops) const
    {
        return dispatcher.validate_or(std::forward<decltype(a)>(a),std::forward<decltype(ops)>(ops));
    }

    /**
     * @brief Execute validators on object's member and aggregate their results using logical OR.
     * @param a Object to validate or adapter.
     * @param member Member to process with validators.
     * @param ops List of intermediate validators or validation operators.
     * @return Logical OR of results of intermediate validators.
     */
    template <typename T, typename OpsT, typename MemberT>
    status operator () (T&& a,MemberT&& member,OpsT&& ops) const
    {
        return dispatcher.validate_or(std::forward<decltype(a)>(a),std::forward<decltype(member)>(member),std::forward<decltype(ops)>(ops));
    }
};

/**
 * @brief Instance of aggregation of intermediate validators using logical OR.
 */
constexpr aggregate_or_t aggregate_or{};
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_AGGREGATE_OR_HPP
