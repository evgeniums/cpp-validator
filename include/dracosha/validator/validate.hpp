/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/validate.hpp
*
*  Defines "validate" alias.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATE_HPP
#define DRACOSHA_VALIDATOR_VALIDATE_HPP

#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
  Alias to call validation dispatcher.
*/
constexpr dispatcher_t validate{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATE_HPP
