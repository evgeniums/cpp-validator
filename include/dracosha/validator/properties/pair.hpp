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

#ifndef DRACOSHA_VALIDATOR_PAIR_HPP
#define DRACOSHA_VALIDATOR_PAIR_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_PROPERTY(first)

DRACOSHA_VALIDATOR_PROPERTY(second)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PAIR_HPP
