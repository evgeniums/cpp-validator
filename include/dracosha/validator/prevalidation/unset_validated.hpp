/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/unset_validated.hpp
*
*  Defines "unset_validated" helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_UNSET_VALIDATED_HPP
#define DRACOSHA_VALIDATOR_UNSET_VALIDATED_HPP

#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Default implementation of member setter that uses erase() to unset element of container.
 */
template <typename ObjectT, typename MemberT, typename Enable=void>
struct unset_member_t
{
    template <typename ObjectT1, typename MemberT1>
    void operator() (
            ObjectT1& obj,
            MemberT1&& member
        ) const
    {
        obj.erase(member.key());
    }
};

/**
 * @brief Instantiation of member unsetter template.
 */
template <typename ObjectT, typename MemberT>
constexpr unset_member_t<ObjectT,MemberT> unset_member_inst{};

/**
 * @brief Set object's member.
 * @param obj Object whose member to set.
 * @param member Member.
 */
template <typename ObjectT, typename MemberT>
void unset_member(
        ObjectT& obj,
        MemberT&& member
    )
{
    unset_member_inst<std::decay_t<ObjectT>,std::decay_t<MemberT>>(obj,std::forward<MemberT>(member));
}

/**
 * @brief Unset object's member with pre-validation with validation result put in the last argument.
 * @param obj Object whose member to unset.
 * @param member Member.
 * @param validator Validator to use for validation.
 * @param err Validation result.
 */
template <typename ObjectT, typename MemberT, typename ValidatorT>
void unset_validated(
        ObjectT& obj,
        MemberT&& member,
        ValidatorT&& validator,
        error_report& err
    )
{
    validate(member[exists],false,std::forward<ValidatorT>(validator),err);
    if (!err)
    {
        unset_member(obj,member);
    }
}

/**
 * @brief Unset object's member with pre-validation with exception if validation fails.
 * @param obj Object whose member to unset.
 * @param member Member.
 * @param validator Validator to use for validation.
 *
 * @throws validation_error if validation fails.
 */
template <typename ObjectT, typename MemberT, typename ValidatorT>
void unset_validated(
        ObjectT& obj,
        MemberT&& member,
        ValidatorT&& validator
    )
{
    error_report err;
    unset_validated(obj,std::forward<MemberT>(member),std::forward<ValidatorT>(validator),err);
    if (err)
    {
        throw validation_error(err);
    }
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_UNSET_VALIDATED_HPP
