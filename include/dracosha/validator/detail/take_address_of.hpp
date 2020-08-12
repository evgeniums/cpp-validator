/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/take_address_of.hpp
*
*  Defines helper for taking address of result returned from a function.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TAKE_ADDRESS_HPP
#define DRACOSHA_VALIDATOR_TAKE_ADDRESS_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Stub to use for address extracting.
 */
struct take_address_of_stub_t
{
};

/**
 * @brief Instance of stub to use for address extracting.
 */
constexpr take_address_of_stub_t take_address_of_stub{};

/**
 * @brief Default helper for taking address of stub variable.
 */
template <typename T, typename=hana::when<true>>
struct take_address_of_t
{
};

/**
 * @brief Helper for taking address of stub variable if supplied variable is not of lvalue reference type.
 */
template <typename T>
struct take_address_of_t<T,hana::when<!std::is_lvalue_reference<T>::value>>
{
    constexpr auto operator() (T) const
    {
        return &take_address_of_stub;
    }
};

/**
 * @brief Helper for taking address of supplied variable if it is of lvalue reference type.
 */
template <typename T>
struct take_address_of_t<T,hana::when<std::is_lvalue_reference<T>::value>>
{
    constexpr auto operator() (T t) const
    {
        return &t;
    }
};

/**
 * @brief Callable for taking address of variable.
 */
template <typename T>
constexpr take_address_of_t<T> take_address_of{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TAKE_ADDRESS_HPP
