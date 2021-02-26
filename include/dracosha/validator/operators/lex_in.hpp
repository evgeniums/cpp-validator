/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/lex_in.hpp
*
*  Defines lexicographical lex_in/lex_nin operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_LEX_IN_HPP
#define DRACOSHA_VALIDATOR_LEX_IN_HPP

#include <algorithm>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/operators/lexicographical.hpp>
#include <dracosha/validator/operators/in.hpp>
#include <dracosha/validator/interval.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Definition of operator "lex_in" for checking if value is in interval/range using lexicographical comparison operators.
 */
struct lex_in_t : public op<lex_in_t>
{
    constexpr static const char* description=in_t::description;
    constexpr static const char* n_description=in_t::n_description;

    /**
     * @brief Call when operand is an interval.
     */
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b,
                               std::enable_if_t<hana::is_a<interval_tag,T2>,void*> =nullptr) const
    {
        if (b.mode==interval.closed())
        {
            return lex_gte(a,b.from) && lex_lte(a,b.to);
        }

        if (b.mode==interval.open())
        {
            return lex_gt(a,b.from) && lex_lt(a,b.to);
        }

        if (b.mode==interval.open_from())
        {
            return lex_gt(a,b.from) && lex_lte(a,b.to);
        }

        // open_to
        return lex_gte(a,b.from) && lex_lt(a,b.to);
    }

    /**
     * @brief Call when operand is a range.
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
                            return lex_eq(a,v);
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
        return std::binary_search(std::begin(container),std::end(container),a,lex_lt);
    }
};

/**
  @brief Operator for checking if value is in interval or range using lexicographical comparison operators
*/
constexpr lex_in_t lex_in{};

/**
 * @brief Definition of operator "lex_non" to check if value is not in interval/range using lexicographical comparison operators.
 */
struct lex_nin_t : public op<lex_nin_t>
{
    constexpr static const char* description=lex_in_t::n_description;
    constexpr static const char* n_description=lex_in_t::description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !lex_in(a,b);
    }
};

/**
  @brief Operator for checking if value is not in interval or range using lexicographical comparison operators
*/
constexpr lex_nin_t lex_nin{};

/**
 * @brief Definition of operator "ilex_in" for checking if value is in interval/range using case insensitive lexicographical comparison operators
 */
struct ilex_in_t : public op<ilex_in_t>
{
    constexpr static const char* description=in_t::description;
    constexpr static const char* n_description=in_t::n_description;

    /**
     * @brief Call when operand is an interval
     */
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b,
                               std::enable_if_t<hana::is_a<interval_tag,T2>,void*> =nullptr) const
    {
        if (b.mode==interval.closed())
        {
            return ilex_gte(a,b.from) && ilex_lte(a,b.to);
        }

        if (b.mode==interval.open())
        {
            return ilex_gt(a,b.from) && ilex_lt(a,b.to);
        }

        if (b.mode==interval.open_from())
        {
            return ilex_gt(a,b.from) && ilex_lte(a,b.to);
        }

        // open_to
        return ilex_gte(a,b.from) && ilex_lt(a,b.to);
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
                            return ilex_eq(a,v);
                         }
                         )!=std::end(container);
    }

    /**
     * @brief Call when operand is a sorted range
     */
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b,
                               std::enable_if_t<
                                 (hana::is_a<range_tag,T2> && T2::is_sorted::value),
                               void*> =nullptr
                            ) const
    {
        const auto& container=b.container;
        return std::binary_search(std::begin(container),std::end(container),a,ilex_lt);
    }
};

/**
  @brief Operator for checking if value is in interval or range using case insensitive lexicographical comparison operators.
*/
constexpr ilex_in_t ilex_in{};

/**
  @brief Definition of operator "ilex_nin" for checking if value is not in interval or range using case insensitive lexicographical comparison operators.
*/
struct ilex_nin_t : public op<ilex_nin_t>
{
    constexpr static const char* description=ilex_in_t::n_description;
    constexpr static const char* n_description=ilex_in_t::description;

    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return !ilex_in(a,b);
    }
};

/**
  @brief Operator for checking if value is not in interval or range using case insensitive lexicographical comparison operators.
*/
constexpr ilex_nin_t ilex_nin{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_LEX_IN_HPP
