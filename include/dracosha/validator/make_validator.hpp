/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/make_validator.hpp
*
*  Defines creator of validator instances
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAKE_HPP
#define DRACOSHA_VALIDATOR_MAKE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/validators.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Create validator instance
  @param fn Validation handler that will be held in validator instance
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto make_validator = [](auto fn)
{
    return validator_t<decltype(fn)>(std::move(fn));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAKE_HPP
