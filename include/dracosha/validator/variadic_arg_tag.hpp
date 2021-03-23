/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/variadic_arg_tag.hpp
*
* Defines tags of variadic arguments.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VARIADIC_ARG_TAG_HPP
#define DRACOSHA_VALIDATOR_VARIADIC_ARG_TAG_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base tag of variadic arguments.
 */
struct variadic_arg_tag{};

/**
 * @brief Base tag of variadic arguments with element aggregations.
 */
struct variadic_arg_aggregation_tag{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VARIADIC_ARG_TAG_HPP
