/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/pointer_as_reference.hpp
*
*  Defines helpers for pointer dereferencing.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_POINTER_AS_REFERENCE_HPP
#define DRACOSHA_VALIDATOR_POINTER_AS_REFERENCE_HPP

#include <memory>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper for dereferencing pointers if applicable.
 * By defaualt if value is not a pointer the value is used itself as is.
 */
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
/**
 * Dereference raw pointer.
 */
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
/**
 * Dereference shared pointer.
 */
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
/**
 * @brief Instance of pointer dereferencing object.
 */
template <typename T>
constexpr pointer_as_reference_t<T> pointer_as_reference_inst{};

//-------------------------------------------------------------

/**
 * @brief Implementer of as_reference().
 */
struct pointer_as_reference_impl
{
    template <typename T>
    constexpr auto operator () (T&& v) const -> decltype(auto)
    {
        return pointer_as_reference_inst<std::decay_t<T>>(std::forward<T>(v));
    }
};
/**
 * @brief Dereference pointer if applicable.
 * @param v Value.
 * @return Dereferenced pointer if value is of pointer type, otherwise value as is.
 */
constexpr pointer_as_reference_impl as_reference{};

//-------------------------------------------------------------

/**
 * @brief Implementer of is_pointer.
 */
struct is_pointer_impl
{
    template <typename T>
    constexpr auto operator () (T&& v) const
    {
        std::ignore=v;
        using type=typename pointer_as_reference_t<std::decay_t<T>>::is_pointer;
        return type{};
    }
};
/**
 * @brief Helper to figure out if type is a smart pointer or shared pointer.
 * @param v Object.
 * @return Logical integral constant.
 */
constexpr is_pointer_impl is_pointer{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_POINTER_AS_REFERENCE_HPP
