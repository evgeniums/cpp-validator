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

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  Definition of stub object with comparison operator that returns true when other object is empty.
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
                        return false;
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
    friend bool operator == (const T& other, const T& v) noexcept
    {
        return v==other;
    }
    template <typename T>
    friend bool operator != (const T& other, const T& v) noexcept
    {
        return v!=other;
    }
    template <typename T>
    friend bool operator < (const T& other, const T& v) noexcept
    {
        return v>other;
    }
    template <typename T>
    friend bool operator <= (const T& other, const T& v) noexcept
    {
        return v>=other;
    }
    template <typename T>
    friend bool operator > (const T& other, const T& v) noexcept
    {
        return v<other;
    }
    template <typename T>
    friend bool operator >= (const T& other, const T& v) noexcept
    {
        return v<=other;
    }
};

/**
  Stub object with comparison operator that returns true when other object is empty.
 **/
constexpr true_if_empty_t true_if_empty{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRUE_IF_EMPTY_HPP
