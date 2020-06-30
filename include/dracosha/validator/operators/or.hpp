/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/or.hpp
*
*  Defines logical pseudo operator OR
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OR_HPP
#define DRACOSHA_VALIDATOR_OR_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/detail/aggregate_or.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Logical pseudo operator OR
 * @param xs Intermediate validators whose result must be forwarded to logical OR
 * @return Logical "or" of intermediate validator results
 *
 * Can be used both in functioin call notation OR(...) and in infix notation (... ^OR^ ...)
 */
BOOST_HANA_CONSTEXPR_LAMBDA auto OR=hana::infix([](auto&& ...xs) -> decltype(auto)
{
    return make_validator(
                hana::reverse_partial(
                    detail::aggregate_or,
                    hana::make_tuple(std::forward<decltype(xs)>(xs)...)
                )
           );
});

//-------------------------------------------------------------
struct string_or_t : public op<string_or_t>
{
    constexpr static const char* description="none of the following conditions are met";
};
constexpr string_or_t string_or{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OR_HPP
