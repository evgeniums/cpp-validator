/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/reporter.hpp
*
*  Defines reporter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REPORTER_HPP
#define DRACOSHA_VALIDATOR_REPORTER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct reporter_tag;

/**
 * @brief Reporter constructs a report regarding the failure.
 *
 * A report will be put to the destination object which is usually a sring but
 * formatter backends can also support other container types.
 *
 * Actual string formatting is performed by the formatter object.
 */
template <typename DstT, typename FormatterT>
struct reporter
{
    using hana_tag=reporter_tag;

    DstT& _dst;
    const FormatterT& _formatter;

    /**
     *  @brief Report validation of object at one level without member nesting
     *  @param op Operator for validation
     *  @param b Sample argument for validation
     */
    template <typename T2, typename OpT>
    void validate_operator(const OpT& op, const T2& b)
    {
        _formatter.validate_operator(_dst,op,b);
    }

    /**
     *  @brief Report validation of object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Sample argument for validation
     */
    template <typename T2, typename OpT, typename PropT>
    void validate_property(const PropT& prop, const OpT& op, const T2& b)
    {
        _formatter.validate_property(_dst,prop,op);
    }

    /**
     *  @brief Report validation of existance of a member
     *  @param member Member descriptor
     *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
     */
    template <typename T2, typename MemberT>
    void validate_exists(const MemberT& member, const T2& b)
    {
        _formatter.validate_exists(_dst,member,b);
    }

    /**
     *  @brief Report normal validation of a member
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Sample argument for validation
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    void validate(const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
    {
        _formatter.validate(_dst,member,prop,op,b);
    }

    /**
     *  @brief Report validation using other member of the same object as a reference argument for validation
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Descriptor of sample member of the same object
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_other_member(const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
    {
        _formatter.validate_with_other_member(_dst,member,prop,op,b);
    }

    /**
     *  @brief Report validation using the same member of a Sample object
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Sample object whose member to use as argument passed to validation operator
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_master_sample(const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
    {
        _formatter.validate_with_master_sample(_dst,member,prop,op,b);
    }

    /**
     * @brief Report logical AND
     */
    void validate_and()
    {
        _formatter.validate_and(_dst);
    }

    /**
     * @brief Report logical AND on object's member
     * @param member Member to process with validators
     */
    template <typename MemberT>
    void validate_and(const MemberT& member)
    {
        _formatter.validate_and(_dst,member);
    }

    /**
     * @brief Report logical OR
     */
    void validate_or()
    {
        _formatter.validate_or(_dst);
    }

    /**
     * @brief Report logical OR on object's member
     * @param member Member to process with validators
     */
    template <typename MemberT>
    void validate_or(const MemberT& member)
    {
        _formatter.validate_or(_dst,member);
    }

    /**
     * @brief Report logical NOT
     */
    void validate_not()
    {
        _formatter.validate_not(_dst);
    }

    /**
     * @brief Report logical NOT on object's member
     * @param member Member to process with validators
     */
    template <typename MemberT>
    void validate_not(const MemberT& member)
    {
        _formatter.validate_not(_dst,member);
    }

    /**
     * @brief Reset reporter
     *
     * Usually it just means clearing the report destination.
     */
    void reset()
    {
        _dst.clear();
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTER_HPP
