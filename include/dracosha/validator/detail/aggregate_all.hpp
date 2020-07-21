/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/aggregate_all.hpp
*
*  Defines ALL aggregation for validating if all elements satisfy validation conditions
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATE_ALL_HPP
#define DRACOSHA_VALIDATOR_AGGREGATE_ALL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Aggregation operator to check if all elements of a container satisfy the condition.
 */
struct aggregate_all_t
{
    /**
     * @brief Execute validator on container
     * @param a Container to validate
     * @param op Validator to apply to container's elements
     * @return True if all elements of the container passed the validator
     */
    template <typename T, typename OpT>
    constexpr bool operator ()(T&& a,OpT&& op) const
    {
        return dispatcher.validate_all(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
    }

    /**
     * @brief Execute validator on elements of object's member
     * @param a Object to validate
     * @param member Member to process with validator, assumed to be a container
     * @param op Validator to apply to container's elements
     * @return True if all elements of the container passed the validator
     */
    template <typename T, typename OpT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpT&& op) const
    {
        return dispatcher.validate_all(std::forward<decltype(a)>(a),std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
    }
};
constexpr aggregate_all_t aggregate_all{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATE_ALL_HPP
