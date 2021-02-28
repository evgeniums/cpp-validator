/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/object_wrapper.hpp
*
*  Defines object wrapper to use in adapters.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP
#define DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct object_wrapper_tag;

/**
 * @brief Wrapper of object that can wrap either object or reference to the object.
 */
template <typename T>
class object_wrapper
{
    public:

        using hana_tag=object_wrapper_tag;
        using type=T;

        /**
         * @brief Constructor.
         * @param obj Object to wrap.
         */
        object_wrapper(
                T&& obj
            ) : _obj(std::forward<T>(obj))
        {}

        /**
         * @brief Get const reference to object.
         * @return Constant reference to wrapped object.
         */
        const std::remove_reference_t<T>& get() const noexcept
        {
            return _obj;
        }

        /**
         * @brief Get reference to object.
         * @return Reference to wrapped object.
         */
        std::remove_reference_t<T>& get() noexcept
        {
            return _obj;
        }

        /**
          Return embedded object "as is".
        **/
        auto id() noexcept -> decltype(auto)
        {
            return hana::id(_obj);
        }

        /**
         * @brief Get const reference to object.
         * @return Constant reference to wrapped object.
         */
        const std::remove_reference_t<T>& operator () () const noexcept
        {
            return _obj;
        }

        /**
         * @brief Get reference to object.
         * @return Reference to wrapped object.
         */
        std::remove_reference_t<T>& operator() () noexcept
        {
            return _obj;
        }

    private:

        T _obj;
};

template <typename T>
auto extract_object_wrapper(T&& val) -> decltype(auto)
{
    return hana::if_(
        hana::is_a<object_wrapper_tag,T>,
        [](auto&& val) -> decltype(auto)
        {
            return val.get();
        },
        [](auto&& val) -> decltype(auto)
        {
            return hana::id(std::forward<decltype(val)>(val));
        }
    )(std::forward<T>(val));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP
