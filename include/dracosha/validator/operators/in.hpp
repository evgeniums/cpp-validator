/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/range.hpp
*
*  Defines ranges operators
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_RANGE_HPP
#define DRACOSHA_VALIDATOR_RANGE_HPP

#include <algorithm>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/operators/comparison.hpp>
#include <dracosha/validator/interval.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Operator to check if value is in container
 */
struct in_t : public op<in_t>
{
    constexpr static const char* description="must be in";
    constexpr static const char* n_description="must be not in";

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b,
                               std::enable_if_t<hana::is_a<interval_tag,T2>,void*> =nullptr) const
    {
        if (b.mode==interval.closed())
        {
            return gte(a,b.from) && lte(a,b.to);
        }

        if (b.mode==interval.open())
        {
            return gt(a,b.from) && lt(a,b.to);
        }

        if (b.mode==interval.open_from())
        {
            return gt(a,b.from) && lte(a,b.to);
        }

        // open_to
        return gte(a,b.from) && lt(a,b.to);
    }

//    template <typename T1, typename T2>
//    constexpr bool operator() (const T1& a, const T2& b) const
//    {
//        return std::find(std::begin(b),std::end(b),a)!=std::end(b);
//    }
};
constexpr in_t in{};

/**
 * @brief Operator to check if value is not in container
 */
struct nin_t : public op<nin_t>
{
    constexpr static const char* description=in_t::n_description;
    constexpr static const char* n_description=in_t::description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !in(a,b);
    }
};
constexpr nin_t nin{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RANGE_HPP
