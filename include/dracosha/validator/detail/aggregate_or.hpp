/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/aggregate_or.hpp
*
*  Defines aggregation of intermediate validators or validation operators using logical AND
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATE_OR_HPP
#define DRACOSHA_VALIDATOR_AGGREGATE_OR_HPP

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
struct aggregate_or_t
{
    /**
     * @brief Execute validators on object and aggregate their results using logical OR
     * @param a Object to validate
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename T, typename OpsT>
    constexpr bool operator ()(T&& a,OpsT&& ops) const
    {
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&a](bool prevResult, auto&& op)
                    {
                        if (prevResult)
                        {
                            return true;
                        }
                        return apply(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op));
                    }
                );
    }

    /**
     * @brief Execute validators on object's member and aggregate their results using logical OR
     * @param a Object to validate
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename T, typename OpsT, typename MemberT>
    constexpr bool operator () (T&& a,MemberT&& member,OpsT&& ops) const
    {
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&a,&member](bool prevResult, auto&& op)
                    {
                        if (prevResult)
                        {
                            return true;
                        }
                        return apply_member(std::forward<decltype(a)>(a),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
                    }
                );
    }
};
aggregate_or_t aggregate_or{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATE_OR_HPP
