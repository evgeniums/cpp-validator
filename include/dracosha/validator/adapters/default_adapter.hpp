/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/default_adapter.hpp
*
*  Defines default adapter implementation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/with_check_member_exists.hpp>
#include <dracosha/validator/check_member_exists_traits_proxy.hpp>
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/detail/default_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default adapter traits
 */
template <typename T>
struct default_adapter_traits : public with_check_member_exists<adapter<default_adapter_traits<T>>>,
                                public reference_wrapper_t<const T>,
                                public detail::default_adapter_impl
{
    /**
     * @brief Constructor
     * @param adpt Adapter the traits is used in
     * @param obj Object to wrap into adapter
     */
    default_adapter_traits(
                const adapter<default_adapter_traits<T>>& adpt,
                const T& obj
            )
        : with_check_member_exists<adapter<default_adapter_traits<T>>>(adpt),
          reference_wrapper_t<const T>(obj)
    {}
};

/**
 * @brief Default adapter performs object validation using operators directly "as is".
 *
 * Adapter wraps the constant reference to the object under validation.
 *
 */
template <typename T>
class default_adapter : public adapter<default_adapter_traits<T>>,
                        public check_member_exists_traits_proxy<default_adapter<T>>
{
    public:

        /**
         * @brief Constructor
         * @param obj COnstant reference to object under validation
         */
        default_adapter(const T& obj)
            : adapter<default_adapter_traits<T>>(obj),
              check_member_exists_traits_proxy<default_adapter<T>>(*this)
        {}
};

/**
  @brief Make default validation adapter wrapping the embedded object
  @param v Object to wrap into adapter
  @return Validation adapter
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto make_default_adapter = [](auto&& v)
{
    return default_adapter<std::decay_t<decltype(v)>>(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_HPP
