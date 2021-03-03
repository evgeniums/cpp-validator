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

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

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

/**
 * @brief Wrapper of object that can wrap either object or reference to the object.
 */
template <typename T>
class object_wrapper : public adjust_view_ignore
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

    private:

        T _obj;
};

template <typename T, typename=hana::when<true>>
struct extract_object_wrapper_t
{
    using type=std::decay_t<T>;
};
template <typename T>
struct extract_object_wrapper_t<T,
            hana::when<hana::is_a<object_wrapper_tag,T>>
        >
{
    using type=std::decay_t<typename std::decay_t<T>::type>;
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

template <typename T>
auto object_wrapper_value(T&& val) -> decltype(auto)
{
    return hana::if_(
        hana::is_a<object_wrapper_tag,T>,
        [](auto&& val) -> decltype(auto)
        {
            return val.value();
        },
        [](auto&& val) -> decltype(auto)
        {
            return hana::id(std::forward<decltype(val)>(val));
        }
    )(std::forward<T>(val));
}

struct extract_object_wrapper_type_c_t
{
    template <typename T>
    auto operator ()(T&& v) const
    {
        return hana::make_type(std::decay_t<decltype(extract_object_wrapper(v))>(v));
    }
};
constexpr extract_object_wrapper_type_c_t extract_object_wrapper_type_c{};

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

#endif // DRACOSHA_VALIDATOR_OBJECT_WRAPPER_HPP
