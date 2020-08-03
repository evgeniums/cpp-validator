/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/lexicographical.hpp
*
*  Defines lexigrophical operators
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

/**
 * @brief Operator "lexicoraphical equal"
 */
struct lex_eq_t : public op<lex_eq_t>
{
    constexpr static const char* description=eq_t::description;
    constexpr static const char* n_description=eq_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return boost::algorithm::equals(a,b);
    }
};
constexpr lex_eq_t lex_eq{};

/**
 * @brief Operator "lexicoraphical not equal"
 */
struct lex_ne_t : public op<lex_ne_t>
{
    constexpr static const char* description=ne_t::description;
    constexpr static const char* n_description=ne_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !boost::algorithm::equals(a,b);
    }
};
constexpr lex_ne_t lex_ne{};

/**
 * @brief Operator "lexicoraphical less than"
 */
struct lex_lt_t : public op<lex_lt_t>
{
    constexpr static const char* description=lt_t::description;
    constexpr static const char* n_description=lt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return boost::algorithm::lexicographical_compare(a,b);
    }
};
constexpr lex_lt_t lex_lt{};

/**
 * @brief Operator "lexicoraphical less than or equal"
 */
struct lex_lte_t : public op<lex_lte_t>
{
    constexpr static const char* description=lte_t::description;
    constexpr static const char* n_description=lte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return boost::algorithm::lexicographical_compare(a,b)
                ||
               boost::algorithm::equals(a,b);
    }
};
constexpr lex_lte_t lex_lte{};

/**
 * @brief Operator "lexicoraphical greater than"
 */
struct lex_gt_t : public op<lex_gt_t>
{
    constexpr static const char* description=gt_t::description;
    constexpr static const char* n_description=gt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !lex_lte(a,b);
    }
};
constexpr lex_gt_t lex_gt{};

/**
 * @brief Operator "lexicoraphical greater than or equal"
 */
struct lex_gte_t : public op<lex_gte_t>
{
    constexpr static const char* description=gte_t::description;
    constexpr static const char* n_description=gte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !lex_lt(a,b);
    }
};
constexpr lex_gte_t lex_gte{};

/**
 * @brief Operator "lexicoraphical case insensitive equal"
 */
struct ilex_eq_t : public op<ilex_eq_t>
{
    constexpr static const char* description=eq_t::description;
    constexpr static const char* n_description=eq_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return boost::algorithm::iequals(a,b);
    }
};
constexpr ilex_eq_t ilex_eq{};

/**
 * @brief Operator "lexicoraphical case insensitive not equal"
 */
struct ilex_ne_t : public op<ilex_ne_t>
{
    constexpr static const char* description=ne_t::description;
    constexpr static const char* n_description=ne_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !boost::algorithm::iequals(a,b);
    }
};
constexpr ilex_ne_t ilex_ne{};

/**
 * @brief Operator "lexicoraphical case insensitive less than"
 */
struct ilex_lt_t : public op<ilex_lt_t>
{
    constexpr static const char* description=lt_t::description;
    constexpr static const char* n_description=lt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return boost::algorithm::ilexicographical_compare(a,b);
    }
};
constexpr ilex_lt_t ilex_lt{};

/**
 * @brief Operator "lexicoraphical case insensitive less than or equal"
 */
struct ilex_lte_t : public op<ilex_lte_t>
{
    constexpr static const char* description=lte_t::description;
    constexpr static const char* n_description=lte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return boost::algorithm::ilexicographical_compare(a,b)
                ||
               boost::algorithm::iequals(a,b);
    }
};
constexpr ilex_lte_t ilex_lte{};

/**
 * @brief Operator "lexicoraphical case insensitive greater than"
 */
struct ilex_gt_t : public op<ilex_gt_t>
{
    constexpr static const char* description=gt_t::description;
    constexpr static const char* n_description=gt_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !ilex_lte(a,b);
    }
};
constexpr ilex_gt_t ilex_gt{};

/**
 * @brief Operator "lexicoraphical case insensitive greater than or equal"
 */
struct ilex_gte_t : public op<ilex_gte_t>
{
    constexpr static const char* description=gte_t::description;
    constexpr static const char* n_description=gte_t::n_description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !ilex_lt(a,b);
    }
};
constexpr ilex_gte_t ilex_gte{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_LEXICOGRAPHICAL_HPP
