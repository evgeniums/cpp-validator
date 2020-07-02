/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/strings_order.hpp
*
*  Defines helper that adjusts strings ordering.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRINGS_ORDER_HPP
#define DRACOSHA_VALIDATOR_STRINGS_ORDER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/reorder_strings.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct strings_order_tag;

struct strings_order_t
{
    using hana_tag=strings_order_tag;

    template <typename ...Args>
    constexpr auto operator () (Args&&... args) const -> decltype(auto)
    {
        return detail::reorder_strings(std::forward<Args>(args)...);
    }
};
constexpr strings_order_t default_strings_order{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRINGS_ORDER_HPP
