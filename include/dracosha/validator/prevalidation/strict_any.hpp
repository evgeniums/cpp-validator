/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/strict_any.hpp
*
*  Defines wrapper of strict_any to use in prevalidation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRICT_ANY_HPP
#define DRACOSHA_VALIDATOR_STRICT_ANY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/wrap_object.hpp>
#include <dracosha/validator/utils/string_view.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Tag for strict_any_wrapper.
 */
class strict_any_tag
{
    public:

        void set_strict_any(bool enable) noexcept
        {
            _strict_any=enable;
        }
        bool is_strict_any() const noexcept
        {
            return _strict_any;
        }

    private:

        bool _strict_any{false};
};

/**
 * @brief Wrapper used to wrap values for validation when ANY aggregation must be validated in strict manner.
 * Strict manner means that provided value is considered to be the full set of values among which ANY condition must be met.
 * Otherwise, when strict_any is not used then validator considers there might be other values in a set among which
 * ANY condition must be met so that failure to validate a single value does not mean failure of total validation,
 * it is just ignored.
 */
template <typename T>
struct strict_any_wrapper : public object_wrapper<T>
{
    using hana_tag=strict_any_tag;

    using object_wrapper<T>::object_wrapper;

    auto to_wrapper()
    {
        return wrap_object(std::move(this->_obj));
    }
};

/**
 * @brief Implementer of strict_any.
 */
struct strict_any_impl
{
    template <typename T>
    auto operator() (T&& obj) const -> decltype(auto)
    {
        using value_type=decltype(adjust_view_type(std::forward<T>(obj)));
        return strict_any_wrapper<value_type>(adjust_view_type(std::forward<T>(obj)));
    }
};
/**
 * @brief Wrap value for strict ANY validation.
 * @param obj Value to wrap.
 * @return Value wrapped into strict_any_wrapper.
 */
constexpr strict_any_impl strict_any{};

/**
 * @brief Extract value from strict_any_wrapper if applicable.
 * @param val Value to extract.
 * @return Original value.
 *
 * If argument is not a strict_any_wrapper then it is returned as is.
 */
template <typename T>
auto extract_strict_any(T&& val) -> decltype(auto)
{
    return hana::if_(
        hana::is_a<strict_any_tag,T>,
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

/**
 * @brief Wrap a value into strict_any_wrapper only if sample is a strict_any_wrapper.
 * @param val Value to wrap.
 * @param sample Sample object.
 * @return Wrapped value if sample is a strict any wrapper, otherwise the value as is.
 */
template <typename T1, typename T2>
auto wrap_strict_any(T1&& val, T2&& sample) -> decltype(auto)
{
    std::ignore=sample;
    return hana::if_(
        hana::is_a<strict_any_tag,T2>,
        [](auto&& val)
        {
            return strict_any(val);
        },
        [](auto&& val) -> decltype(auto)
        {
            return hana::id(std::forward<decltype(val)>(val));
        }
    )(std::forward<T1>(val));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
