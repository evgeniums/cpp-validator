/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/set_validated.hpp
*
*  Defines "set_validated" helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SET_VALIDATED_HPP
#define DRACOSHA_VALIDATOR_SET_VALIDATED_HPP

#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/prevalidation/validate_empty.hpp>
#include <dracosha/validator/prevalidation/validate_value.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Default implementation of member setter that uses [] to set element of container.
 */
template <typename ObjectT, typename MemberT, typename Enable=void>
struct set_member_t
{
    template <typename ObjectT1, typename MemberT1, typename ValueT>
    void operator() (
            ObjectT1& obj,
            MemberT1&& member,
            ValueT&& val
        ) const
    {
        auto parent_path=member.parent_path();
        if (check_exists(obj,parent_path))
        {
            auto& parent_element=get_member(obj,parent_path);
            parent_element[member.key()]=unadjust_view_type<decltype(parent_element[member.key()])>(std::forward<ValueT>(val));
        }
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
 * @param member Member name.
 * @param val Value to set.
 */
template <typename ObjectT, typename MemberT, typename ValueT>
void set_member(
        ObjectT& obj,
        MemberT&& member,
        ValueT&& val
    )
{
    set_member_inst<std::decay_t<ObjectT>,unwrap_object_t<MemberT>>
            (
                obj,
                unwrap_object(std::forward<MemberT>(member)),
                std::forward<ValueT>(val)
             );
}

/**
 * @brief Set object's member with pre-validation with validation result put in the last argument.
 * @param obj Object whose member to set.
 * @param member Member name.
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
    validate_value(member,wrap_strict_any(val,validator),extract_strict_any(std::forward<ValidatorT>(validator)),err);
    if (!err)
    {
        set_member(obj,member,std::forward<ValueT>(val));
    }
}

/**
 * @brief Set object's member with pre-validation with exception if validation fails.
 * @param obj Object whose member to set.
 * @param member Member name.
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
