/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/true_if_empty.hpp
*
*  Defines "true_if_empty".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TRUE_IF_EMPTY_HPP
#define DRACOSHA_VALIDATOR_TRUE_IF_EMPTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/operators/lexicographical.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  Definition of stub object to validate clearing.
 **/
struct true_if_empty_t
{
    template <typename T>
    constexpr bool operator ==(const T& other) const noexcept
    {
        auto&& val=adjust_view_type(other);
        return hana::eval_if(
            hana::and_(
                        hana::bool_<has_property<decltype(val),decltype(empty)>()>{}
                      ),
            [&](auto&& _)
            {
                return _(val).empty();
            },
            [&](auto&& _)
            {
                return hana::eval_if(
                    hana::bool_<has_property<decltype(_(val)),decltype(size)>()>{},
                    [&](auto&& _)
                    {
                        return _(val).size()==0;
                    },
                    [&](auto&&)
                    {
                        // ignore check
                        return true;
                    }
                );
            }
        );
    }

    template <typename T>
    constexpr bool operator < (const T& other) const noexcept
    {
        return !(*this==other);
    }

    template <typename T>
    constexpr bool operator <= (const T&) const noexcept
    {
        return true;
    }

    template <typename T>
    constexpr bool operator > (const T& other) const noexcept
    {
        return !(*this<other);
    }

    template <typename T>
    constexpr bool operator >= (const T& other) const noexcept
    {
        return *this==other;
    }

    template <typename T>
    constexpr bool operator !=(const T& other) const noexcept
    {
        return !(*this==other);
    }

    template <typename T>
    friend bool operator == (const T& other, const true_if_empty_t& v) noexcept
    {
        return v==other;
    }
    template <typename T>
    friend bool operator != (const T& other, const true_if_empty_t& v) noexcept
    {
        return v!=other;
    }
    template <typename T>
    friend bool operator < (const T& other, const true_if_empty_t& v) noexcept
    {
        return v>other;
    }
    template <typename T>
    friend bool operator <= (const T& other, const true_if_empty_t& v) noexcept
    {
        return v>=other;
    }
    template <typename T>
    friend bool operator > (const T& other, const true_if_empty_t& v) noexcept
    {
        return v<other;
    }
    template <typename T>
    friend bool operator >= (const T& other, const true_if_empty_t& v) noexcept
    {
        return v<=other;
    }
};

/**
  Stub object to validate clearing.
 **/
constexpr true_if_empty_t true_if_empty{};

/**
 * Template specialization of lexicograpical operators for true_if_empty_t.
 */
template <typename T2>
struct lex_operators<true_if_empty_t,T2>
{
    using T1=true_if_empty_t;

    static bool eq(const T1& a, const T2& b)
    {
        return a==b;
    }

    static bool ne (const T1& a, const T2& b)
    {
        return a!=b;
    }

    static bool lt(const T1& a, const T2& b)
    {
        return a<b;
    }

    static bool lte(const T1& a, const T2& b)
    {
        return a<=b;
    }

    static bool gt(const T1& a, const T2& b)
    {
        return a>b;
    }

    static bool gte(const T1& a, const T2& b)
    {
        return a>=b;
    }

    static bool ieq(const T1& a, const T2& b)
    {
        return eq(a,b);
    }

    static bool ine(const T1& a, const T2& b)
    {
        return ne(a,b);
    }

    static bool ilt(const T1& a, const T2& b)
    {
        return lt(a,b);
    }

    static bool ilte(const T1& a, const T2& b)
    {
        return lte(a,b);
    }

    static bool igt(const T1& a, const T2& b)
    {
        return gt(a,b);
    }

    static bool igte(const T1& a, const T2& b)
    {
        return gte(a,b);
    }

    static bool contains(const T1&, const T2&)
    {
        return false;
    }

    static bool icontains(const T1&, const T2&)
    {
        return false;
    }

    static bool starts_with(const T1&, const T2&)
    {
        return false;
    }

    static bool istarts_with(const T1&, const T2&)
    {
        return false;
    }

    static bool ends_with(const T1&, const T2&)
    {
        return false;
    }

    static bool iends_with(const T1&, const T2&)
    {
        return false;
    }
};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRUE_IF_EMPTY_HPP
