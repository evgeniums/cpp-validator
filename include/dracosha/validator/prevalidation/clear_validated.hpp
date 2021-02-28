/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/clear_validated.hpp
*
*  Defines "clear_validated" helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CLEAR_VALIDATED_HPP
#define DRACOSHA_VALIDATOR_CLEAR_VALIDATED_HPP

#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/utils/get_it.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Default implementation of that uses clear() to clear the object's element.
 */
template <typename ObjectT, typename MemberT, typename Enable=void>
struct clear_member_t
{
    template <typename ObjectT1, typename MemberT1>
    void operator() (
            ObjectT1& obj,
            MemberT1&& member
        ) const
    {
        auto path=member_path(member);
        if (check_exists(obj,path))
        {
            auto& element=get_member(obj,path);
            element.clear();
        }
    }
};

/**
 * @brief Instantiation of clearer template.
 */
template <typename ObjectT, typename MemberT>
constexpr clear_member_t<ObjectT,MemberT> clear_member_inst{};

/**
 * @brief Clear object's member.
 * @param obj Object whose member to clear.
 * @param member Member name.
 */
template <typename ObjectT, typename MemberT>
void clear_member(
        ObjectT& obj,
        MemberT&& member
    )
{
    clear_member_inst<std::decay_t<ObjectT>,std::decay_t<MemberT>>(obj,std::forward<MemberT>(member));
}

/**
 * @brief Clear object's member with pre-validation with validation result put in the last argument.
 * @param obj Object whose member to resize.
 * @param member Member name.
 * @param validator Validator to use for validation. If wrapped into strict_any then strict ANY validation will be invoked.
 * @param err Validation result.
 *
 * @note Use with caution. Only "size", "length" and "empty" properties are validated.
 * In case there are some conditions validating content, they are not checked.
 * For example, clearing string validated with `validator(_[string_field](gte,"Hello world!"))`
 * will not emit error despite the validation condition checking string content is not met.
 */
template <typename ObjectT, typename MemberT, typename ValidatorT>
void clear_validated(
        ObjectT& obj,
        MemberT&& member,
        ValidatorT&& validator,
        error_report& err
    )
{
    validate(member[empty],wrap_strict_any(true,validator),extract_strict_any(validator),err);
    if (!err)
    {
        validate(member[size],wrap_strict_any(0,validator),extract_strict_any(validator),err);
        if (!err)
        {
            clear_member(obj,member);
        }
    }
}

/**
 * @brief Clear object's member with pre-validation with exception if validation fails.
 * @param obj Object whose member to set.
 * @param member Member name.
 * @param validator Validator to use for validation. If wrapped into strict_any then strict ANY validation will be invoked.
 *
 * @throws validation_error if validation fails.
 *
 * @note See note for overloaded function.
 */
template <typename ObjectT, typename MemberT, typename ValidatorT>
void clear_validated(
        ObjectT& obj,
        MemberT&& member,
        ValidatorT&& validator
    )
{
    error_report err;
    clear_validated(obj,std::forward<MemberT>(member),std::forward<ValidatorT>(validator),err);
    if (err)
    {
        throw validation_error(err);
    }
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CLEAR_VALIDATED_HPP
