/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/formatter.hpp
*
*  Defines formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMATTER_HPP
#define DRACOSHA_VALIDATOR_FORMATTER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_exists_t
{
};
constexpr string_exists_t string_exists{};

struct string_not_exists_t
{
};
constexpr string_not_exists_t string_not_exists{};

struct string_master_reference_t
{
};
constexpr string_master_reference_t string_master_reference{};

struct string_and_t
{
};
constexpr string_and_t string_and{};

struct string_or_t
{
};
constexpr string_or_t string_or{};

struct string_not_t
{
};
constexpr string_not_t string_not{};


struct formatter_tag;

template <typename DstT, typename StringsT, typename MemberNamesT>
struct formatter_t
{
    using hana_tag=formatter_tag;

    const StringsT& _strings;
    const MemberNamesT& _member_names;

    template <typename T2, typename OpT>
    void validate_operator(DstT& dst,const OpT& op, const T2& b)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {}",_strings(op),b);
#else
#endif
    }

    /**
     *  @brief Report validation of object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     */
    template <typename T2, typename OpT, typename PropT>
    void validate_property(DstT& dst,const PropT& prop, const OpT& op, const T2& b)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {} {}",_strings(prop),_strings(op),b);
#else
#endif
    }

    /**
     *  @brief Report validation of existance of a member
     *  @param member Member descriptor
     *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
     */
    template <typename T2, typename MemberT>
    void validate_exists(DstT& dst, const MemberT& member, const T2& b)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        if (b)
        {
            dst=fmt::format("{} {}",_member_names(member),_strings(string_exists));
        }
        else
        {
            dst=fmt::format("{} {}",_member_names(member),_strings(string_not_exists));
        }
#else

#endif
    }

    /**
     *  @brief Report normal validation of a member
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    void validate(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {} {} {}",_strings(prop),_member_names(member),_strings(op),b);
#else

#endif
    }

    /**
     *  @brief Report validation using other member of the same object as a reference argument for validation
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Descriptor of reference member of the same object
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_other_member(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {} {} {}",_strings(prop),_member_names(member),_strings(op),_member_names(b));
#else

#endif
    }

    /**
     *  @brief Report validation using the same member of a reference object
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference object whose member to use as argument passed to validation operator
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_master_reference(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2&)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {} {} {}",_strings(prop),_member_names(member),_strings(op),_strings(string_master_reference));
#else

#endif
    }

    /**
     * @brief Report logical AND
     */
    void validate_and(DstT& dst)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{}: {}",_strings(string_and),_strings(dst));
#else

#endif
    }

    /**
     * @brief Report logical AND on object's member
     * @param member Member to process with validators
     */
    template <typename MemberT>
    void validate_and(DstT& dst, const MemberT& member)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{}: {} {}",_strings(string_and),_member_names(member),_strings(dst));
#else

#endif
    }

    /**
     * @brief Report logical OR
     */
    void validate_or(DstT& dst)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{}: {}",_strings(string_or),_strings(dst));
#else

#endif
    }

    /**
     * @brief Report logical OR on object's member
     * @param member Member to process with validators
     */
    template <typename MemberT>
    void validate_or(DstT& dst, const MemberT& member)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{}: {} {}",_strings(string_or),_member_names(member),_strings(dst));
#else

#endif
    }

    /**
     * @brief Report logical NOT
     */
    void validate_not(DstT& dst)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {}",_strings(string_not),_strings(dst));
#else

#endif
    }

    /**
     * @brief Report logical NOT on object's member
     * @param member Member to process with validators
     */
    template <typename MemberT>
    void validate_not(DstT& dst, const MemberT& member)
    {
#ifdef DRACOSHA_VALIDATOR_FMT
        dst=fmt::format("{} {} {}",_member_names(member),_strings(string_not),_strings(dst));
#else

#endif
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_HPP
