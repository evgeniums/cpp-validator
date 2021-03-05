/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/lexicographical.hpp
*
*  Defines lexigrophical operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_LEXICOGRAPHICAL_HPP
#define DRACOSHA_VALIDATOR_LEXICOGRAPHICAL_HPP

#include <boost/algorithm/string/predicate.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/comparison.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T1, typename T2, typename Enable=hana::when<true>>
struct lex_operators
{
    static bool eq(const T1& a, const T2& b)
    {
        return boost::algorithm::equals(a,b);
    }

    static bool ne (const T1& a, const T2& b)
    {
        return !boost::algorithm::equals(a,b);
    }

    static bool lt(const T1& a, const T2& b)
    {
        return boost::algorithm::lexicographical_compare(a,b);
    }

    static bool lte(const T1& a, const T2& b)
    {
        return boost::algorithm::lexicographical_compare(a,b)
                ||
               boost::algorithm::equals(a,b);
    }

    static bool gt(const T1& a, const T2& b)
    {
        return !lte(a,b);
    }

    static bool gte(const T1& a, const T2& b)
    {
        return !lt(a,b);
    }

    static bool ieq(const T1& a, const T2& b)
    {
        return boost::algorithm::iequals(a,b);
    }

    static bool ine(const T1& a, const T2& b)
    {
        return !boost::algorithm::iequals(a,b);
    }

    static bool ilt(const T1& a, const T2& b)
    {
        return boost::algorithm::ilexicographical_compare(a,b);
    }

    static bool ilte(const T1& a, const T2& b)
    {
        return boost::algorithm::ilexicographical_compare(a,b)
                ||
               boost::algorithm::iequals(a,b);
    }

    static bool igt(const T1& a, const T2& b)
    {
        return !ilte(a,b);
    }

    static bool igte(const T1& a, const T2& b)
    {
        return !ilt(a,b);
    }

    static bool contains(const T1& a, const T2& b)
    {
        return boost::algorithm::contains(a,b);
    }

    static bool icontains(const T1& a, const T2& b)
    {
        return boost::algorithm::icontains(a,b);
    }

    static bool starts_with(const T1& a, const T2& b)
    {
        return boost::algorithm::starts_with(a,b);
    }

    static bool istarts_with(const T1& a, const T2& b)
    {
        return boost::algorithm::istarts_with(a,b);
    }

    static bool ends_with(const T1& a, const T2& b)
    {
        return boost::algorithm::ends_with(a,b);
    }

    static bool iends_with(const T1& a, const T2& b)
    {
        return boost::algorithm::iends_with(a,b);
    }
};

/**
 * @brief Definition of operator "lexicographically equals to".
 */
struct lex_eq_t : public op<lex_eq_t>
{
    constexpr static const char* description=eq_t::description;
    constexpr static const char* n_description=eq_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::eq(a,b);
    }
};

/**
    @brief Operator "lexicographically equals to".
*/
constexpr lex_eq_t lex_eq{};

/**
 * @brief Definition of operator "lexicographically not equals to".
 */
struct lex_ne_t : public op<lex_ne_t>
{
    constexpr static const char* description=ne_t::description;
    constexpr static const char* n_description=ne_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::ne(a,b);
    }
};

/**
    @brief Operator "lexicographically not equals to".
*/
constexpr lex_ne_t lex_ne{};

/**
 * @brief Definition of operator "lexicographically less than".
 */
struct lex_lt_t : public op<lex_lt_t>
{
    constexpr static const char* description=lt_t::description;
    constexpr static const char* n_description=lt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::lt(a,b);
    }
};

/**
    @brief Operator "lexicographically less than".
*/
constexpr lex_lt_t lex_lt{};

/**
 * @brief Definition of operator "lexicographically less than or equals to".
 */
struct lex_lte_t : public op<lex_lte_t>
{
    constexpr static const char* description=lte_t::description;
    constexpr static const char* n_description=lte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::lte(a,b);
    }
};

/**
    @brief Operator "lexicographically less than or equals to".
*/
constexpr lex_lte_t lex_lte{};

/**
 * @brief Definition of operator "lexicographically greater than".
 */
struct lex_gt_t : public op<lex_gt_t>
{
    constexpr static const char* description=gt_t::description;
    constexpr static const char* n_description=gt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::gt(a,b);
    }
};

/**
    @brief Operator "lexicographically greater than".
*/
constexpr lex_gt_t lex_gt{};

/**
 * @brief Definition of operator "lexicographically greater than or equals to".
 */
struct lex_gte_t : public op<lex_gte_t>
{
    constexpr static const char* description=gte_t::description;
    constexpr static const char* n_description=gte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::gte(a,b);
    }
};

/**
 * @brief Operator "lexicographically greater than or equals to".
 */
constexpr lex_gte_t lex_gte{};

/**
 * @brief Definition of operator "lexicographically case insensitive equals to".
 */
struct ilex_eq_t : public op<ilex_eq_t>
{
    constexpr static const char* description=eq_t::description;
    constexpr static const char* n_description=eq_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::ieq(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive equals to".
 */
constexpr ilex_eq_t ilex_eq{};

/**
 * @brief Definition of operator "lexicographically case insensitive not equals to".
 */
struct ilex_ne_t : public op<ilex_ne_t>
{
    constexpr static const char* description=ne_t::description;
    constexpr static const char* n_description=ne_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::ine(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive not equals to".
 */
constexpr ilex_ne_t ilex_ne{};

/**
 * @brief Definition of operator "lexicographically case insensitive less than".
 */
struct ilex_lt_t : public op<ilex_lt_t>
{
    constexpr static const char* description=lt_t::description;
    constexpr static const char* n_description=lt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::ilt(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive less than".
 */
constexpr ilex_lt_t ilex_lt{};

/**
 * @brief Definition of operator "lexicographically case insensitive less than or equals to".
 */
struct ilex_lte_t : public op<ilex_lte_t>
{
    constexpr static const char* description=lte_t::description;
    constexpr static const char* n_description=lte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::ilte(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive less than or equals to".
 */
constexpr ilex_lte_t ilex_lte{};

/**
 * @brief Definition of operator "lexicographically case insensitive greater than".
 */
struct ilex_gt_t : public op<ilex_gt_t>
{
    constexpr static const char* description=gt_t::description;
    constexpr static const char* n_description=gt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::igt(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive greater than".
 */
constexpr ilex_gt_t ilex_gt{};

/**
 * @brief Definition of operator "lexicographically case insensitive greater than or equals to".
 */
struct ilex_gte_t : public op<ilex_gte_t>
{
    constexpr static const char* description=gte_t::description;
    constexpr static const char* n_description=gte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::igte(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive greater than or equals to".
 */
constexpr ilex_gte_t ilex_gte{};

/**
 * @brief Definition of operator "lexicographically contains".
 */
struct lex_contains_t : public op<lex_contains_t>
{
    constexpr static const char* description="must contain";
    constexpr static const char* n_description="must not contain";

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::contains(a,b);
    }
};

/**
 * @brief Operator "lexicographically contains".
 */
constexpr lex_contains_t lex_contains{};

/**
 * @brief Definition of operator "lexicographically case insensitive contains".
 */
struct ilex_contains_t : public op<ilex_contains_t>
{
    constexpr static const char* description=lex_contains_t::description;
    constexpr static const char* n_description=lex_contains_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::icontains(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive contains".
 */
constexpr ilex_contains_t ilex_contains{};

/**
 * @brief Definition of operator "lexicographically starts with".
 */
struct lex_starts_with_t : public op<lex_starts_with_t>
{
    constexpr static const char* description="must start with";
    constexpr static const char* n_description="must not start with";

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::starts_with(a,b);
    }
};

/**
 * @brief Operator "lexicographically starts with".
 */
constexpr lex_starts_with_t lex_starts_with{};

/**
 * @brief Definition of operator "lexicographically case insensitive starts with".
 */
struct ilex_starts_with_t : public op<ilex_starts_with_t>
{
    constexpr static const char* description=lex_starts_with_t::description;
    constexpr static const char* n_description=lex_starts_with_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::istarts_with(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive starts with".
 */
constexpr ilex_starts_with_t ilex_starts_with{};

/**
 * @brief Definition of operator "lexicographically ends with".
 */
struct lex_ends_with_t : public op<lex_ends_with_t>
{
    constexpr static const char* description="must end with";
    constexpr static const char* n_description="must not end with";

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::ends_with(a,b);
    }
};

/**
 * @brief Operator "lexicographically ends with".
 */
constexpr lex_ends_with_t lex_ends_with{};

/**
 * @brief Definition of operator "lexicographically case insensitive ends with".
 */
struct ilex_ends_with_t : public op<ilex_ends_with_t>
{
    constexpr static const char* description=lex_ends_with_t::description;
    constexpr static const char* n_description=lex_ends_with_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return lex_operators<T1,T2>::iends_with(a,b);
    }
};

/**
 * @brief Operator "lexicographically case insensitive ends with".
 */
constexpr ilex_ends_with_t ilex_ends_with{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_LEXICOGRAPHICAL_HPP
