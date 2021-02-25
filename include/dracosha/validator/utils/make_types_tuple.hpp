/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/make_types_tuple.hpp
*
*  Defines helper to create a tuple of types from variadic arguments.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAKE_TYPES_TUPLE_HPP
#define DRACOSHA_VALIDATOR_MAKE_TYPES_TUPLE_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Make tuple of hana::type_c<> from variadic arguments.
 */
template <typename ...Args>
constexpr auto make_types_tuple(Args&&...)
{
    return hana::tuple_t<std::decay_t<Args>...>;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAKE_TYPES_TUPLE_HPP
