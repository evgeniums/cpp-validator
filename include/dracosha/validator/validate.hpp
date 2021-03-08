/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/validate.hpp
*
*  Defines validate() helper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATE_HPP
#define DRACOSHA_VALIDATOR_VALIDATE_HPP

#include <stdexcept>

#include <dracosha/validator/error.hpp>

#include <dracosha/validator/validators.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/adapters/prevalidation_adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Implementation of a helper to invoke validate() as a single callable.
 */
struct validate_t
{
    /**
     * @brief Validate object with validator and put result to the last argument.
     * @brief obj Object to validate.
     * @brief validator Validator.
     * @brief err Error to put validation result to.
     */
    template <typename ObjectT, typename ValidatorT>
    void operator() (
            ObjectT&& obj,
            ValidatorT&& validator,
            error& err
        ) const
    {
        err.set_value(validator.apply(std::forward<ObjectT>(obj)));
    }

    /**
     * @brief Validate object with validator and put validation result with error description to the last argument.
     * @brief obj Object to validate.
     * @brief validator Validator.
     * @brief err Error to put validation result to.
     */
    template <typename ObjectT, typename ValidatorT>
    void operator() (
            ObjectT&& obj,
            ValidatorT&& validator,
            error_report& err
        ) const
    {
        err.reset();
        err.set_value(validator.apply(
                          make_reporting_adapter(
                              std::forward<ObjectT>(obj),
                              err._message
                          )
                      ));
    }

    /**
     * @brief Validate object with validator and throw validation_error if operation fails.
     * @brief obj Object to validate.
     * @brief validator Validator.
     *
     * @throws validation_error if validation fails.
     */
    template <typename ObjectT, typename ValidatorT>
    void operator() (
            ObjectT&& obj,
            ValidatorT&& validator
        ) const
    {
        error_report err;
        (*this)(std::forward<ObjectT>(obj),std::forward<ValidatorT>(validator),err);
        if (err)
        {
            throw validation_error(err);
        }
    }

    /**
     * @brief Pre-validate object's member with validator and put validation result with error description to the last argument.
     * @brief member Path of the member to validate.
     * @brief obj Object to validate.
     * @brief validator Validator.
     */
    template <typename MemberT, typename ValueT, typename ValidatorT>
    void operator() (
            MemberT&& member,
            ValueT&& val,
            ValidatorT&& validator,
            error_report& err
        ) const
    {
        err.reset();
        err.set_value(validator.apply(
                          make_prevalidation_adapter(
                              std::forward<MemberT>(member),
                              std::forward<ValueT>(val),
                              err._message
                          )
                      ));
    }

    /**
     * @brief Pre-validate object's member with validator and throw validation_error if operation fails.
     * @brief member Path of the member to validate.
     * @brief obj Object to validate.
     * @brief validator Validator.
     *
     * @throws validation_error if validation fails.
     */
    template <typename MemberT, typename ValueT, typename ValidatorT>
    void operator() (
            MemberT&& member,
            ValueT&& val,
            ValidatorT&& validator
        ) const
    {
        error_report err;
        (*this)(std::forward<MemberT>(member),std::forward<ValueT>(val),std::forward<ValidatorT>(validator),err);
        if (err)
        {
            throw validation_error(err);
        }
    }
};

/**
 * @brief Helper to invoke validate() as a single callable.
 */
constexpr validate_t validate{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATE_HPP
