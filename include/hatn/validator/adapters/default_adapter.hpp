/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/default_adapter.hpp
*
*  Defines default adapter implementation.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_DEFAULT_ADAPTER_HPP
#define HATN_VALIDATOR_DEFAULT_ADAPTER_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/status.hpp>
#include <hatn/validator/utils/object_wrapper.hpp>
#include <hatn/validator/with_check_member_exists.hpp>
#include <hatn/validator/adapter.hpp>
#include <hatn/validator/adapters/impl/default_adapter_impl.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default adapter traits.
 */
template <typename T>
class default_adapter_traits :  public adapter_traits,
                                public with_check_member_exists<default_adapter_traits<T>>,
                                public default_adapter_impl
{
    public:

        using base_tag=adapter_traits;

        /**
         * @brief Constructor.
         * @param adpt Adapter the traits is used in.
         * @param obj Object to wrap into adapter.
         */
        default_adapter_traits(
                    T&& obj
                ) : with_check_member_exists<default_adapter_traits<T>>(*this),
                    _obj(std::forward<T>(obj))
        {}

        /**
         *  @brief Get wrapped object under validation.
         */
        auto get() const -> decltype(auto)
        {
            return _obj.get();
        }

        /**
         *  @brief Get wrapped object under validation.
         */
        auto get() -> decltype(auto)
        {
            return _obj.get();
        }

    private:

        object_wrapper<T> _obj;
};

/**
 * @brief Default adapter performs object validation using operators directly "as is".
 *
 * Adapter wraps the object under validation.
 *
 */
template <typename T>
using default_adapter = adapter<default_adapter_traits<T>>;

/**
  @brief Make default validation adapter wrapping the embedded object.
  @param v Object to wrap into adapter.
  @return Validation adapter.
  */
template <typename T>
auto make_default_adapter(T&& v)
{
    return default_adapter<T>(std::forward<T>(v));
}

/**
 * @brief Ensure that object is wrapped with validation adapter.
 * @param Object or validation adapter.
 * @return If input is an adapter then returns adapter "as is", otherwise returns default adapter wrapping input object.
 */
template <typename T>
auto ensure_adapter(T&& v) -> decltype(auto)
{
    return hana::if_(
        hana::is_a<adapter_tag,T>,
        [](auto&& v) -> decltype(auto)
        {
            return hana::id(std::forward<decltype(v)>(v));
        },
        [](auto&& v)
        {
            return make_default_adapter(std::forward<decltype(v)>(v));
        }
    )(std::forward<T>(v));
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_DEFAULT_ADAPTER_HPP
