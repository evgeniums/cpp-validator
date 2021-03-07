/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/comparison.hpp
*
*  Defines comparison operators eq, neq, gt, lt, gte, lte
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_COMPARISON_HPP
#define DRACOSHA_VALIDATOR_COMPARISON_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/safe_compare.hpp>
#include <dracosha/validator/operators/operator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of operator "equal to".
 */
struct eq_t : public op<eq_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return safe_compare_equal(a,b);
    }

    constexpr static const char* description="must be equal to";
    constexpr static const char* n_description="must be not equal to";
};
/**
  @brief Operator "equal to".
*/
constexpr eq_t eq{};

/**
 * @brief Definition of operator "not equal to".
 */
struct ne_t : public op<ne_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return safe_compare_not_equal(a,b);
    }

    constexpr static const char* description="must be not equal to";
    constexpr static const char* n_description="must be equal to";
};
/**
  @brief Operator "not equal to".
*/
constexpr ne_t ne{};

/**
 * @brief Definition of operator "less than".
 */
struct lt_t : public op<lt_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return safe_compare_less(a,b);
    }

    constexpr static const char* description="must be less than";
    constexpr static const char* n_description="must be greater than or equal to";
};
/**
  @brief Operator "less than".
*/
constexpr lt_t lt{};

/**
 * @brief Definition of operator "less than or equal to".
 */
struct lte_t : public op<lte_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return safe_compare_less_equal(a,b);
    }

    constexpr static const char* description="must be less than or equal to";
    constexpr static const char* n_description="must be greater than";
};
/**
  @brief Operator "less than or equal to".
*/
constexpr lte_t lte{};

/**
 * @brief Definition of operator "greater than".
 */
struct gt_t : public op<gt_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        auto ok=safe_compare_greater(a,b);
        return ok;
    }

    constexpr static const char* description="must be greater than";
    constexpr static const char* n_description="must be less than or equal to";
};
/**
  @brief Operator "greater than".
*/
constexpr gt_t gt{};

/**
 * @brief Definition of operator "greater than or equal to"
 */
struct gte_t : public op<gte_t>
{
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return safe_compare_greater_equal(a,b);
    }

    constexpr static const char* description="must be greater than or equal to";
    constexpr static const char* n_description="must be less than";
};
/**
  @brief Operator "greater than or equal to".
*/
constexpr gte_t gte{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_COMPARISON_HPP
