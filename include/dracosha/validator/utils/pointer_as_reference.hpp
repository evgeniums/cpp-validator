/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/poniter_as_reference.hpp
*
*  Defines poniter_as_reference.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_POINTER_AS_REFERENCE_HPP
#define DRACOSHA_VALIDATOR_POINTER_AS_REFERENCE_HPP

#include <memory>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T, typename = hana::when<true>>
struct pointer_as_reference_t
{
    using is_pointer=hana::false_;

    template <typename T1>
    constexpr auto operator () (T1&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(v));
    }
};
template <typename T>
struct pointer_as_reference_t<T,
            hana::when<std::is_pointer<T>::value>
        >
{
    using is_pointer=hana::true_;

    template <typename T1>
    auto operator () (T1&& v) const -> decltype(auto)
    {
       return *v;
    }
};
template <typename T>
struct pointer_as_reference_t<std::shared_ptr<T>>
{
    using is_pointer=hana::true_;

    template <typename T1>
    auto operator () (T1&& v) const -> decltype(auto)
    {
       return *v;
    }
};
template <typename T>
constexpr pointer_as_reference_t<T> pointer_as_reference_inst{};

struct pointer_as_reference_impl
{
    template <typename T>
    constexpr auto operator () (T&& v) const -> decltype(auto)
    {
        return pointer_as_reference_inst<std::decay_t<T>>(std::forward<T>(v));
    }
};
constexpr pointer_as_reference_impl as_reference{};

struct is_pointer_impl
{
    template <typename T>
    constexpr auto operator () (T&&) const
    {
        using type=typename pointer_as_reference_t<std::decay_t<T>>::is_pointer;
        return type{};
    }
};
constexpr is_pointer_impl is_pointer{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_POINTER_AS_REFERENCE_HPP
