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

template <typename T, typename=hana::when<true>>
struct adjust_storable_type
{
};

/**
 * @brief Convert const char* type to std::string.
 *
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<std::is_constructible<const char*,T>::value>
                    >
{
    using type=std::string;
};

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
 * @brief Adjust all the rest types that must be stored within object_wrapper.
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<
                            !std::is_base_of<adjust_storable_ignore,std::decay_t<T>>::value
                            &&
                            !std::is_constructible<const char*,T>::value
                            &&
                            !is_constant_size<std::decay_t<T>>::value
                        >
                    >
{
    using type=object_wrapper<T>;
};

/**
 * @brief Make storable type from a variable.
 */
struct adjust_storable_t
{
    template <typename T>
    auto operator () (T&& v) const
    {
        return typename adjust_storable_type<T>::type{std::forward<T>(v)};
    }
};
constexpr adjust_storable_t adjust_storable{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP
