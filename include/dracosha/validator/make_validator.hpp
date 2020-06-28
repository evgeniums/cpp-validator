/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/make_validator.hpp
*
*  Defines creator of validator implementation instance
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAKE_HPP
#define DRACOSHA_VALIDATOR_MAKE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/validator_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Create validator implementation instance
  @param fn Validation handler that will be held in validator instance
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto make_validator = [](auto fn)
{
    return detail::validator_impl<decltype(fn)>(std::move(fn));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAKE_HPP
