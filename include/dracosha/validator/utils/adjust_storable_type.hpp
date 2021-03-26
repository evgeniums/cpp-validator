/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/adjust_storable_type.hpp
*
*  Defines helpers for adjusting types used as storable elements in tuples.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP
#define DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_ignore.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/heterogeneous_size.hpp>
#include <dracosha/validator/heterogeneous_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper for adjusting types for storage in validator.
 * By default all storable objects are put into object_wrapper.
 */
template <typename T, typename=hana::when<true>>
struct adjust_storable_type
{
    using type=object_wrapper<T>;
};

/**
 * @brief Adjust const char* that must be stored as std::string.
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<std::is_constructible<const char*,T>::value>
                    >
{
    using type=std::string;
};

/**
 * @brief Adjust is_constant_size types that must be stored wrapped into heterogeneous_property_just_index_t.
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<is_constant_size<std::decay_t<T>>::value>
                    >
{
    using type=heterogeneous_property_just_index_t<std::decay_t<T>::value>;
};

/**
 * @brief Adjust special types that must be stored as copies.
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<
                            std::is_base_of<adjust_storable_ignore,std::decay_t<T>>::value
                        >
                    >
{
    using type=std::decay_t<T>;
};

/**
 * @brief Implementer of adjust_storable.
 */
struct adjust_storable_impl
{
    template <typename T>
    auto operator () (T&& v) const
    {
        return typename adjust_storable_type<T>::type{std::forward<T>(v)};
    }
};
/**
 * @brief Make storable type from a variable.
 * @param v Value to adjust.
 * @return Inpout value adjusted to storable type.
 */
constexpr adjust_storable_impl adjust_storable{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP
