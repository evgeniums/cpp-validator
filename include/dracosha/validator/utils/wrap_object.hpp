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

/**
 * @brief Wrap a value into object_wrapper if it is not an object_wrapper already.
 * @param v Value.
 * @return object_wrapper if value is not an object_wrapper, otherwise value as is.
 */
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

//-------------------------------------------------------------
/**
 * @brief Wrap a reference to an object into object_wrapper if object is already wrapped by another object_wrapper.
 * @param v Value.
 * @return A wrapped reference if value is a object_wrapper, otherwise the value as is.
 *
 * This helper is used to avoid extra copies of object_wrapper embeddable in cases when object_wrapper wraps rvalue.
 * In this case the original object_wrapper with actual storable value will be transformed to object_wrapper
 * with constant reference to the value.
 */
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
