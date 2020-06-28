/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/aggregate_and.hpp
*
*  Defines aggregation of intermediate validators or validation operators using logical AND
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATE_AND_HPP
#define DRACOSHA_VALIDATOR_AGGREGATE_AND_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/make_validator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Aggregation of intermediate validators using logical AND
 */
struct aggregate_and_t
{
    /**
     * @brief Execute validators on object and aggregate their results using logical AND
     * @param a Object to validate
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename T, typename OpsT>
    constexpr bool operator ()(T&& a,OpsT&& ops) const
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&a](bool prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return false;
                        }
                        return apply(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
                    }
                );
    }

    /**
     * @brief Execute validators on object's member and aggregate their results using logical AND
     * @param a Object to validate
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename T, typename OpsT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpsT&& ops) const
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&a,&member](bool prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return false;
                        }
                        return apply_member(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
                    }
                );
    }
};
aggregate_and_t aggregate_and{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATE_AND_HPP
