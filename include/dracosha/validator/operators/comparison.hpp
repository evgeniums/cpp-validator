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
#include <dracosha/validator/utils/scalar_compare.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Operator "=="
 */
struct eq_t : public op<eq_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T1,T2>::equal(a,b);
    }

    constexpr static const char* description="is equal to";
};
constexpr eq_t eq{};

/**
 * @brief Operator "!="
 */
struct ne_t : public op<ne_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !compare<T1,T2>::equal(a,b);
    }

    constexpr static const char* description="is not equal to";
};
constexpr ne_t ne{};

/**
 * @brief Operator "<"
 */
struct lt_t : public op<lt_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T1,T2>::less(a,b);
    }

    constexpr static const char* description="is less than";
};
constexpr lt_t lt{};

/**
 * @brief Operator "<="
 */
struct lte_t : public op<lte_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T1,T2>::less_equal(a,b);
    }

    constexpr static const char* description="is less than or equal to";
};
constexpr lte_t lte{};

/**
 * @brief Operator ">"
 */
struct gt_t : public op<gt_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less(b,a);
    }

    constexpr static const char* description="is greater than";
};
constexpr gt_t gt{};

/**
 * @brief Operator ">="
 */
struct gte_t : public op<gte_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return compare<T2,T1>::less_equal(b,a);
    }

    constexpr static const char* description="is greater than or equal to";
};
constexpr gte_t gte{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_COMPARISON_HPP
