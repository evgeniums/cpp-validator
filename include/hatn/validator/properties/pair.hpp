/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/properties/pair.hpp
*
*  Defines "first" and "second" properties to use with pairs/iterators.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_PAIR_HPP
#define HATN_VALIDATOR_PAIR_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/property.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

HATN_VALIDATOR_PROPERTY(first)

HATN_VALIDATOR_PROPERTY(second)

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_PAIR_HPP
