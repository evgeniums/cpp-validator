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
#include <dracosha/validator/utils/make_object_wrapper.hpp>
#include <dracosha/validator/utils/string_view.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

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

template <typename T>
struct strict_any_wrapper : public object_wrapper<T>
{
    using hana_tag=strict_any_tag;

    using object_wrapper<T>::object_wrapper;

    auto to_wrapper()
    {
        return make_object_wrapper(std::move(this->_obj));
    }
};

struct strict_any_t
{
    template <typename T>
    auto operator() (T&& obj) const -> decltype(auto)
    {
        using value_type=decltype(adjust_view_type(std::forward<T>(obj)));
        return strict_any_wrapper<value_type>(adjust_view_type(std::forward<T>(obj)));
    }
};
constexpr strict_any_t strict_any{};

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

template <typename T1, typename T2>
auto wrap_strict_any(T1&& val, T2&&) -> decltype(auto)
{
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
