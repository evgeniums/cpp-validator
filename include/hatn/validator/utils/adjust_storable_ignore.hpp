/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/adjust_storable_ignore.hpp
*
*  Defines adjust_storable_ignore.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_ADJUST_STORABLE_IGNORE_HPP
#define HATN_VALIDATOR_ADJUST_STORABLE_IGNORE_HPP

#include <hatn/validator/config.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Struct to be used as a base struct for objects that must be copied as is to adjust_storable_t wrapper.
 */
struct adjust_storable_ignore{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_ADJUST_STORABLE_IGNORE_HPP
