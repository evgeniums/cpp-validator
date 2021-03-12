/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/copy.hpp
*
*  Defines helper for explicit variable copying.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_COPY_HPP
#define DRACOSHA_VALIDATOR_COPY_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct copy_impl
{
    template <typename T>
    T operator () (const T& v) const
    {
        return v;
    }
};
constexpr copy_impl copy{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_COPY_HPP
