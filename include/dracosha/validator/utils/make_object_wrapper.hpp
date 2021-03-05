/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/make_object_wrapper.hpp
*
*  Defines helpers for making object wrappers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAKE_OBJECT_WRAPPER_HPP
#define DRACOSHA_VALIDATOR_MAKE_OBJECT_WRAPPER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/extract_object_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T>
auto make_object_wrapper(T&& v)
{
    return object_wrapper<T>(std::forward<T>(v));
}

template <typename T>
auto make_object_wrapper_ref(T&& v) -> decltype(auto)
{
    return hana::eval_if(
        hana::is_a<object_wrapper_tag,T>,
        [&](auto&& _)
        {
            const auto& val=extract_object_wrapper(_(v));
            return object_wrapper<decltype(val)>(val);
        },
        [&](auto&& _) -> decltype(auto)
        {
            return hana::id(_(v));
        }
    );
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAKE_OBJECT_WRAPPER_HPP