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

#ifndef HATN_VALIDATOR_VARIADIC_ARG_TAG_HPP
#define HATN_VALIDATOR_VARIADIC_ARG_TAG_HPP

#include <hatn/validator/config.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

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

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_VARIADIC_ARG_TAG_HPP
