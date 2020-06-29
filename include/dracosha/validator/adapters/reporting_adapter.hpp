/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/reporting_adapter.hpp
*
*  Defines reporting adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/adapters/adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  @brief Reporting adapter
 *
 *  Adaper first tries to perform validation and if validation fails the adapter then calls reporter to construct a report describing the failure.
 */
template <typename AdapterT, typename ReporterT>
struct reporting_adapter_t
{
    using hana_tag=adapter_tag;

    const AdapterT& _next_adapter;
    ReporterT& _reporter;

    /**
     *  @brief Perform validation of object at one level without member nesting
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT>
    bool validate_operator(OpT&& op, T2&& b)
    {
        if (!_next_adapter.validate_operator(op,b))
        {
            _reporter.validate_operator(op,b);
            return false;
        }
        return true;
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT>
    bool validate_property(PropT&& prop, OpT&& op, T2&& b)
    {
        if (!_next_adapter.validate_property(prop,op,b))
        {
            _reporter.validate_property(prop,op,b);
            return false;
        }
        return true;
    }

    /**
     *  @brief Validate existance of a member
     *  @param a Object to validate
     *  @param member Member descriptor
     *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
     *  @return Validation status
     */
    template <typename T2, typename MemberT>
    bool validate_exists(MemberT&& member, T2&& b)
    {
        if (!_next_adapter.validate_exists(member,b))
        {
            _reporter.validate_exists(member,b);
            return false;
        }
        return true;
    }

    /**
     *  @brief Normal validation of a member
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate(MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!_next_adapter.validate(member,prop,op,b))
        {
            _reporter.validate(member,prop,op,b);
            return false;
        }
        return true;
    }

    /**
     *  @brief Validate using other member of the same object as a reference argument for validation
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Descriptor of reference member of the same object
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate_with_other_member(MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!_next_adapter.validate(member,prop,op,b))
        {
            _reporter.validate_with_other_member(member,prop,op,b);
            return false;
        }
        return true;
    }

    /**
     *  @brief Validate using the same member of a reference object
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference object whose member to use as argument passed to validation operator
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate_with_master_reference(MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!_next_adapter.validate(member,prop,op,b))
        {
            _reporter.validate_with_master_reference(member,prop,op,b);
            return false;
        }
        return true;
    }

    /**
     * @brief Execute validators on object and aggregate their results using logical AND
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename OpsT>
    bool validate_and(OpsT&& ops)
    {
        if (!_next_adapter.validate_and(ops))
        {
            _reporter.validate_and();
            return false;
        }
        return true;
    }

    /**
     * @brief Execute validators on object's member and aggregate their results using logical AND
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename OpsT, typename MemberT>
    bool validate_and(MemberT&& member,OpsT&& ops)
    {
        if (!_next_adapter.validate_and(member,ops))
        {
            _reporter.validate_and(member);
            return false;
        }
        return true;
    }

    /**
     * @brief Execute validators on object and aggregate their results using logical OR
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename OpsT>
    bool validate_or(OpsT&& ops)
    {
        if (!_next_adapter.validate_or(ops))
        {
            _reporter.validate_or();
            return false;
        }
        else
        {
            _reporter.reset();
        }
        return true;
    }

    /**
     * @brief Execute validators on object's member and aggregate their results using logical OR
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename OpsT, typename MemberT>
    bool validate_or(MemberT&& member,OpsT&& ops)
    {
        if (!_next_adapter.validate_or(member,ops))
        {
            _reporter.validate_or(member);
            return false;
        }
        else
        {
            _reporter.reset();
        }
        return true;
    }

    /**
     * @brief Execute validator on object and negate the result
     * @param op Intermediate validator or validation operator
     * @return Logical NOT of results of intermediate validator
     */
    template <typename OpT>
    bool validate_not(OpT&& op)
    {
        if (!_next_adapter.validate_not(op))
        {
            _reporter.validate_not();
            return false;
        }
        else
        {
            _reporter.reset();
        }
        return true;
    }

    /**
     * @brief Execute validator on object's member and negate the result
     * @param member Member to process with validator
     * @param op Intermediate validator or validation operator
     * @return Logical NOT of results of intermediate validator
     */
    template <typename OpT, typename MemberT>
    bool validate_not(MemberT&& member,OpT&& op)
    {
        if (!_next_adapter.validate_not(member,op))
        {
            _reporter.validate_not(member);
            return false;
        }
        else
        {
            _reporter.reset();
        }
        return true;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP
