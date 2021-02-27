/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/strict_any.hpp
*
*  Defines wrapper of strict_any to use in prevalidation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRICT_ANY_HPP
#define DRACOSHA_VALIDATOR_STRICT_ANY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/string_view.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct strict_any_tag;

template <typename T>
struct strict_any_wrapper : public object_wrapper<T>
{
    using hana_tag=strict_any_tag;

    using object_wrapper<T>::object_wrapper;
};

struct strict_any_t
{
    template <typename T>
    auto operator() (T&& obj) const -> decltype(auto)
    {
        using value_type=decltype(adjust_view_type(std::forward<T>(obj)));
        return strict_any_wrapper<value_type>(adjust_view_type(std::forward<T>(obj)));
    }
};
constexpr strict_any_t strict_any{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
