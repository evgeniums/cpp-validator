/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/in.hpp
*
*  Defines in/nin operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERATOR_IN_HPP
#define DRACOSHA_VALIDATOR_OPERATOR_IN_HPP

#include <algorithm>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/operators/comparison.hpp>
#include <dracosha/validator/interval.hpp>
#include <dracosha/validator/range.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of operator "in" for checking if value is in interval/range.
 */
struct in_t : public op<in_t>
{
    constexpr static const char* description="must be in";
    constexpr static const char* n_description="must be not in";

    /**
     * @brief Call when operand is an interval.
     */
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

    /**
     * @brief Call when operand is a range
     */
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b,
                               std::enable_if_t<
                                 (hana::is_a<range_tag,T2> && !T2::is_sorted::value),
                               void*> =nullptr
                            ) const
    {
        const auto& container=b.container;
        return std::find_if(std::begin(container),std::end(container),
                         [&a](const auto& v)
                         {
                            return eq(a,v);
                         }
                         )!=std::end(container);
    }

    /**
     * @brief Call when operand is a sorted range.
     */
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b,
                               std::enable_if_t<
                                 (hana::is_a<range_tag,T2> && T2::is_sorted::value),
                               void*> =nullptr
                            ) const
    {
        const auto& container=b.container;
        return std::binary_search(std::begin(container),std::end(container),a,lt);
    }
};

/**
    @brief Operator for checking if value is in interval or range.
*/
constexpr in_t in{};

/**
 * @brief Definition of operator "nin" for checking if value is not in interval/range.
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

/**
    @brief Operator for checking if value is not in interval or range.
*/
constexpr nin_t nin{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERATOR_IN_HPP
