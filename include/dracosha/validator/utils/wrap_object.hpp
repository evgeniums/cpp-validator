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

#ifndef DRACOSHA_VALIDATOR_WRAP_OBJECT_HPP
#define DRACOSHA_VALIDATOR_WRAP_OBJECT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/variadic_arg.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct wrap_object_impl
{
    template <typename T>
    auto operator () (T&& v) const -> decltype(auto)
    {
        return hana::eval_if(
            hana::is_a<object_wrapper_tag,T>,
            [&](auto&& _) -> decltype(auto)
            {
                return hana::id(_(v));
            },
            [&](auto&& _)
            {
                return object_wrapper<T>(std::forward<T>(_(v)));
            }
        );
    }
};
constexpr wrap_object_impl wrap_object{};

template <typename T>
auto wrap_object_ref(T&& v) -> decltype(auto)
{
    return hana::eval_if(
        hana::and_(
            hana::is_a<object_wrapper_tag,T>,
            hana::not_(std::is_base_of<variadic_arg_tag,std::decay_t<T>>{})
        ),
        [&](auto&& _)
        {
            const auto& val=unwrap_object(_(v));
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

#endif // DRACOSHA_VALIDATOR_WRAP_OBJECT_HPP
