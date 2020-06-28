/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/and.hpp
*
*  Defines logical pseudo operator AND
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AND_HPP
#define DRACOSHA_VALIDATOR_AND_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/detail/aggregate_and.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Logical pseudo operator AND
 * @param xs Intermediate validators whose result must be forwarded to logical AND
 * @return Logical "and" of intermediate validator results
 *
 * Can be used both in functioin call notation AND(...) and in infix notation (... ^AND^ ...)
 */
BOOST_HANA_CONSTEXPR_LAMBDA auto AND=hana::infix([](auto&& ...xs) -> decltype(auto)
{
    return make_validator(
                hana::reverse_partial(
                    detail::aggregate_and,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AND_HPP
