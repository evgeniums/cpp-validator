/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/prepare_validate.hpp
*
*  Defines utilities for validation preparing
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREPARE_VALIDATE_HPP
#define DRACOSHA_VALIDATOR_PREPARE_VALIDATE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Run validation
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto validate_invoke = [](auto&&... args) -> decltype(auto)
{
    return validate.invoke(std::forward<decltype(args)>(args)...);
};

/**
 * @brief Prepare validation executor
 */
struct prepare_validate_t
{
    template <typename ... Args>
    constexpr auto operator() (Args&&... args) const -> decltype(auto)
    {
        return hana::reverse_partial(validate_invoke,std::forward<Args>(args)...);
    }
};
constexpr prepare_validate_t prepare_validate{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREPARE_VALIDATE_HPP
