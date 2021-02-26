/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/logical_not.hpp
*
*  Defines negation of intermediate validator or validation operators using logical NOT.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_LOGICAL_NOT_HPP
#define DRACOSHA_VALIDATOR_LOGICAL_NOT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Helper for negation of intermediate validators using logical NOT.
 */
struct logical_not_t
{
    /**
     * @brief Execute validator on object and negate its results using logical NOT.
     * @param a Object to validate or adapter.
     * @param op Intermediate validator or validation operator.
     * @return Logical NOT of result of intermediate validator.
     */
    template <typename T, typename OpT>
    constexpr bool operator ()(T&& a,OpT&& op) const
    {
        return dispatcher.validate_not(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
    }

    /**
     * @brief Execute validator on object's member and negate its result using logical NOT.
     * @param a Object to validate or adapter.
     * @param member Member to process with validator.
     * @param op Intermediate validator or validation operator.
     * @return Logical NOT of result of intermediate validator.
     */
    template <typename T, typename OpT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpT&& op) const
    {
        return dispatcher.validate_not(std::forward<decltype(a)>(a),std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
    }
};

/**
 * @brief Instance of callable helper for negation of intermediate validators using logical NOT.
 */
constexpr logical_not_t logical_not{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_LOGICAL_NOT_HPP
