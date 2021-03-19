/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/to_string.hpp
*
* Defines helper for strings conversion.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TO_STRING_HPP
#define DRACOSHA_VALIDATOR_TO_STRING_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/aggregation/wrap_it.hpp>
#include <dracosha/validator/aggregation/wrap_index.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  @brief Convert to <?????> token if no other conversion is possible.
 */
template <typename T, typename =hana::when<true>>
struct to_string_impl
{
    template <typename T1>
    std::string operator () (T1&&) const
    {
        return std::string("<\?\?\?\?\?>");
    }
};

/**
 *  @brief Convert to string if string is constructible of argument.
 */
template <typename T>
struct to_string_impl<T,hana::when<
            std::is_constructible<std::string,T>::value
            &&
            !hana::is_a<property_tag,T>
        >>
{
    template <typename T1>
    std::string operator () (T1&& id) const
    {
        return std::string(std::forward<T1>(id));
    }
};

/**
 *  @brief Convert to string if argument is a property.
 */
template <typename T>
struct to_string_impl<T,hana::when<hana::is_a<property_tag,T>>>
{
    std::string operator () (const T& v) const
    {
        return std::string(v.name());
    }
};

struct tree_tag;
/**
 *  @brief Convert to string if argument is an iterator.
 */
template <typename T>
struct to_string_impl<T,hana::when<
            hana::is_a<wrap_iterator_tag,T>
            ||
            hana::is_a<wrap_index_tag,T>
            ||
            hana::is_a<tree_tag,T>
        >>
{
    std::string operator () (const T& id) const
    {
        return std::string(id.name());
    }
};

/**
 *  @brief Check if sdt::to_string() can be callable with the given type.
 */
template <typename T, typename=void>
struct can_to_string
{
    constexpr static const bool value=false;
};
/**
 *  @brief Check if std::to_string() can be callable with the given type.
 */
template <typename T>
struct can_to_string<T,
        decltype((void)std::to_string(std::declval<T>()))>
{
    constexpr static const bool value=true;
};

/**
 *  @brief Convert to string if string can be constructible using sdt::to_string().
 */
template <typename T>
struct to_string_impl<T,hana::when<
            can_to_string<T>::value
            &&
            !hana::is_a<wrap_index_tag,T>
        >>
{
    std::string operator () (const T& id) const
    {
        return std::to_string(id);
    }
};

/**
 * @brief Template instance for converting variable to string.
 */
template <typename T>
constexpr to_string_impl<T> to_string_inst{};

/**
 * @brief Convert argument to string.
 * @param v Argument
 * @return String
 */
struct to_string_t
{
    template <typename T>
    std::string operator () (const T& v) const
    {
        return to_string_inst<std::decay_t<decltype(unwrap_object(v))>>(unwrap_object(v));
    }
};
constexpr to_string_t to_string{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TO_STRING_HPP
