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
#include <dracosha/validator/utils/safe_compare.hpp>
#include <dracosha/validator/utils/string_view.hpp>
#include <dracosha/validator/utils/adjust_storable_ignore.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Tag for object_wrapper types.
 */
struct object_wrapper_tag
{
    template <typename T>
    friend bool operator == (const T&, const object_wrapper_tag&) noexcept
    {
        return true;
    }
    template <typename T>
    friend bool operator != (const T&, const object_wrapper_tag&) noexcept
    {
        return false;
    }

    template <typename T>
    bool operator == (const T&) noexcept
    {
        return true;
    }
    template <typename T>
    bool operator != (const T&) noexcept
    {
        return false;
    }
};

struct object_wrapper_base{};

/**
 * @brief Wrapper of object that can wrap either object or reference to the object.
 */
template <typename T>
class object_wrapper : public adjust_view_ignore,
                       public object_wrapper_base,
                       public adjust_storable_ignore
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

        T value() const noexcept
        {
            return _obj;
        }
        T value() noexcept
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
        operator const std::remove_reference_t<T>& () const noexcept
        {
            return _obj;
        }

        /**
         * @brief Get reference to object.
         * @return Reference to wrapped object.
         */
        operator std::remove_reference_t<T>& () noexcept
        {
            return _obj;
        }

        bool operator == (const object_wrapper& other) const noexcept
        {
            return other._obj==_obj;
        }
        bool operator != (const object_wrapper& other) const noexcept
        {
            return other._obj!=_obj;
        }

        template <typename T1>
        bool operator == (const T1& other) const noexcept
        {
            return safe_compare_equal(other,_obj);
        }
        template <typename T1>
        bool operator != (const T1& other) const noexcept
        {
            return safe_compare_not_equal(other,_obj);
        }
        template <typename T1>
        friend bool operator == (const T1& a, const object_wrapper<T>& b) noexcept
        {
            return hana::eval_if(
                hana::is_a<object_wrapper_tag,T1>,
                [&](auto&& _)
                {
                    return safe_compare_equal(_(a).get(),_(b).get());
                },
                [&](auto&& _)
                {
                    return safe_compare_equal(_(a),_(b).get());
                }
            );
        }
        template <typename T1>
        friend bool operator != (const T1& a, const object_wrapper<T>& b) noexcept
        {
            return hana::eval_if(
                hana::is_a<object_wrapper_tag,T1>,
                [&](auto&& _)
                {
                    return safe_compare_not_equal(_(a).get(),_(b).get());
                },
                [&](auto&& _)
                {
                    return safe_compare_not_equal(_(a),_(b).get());
                }
            );
        }

    protected:

        T _obj;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP
