/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/comparison.hpp
*
*  Defines comparison operators eq, neq, gt, lt, gte, lte
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_COMPARISON_HPP
#define DRACOSHA_VALIDATOR_COMPARISON_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/scalar_compare.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Operator "=="
 */
struct eq_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::equal(a,b);
    }
};
constexpr eq_t eq{};

/**
 * @brief Operator "!="
 */
struct ne_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !compare<T2,T1>::equal(a,b);
    }
};
constexpr ne_t ne{};

/**
 * @brief Operator "<"
 */
struct lt_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less(a,b);
    }
};
constexpr lt_t lt{};

/**
 * @brief Operator "<="
 */
struct lte_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less_equal(a,b);
    }
};
constexpr lte_t lte{};

/**
 * @brief Operator ">"
 */
struct gt_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less(b,a);
    }
};
constexpr gt_t gt{};

/**
 * @brief Operator ">="
 */
struct gte_t : public op
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less_equal(b,a);
    }
};
constexpr gte_t gte{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_COMPARISON_HPP
