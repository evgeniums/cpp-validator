/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/aggregate_any.hpp
*
*  Defines ANY aggregation for validating if any element satisfies validation conditions.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATE_ANY_HPP
#define DRACOSHA_VALIDATOR_AGGREGATE_ANY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Aggregation operator for checking if at least one element of a container satisfies the condition.
 */
struct aggregate_any_t
{
    /**
     * @brief Execute validator on container.
     * @param a Container to validate or adapter.
     * @param op Validator to apply to container's elements.
     * @return True if at least one element of the container passed the validator.
     */
    template <typename T, typename OpT>
    constexpr bool operator ()(T&& a,OpT&& op) const
    {
        return dispatcher.validate_any(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
    }

    /**
     * @brief Execute validator on elements of object's member.
     * @param a Object to validate or adapter.
     * @param member Member to process with validator, assumed to be a container.
     * @param op Validator to apply to container's elements.
     * @return True if at least one element of the container passed the validator.
     */
    template <typename T, typename OpT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpT&& op) const
    {
        return dispatcher.validate_any(std::forward<decltype(a)>(a),std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
    }
};

/**
 * @brief Instance of aggregation operator for checking if at least one element of a container satisfies the condition.
 */
constexpr aggregate_any_t aggregate_any{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATE_ANY_HPP
