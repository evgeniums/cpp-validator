/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/operator.hpp
*
*  Defines templates to adjust types to be used as storable elements in tuples.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP
#define DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP

#include <string>
#include <type_traits>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T, typename=hana::when<true>>
struct adjust_storable_type
{
};

/**
 * @brief Transform stringable type to std::string.
 *
 * "Stringable type" means a type from which a std::string can be constructed.
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<std::is_constructible<std::string,T>::value>
                    >
{
    using type=std::string;
};

/**
 * @brief Adjust non-stringable type
 *
 * Decay to base type that can store a value as a tuple's element.
 */
template <typename T>
struct adjust_storable_type<T,
                        hana::when<!std::is_constructible<std::string,T>::value>
                    >
{
    using type=std::decay_t<T>;
};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADJUST_STORABLE_TYPE_HPP
