/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/contains.hpp
*
*  Defines operator "contains".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CONTAINS_HPP
#define DRACOSHA_VALIDATOR_CONTAINS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/check_exists.hpp>
#include <dracosha/validator/member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of operator "contains" for checking if object contains a member.
 */
struct contains_t : public op<contains_t>
{
    constexpr static const char* description="must contain";
    constexpr static const char* n_description="must not contain";

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return check_contains(a,b);
    }
};

/**
  @brief Operator for checking if object contains a member.
*/
constexpr contains_t contains{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONTAINS_HPP
