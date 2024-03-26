/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/has_reset.hpp
*
*  Defines helper for checking if class has reset() method.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_HAS_RESET_HPP
#define HATN_VALIDATOR_HAS_RESET_HPP

#include <boost/hana/traits.hpp>

#include <hatn/validator/config.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct has_reset_t
{
    template <typename T>
    auto operator()(T arg) const
    {
        return hana::is_valid([](auto v) -> decltype((void)hana::traits::declval(v).reset()){})(arg);
    }
};

constexpr has_reset_t has_reset{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_HAS_RESET_HPP
