/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/to_string.hpp
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

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 *  @brief Convert to <?????> token if no other conversion is possible
 */
template <typename T, typename =hana::when<true>>
struct to_string_t
{
    std::string operator () (const T&) const
    {
        return std::string("<\?\?\?\?\?>");
    }
};

/**
 *  @brief Convert to string if string can be constructible of argument
 */
template <typename T>
struct to_string_t<T,hana::when<std::is_constructible<std::string,T>::value>>
{
    std::string operator () (const T& id) const
    {
        return id;
    }
};

/**
 *  @brief Convert to string if argument is a property
 */
template <typename T>
struct to_string_t<T,hana::when<hana::is_a<property_tag,T>>>
{
    std::string operator () (const T&) const
    {
        return std::string(T::name());
    }
};

/**
 *  @brief Check if sdt::to_string() can be callable with the given type
 */
template <typename T, typename=void>
struct can_to_string
{
    constexpr static const bool value=false;
};
/**
 *  @brief Check if sdt::to_string() can be callable with the given type
 */
template <typename T>
struct can_to_string<T,
        decltype((void)std::to_string(std::declval<T>()))>
{
    constexpr static const bool value=true;
};

/**
 *  @brief Convert to string string can be constructible using sdt::to_string()
 */
template <typename T>
struct to_string_t<T,hana::when<can_to_string<T>::value>>
{
    std::string operator () (const T& id) const
    {
        return std::to_string(id);
    }
};

template <typename T>
constexpr to_string_t<T> to_string{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TO_STRING_HPP
