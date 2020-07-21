/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/object_wrapper.hpp
*
*  Defines object wrapper to use in adapters.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP
#define DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP

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
 * @brief Wrapper of object, can wrap either object or reference to the object
 */
template <typename T>
class object_wrapper
{
    public:

        /**
         * @brief Constructor
         * @param obj Object to wrap
         */
        object_wrapper(
                T&& obj
            ) : _obj(std::forward<T>(obj))
        {}

        /**
         * @brief Get const reference to object
         * @return Constant reference to wrapped object
         */
        const std::decay_t<T>& get() const
        {
            return _obj;
        }

        /**
         * @brief Get reference to object
         * @return Reference to wrapped object
         */
        std::decay_t<T>& get()
        {
            return _obj;
        }

    private:

        T _obj;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP