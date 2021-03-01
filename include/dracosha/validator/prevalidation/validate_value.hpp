/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/validate_value.hpp
*
*  Defines "validate_value" helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATE_VALUE_HPP
#define DRACOSHA_VALIDATOR_VALIDATE_VALUE_HPP

#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Check if member's value can be validated by the validator.
 * @param member Member name.
 * @param val Value to set.
 * @param validator Validator to use for validation.
 * @param err Validation result.
 */
template <typename MemberT, typename ValueT, typename ValidatorT>
void validate_value(
        MemberT&& member,
        ValueT&& val,
        ValidatorT&& validator,
        error_report& err
    )
{
    validate(member,wrap_strict_any(std::forward<ValueT>(val),validator),extract_strict_any(std::forward<ValidatorT>(validator)),err);
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATE_VALUE_HPP
