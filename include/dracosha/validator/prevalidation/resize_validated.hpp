/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/resize_validated.hpp
*
*  Defines "resize_validated" helpers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_RESIZE_VALIDATED_HPP
#define DRACOSHA_VALIDATOR_RESIZE_VALIDATED_HPP

#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Default implementation of that uses resize() to resize the object itself.
 */
template <typename ObjectT, typename MemberT, typename Enable=void>
struct resize_member_t
{
    template <typename ObjectT1, typename MemberT1>
    void operator() (
            ObjectT1& obj,
            MemberT1&&,
            size_t size
        ) const
    {
        obj.resize(size);
    }
};

/**
 * @brief Instantiation of resizer template.
 */
template <typename ObjectT, typename MemberT>
constexpr resize_member_t<ObjectT,MemberT> resize_member_inst{};

/**
 * @brief Resize object's member.
 * @param obj Object whose member to resize.
 * @param member Member name.
 * @param size New size.
 */
template <typename ObjectT, typename MemberT>
void resize_member(
        ObjectT& obj,
        MemberT&& member,
        size_t size
    )
{
    resize_member_inst<std::decay_t<ObjectT>,std::decay_t<MemberT>>(obj,std::forward<MemberT>(member),size);
}

/**
 * @brief Resize object's member with pre-validation with validation result put in the last argument.
 * @param obj Object whose member to resize.
 * @param member Member name.
 * @param val New size.
 * @param validator Validator to use for validation.
 * @param err Validation result.
 *
 * @note Use with caution. Only "size" and "empty" properties are validated.
 * If there are some conditions regarding content then they are not checked.
 * For example, when resizing string with `validator(_[string_field](gte,"Hello world!"))`
 * then resizing to 0 will not emit error despite the validation condition
 * regarding string content is not met.
 */
template <typename ObjectT, typename MemberT, typename ValidatorT>
void resize_validated(
        ObjectT& obj,
        MemberT&& member,
        size_t val,
        ValidatorT&& validator,
        error_report& err
    )
{
    validate(member[size],val,validator,err);
    if (!err)
    {
        validate(member[empty],val==0,validator,err);
        if (!err)
        {
            resize_member(obj,member,val);
        }
    }
}

/**
 * @brief Resize object's member with pre-validation with exception if validation fails.
 * @param obj Object whose member to set.
 * @param member Member name.
 * @param size New size.
 * @param validator Validator to use for validation.
 *
 * @throws validation_error if validation fails.
 *
 * @note See note for overloaded function.
 */
template <typename ObjectT, typename MemberT, typename ValidatorT>
void resize_validated(
        ObjectT& obj,
        MemberT&& member,
        size_t size,
        ValidatorT&& validator
    )
{
    error_report err;
    resize_validated(obj,std::forward<MemberT>(member),size,std::forward<ValidatorT>(validator),err);
    if (err)
    {
        throw validation_error(err);
    }
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RESIZE_VALIDATED_HPP
