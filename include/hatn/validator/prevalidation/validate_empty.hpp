/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/validate_empty.hpp
*
*  Defines helper to validate if object is empty.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_VALIDATE_EMPTY_HPP
#define HATN_VALIDATOR_VALIDATE_EMPTY_HPP

#include <hatn/validator/validate.hpp>
#include <hatn/validator/utils/get_it.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Validate if object can be empty.
 * @param member Member name.
 * @param validator Validator to use for validation. If wrapped into strict_any then strict ANY validation will be invoked.
 * @param err Validation result.
 */
template <typename MemberT, typename ValidatorT>
void validate_empty(
        MemberT&& member,
        ValidatorT&& validator,
        error_report& err
    )
{
    validate(member[empty],wrap_strict_any(true,validator),extract_strict_any(validator),err);
    if (!err)
    {
        validate(member[size],wrap_strict_any(0,validator),extract_strict_any(validator),err);
    }
}

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_VALIDATE_EMPTY_HPP
