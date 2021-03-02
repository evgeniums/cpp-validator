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

#ifndef DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/with_check_member_exists.hpp>
#include <dracosha/validator/check_member_exists_traits_proxy.hpp>
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/adapters/impl/default_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default adapter traits.
 */
template <typename T>
class default_adapter_traits :  public object_wrapper<T>,
                                public with_check_member_exists<adapter<default_adapter_traits<T>>>,
                                public default_adapter_impl
{
    public:

        /**
         * @brief Constructor.
         * @param adpt Adapter the traits is used in.
         * @param obj Object to wrap into adapter.
         */
        default_adapter_traits(
                    const adapter<default_adapter_traits<T>>& adpt,
                    T&& obj
                ) : object_wrapper<T>(std::forward<T>(obj)),
                    with_check_member_exists<adapter<default_adapter_traits<T>>>(adpt)
        {}
};

/**
 * @brief Default adapter performs object validation using operators directly "as is".
 *
 * Adapter wraps the object under validation.
 *
 */
template <typename T>
class default_adapter : public adapter<default_adapter_traits<T>>,
                        public check_member_exists_traits_proxy<default_adapter<T>>
{
    public:

        using type=T;

        /**
         * @brief Constructor.
         * @param obj COnstant reference to object under validation.
         */
        default_adapter(T&& obj)
            : adapter<default_adapter_traits<T>>(std::forward<T>(obj)),
              check_member_exists_traits_proxy<default_adapter<T>>(*this)
        {}
};

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

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_HPP
