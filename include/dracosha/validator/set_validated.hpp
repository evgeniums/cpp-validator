\/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/validate.hpp
*
*  Defines "set_validated" helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SET_VALIDATED_HPP
#define DRACOSHA_VALIDATOR_SET_VALIDATED_HPP

#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Default implementation of member setter that uses [] to set element of container.
 */
template <typename ObjectT, typename MemberT>
struct set_member_t
{
    template <typename ObjectT1, typename MemberT1, typename ValueT>
    void operator() (
            ObjectT1& obj,
            MemberT1&& member,
            ValueT&& val
        ) const
    {
        obj[std::forward<MemberT1>(member)]=std::forward<ValueT>(val);
    }
};

/**
 * @brief Instantiation of member setter template.
 */
template <typename ObjectT, typename MemberT>
constexpr set_member_t<ObjectT,MemberT> set_member_inst{};

/**
 * @brief Set object's member.
 * @param obj Object whose member to set.
 * @param member Member name, only single level members are supported.
 * @param val Value to set.
 */
template <typename ObjectT, typename MemberT, typename ValueT>
void set_member(
        ObjectT& obj,
        MemberT&& member,
        ValueT&& val
    )
{
    set_member_inst<std::decay_t<ObjectT>,std::decay_t<MemberT>>(obj,std::forward<MemberT>(member),std::forward<ValueT>(val));
}

/**
 * @brief Set object's member with pre-validation with validation result put in the last argument.
 * @param obj Object whose member to set.
 * @param member Member name, only single level members are supported.
 * @param val Value to set.
 * @param validator Validator to use for validation.
 * @param err Validation result.
 */
template <typename ObjectT, typename MemberT, typename ValueT, typename ValidatorT>
void set_validated(
        ObjectT& obj,
        MemberT&& member,
        ValueT&& val,
        ValidatorT&& validator,
        error_report& err
    )
{
    validate(_[member],val,std::forward<ValidatorT>(validator),err);
    if (!err)
    {
        set_member(obj,member,std::forward<ValueT>(val));
    }
}

/**
 * @brief Set object's member with pre-validation with exception if validation fails.
 * @param obj Object whose member to set.
 * @param member Member name, only single level members are supported.
 * @param val Value to set.
 * @param validator Validator to use for validation.
 *
 * @throws validation_error if validation fails.
 */
template <typename ObjectT, typename MemberT, typename ValueT, typename ValidatorT>
void set_validated(
        ObjectT& obj,
        MemberT&& member,
        ValueT&& val,
        ValidatorT&& validator
    )
{
    error_report err;
    set_validated(obj,std::forward<MemberT>(member),std::forward<ValueT>(val),std::forward<ValidatorT>(validator),err);
    if (err)
    {
        throw validation_error(err);
    }
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SET_VALIDATED_HPP
