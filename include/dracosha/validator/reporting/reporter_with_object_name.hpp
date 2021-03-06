/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/reporter_with_object_name.hpp
*
*  Defines reporter with object name.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REPORTER_WITH_OBJECT_NAME_HPP
#define DRACOSHA_VALIDATOR_REPORTER_WITH_OBJECT_NAME_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/reporter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Reporter that constructs report including name of the object under validation.
 *
 * The object's name is prepended to report in such a way as it were the most significant section of a member name,
 * and a member names formatter is used to format the object's name.
 */
template <typename DstT, typename FormatterT, typename ObjectT>
class reporter_with_object_name : public reporter<DstT,FormatterT>
{
    public:

        using base_type=reporter<DstT,FormatterT>;

        /**
         * @brief Constructor.
         * @param dst Destination object wrapped into backend formatter.
         * @param formatter Formatter to use for reports formatting.
         * @param obj Object name to prepend to report.
         */
        template <typename T>
        reporter_with_object_name(
                    T&& dst,
                    FormatterT&& formatter,
                    ObjectT&& obj
                ) : base_type(std::forward<T>(dst),std::forward<FormatterT>(formatter)),
                    _obj(std::forward<ObjectT>(obj))
        {}

        /**
         *  @brief Report validation of object at one level without member nesting.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         */
        template <typename T2, typename OpT>
        void validate_operator(const OpT& op, const T2& b)
        {
            validate_property(value,op,b);
        }

        /**
         *  @brief Report validation of object's property at one level without member nesting.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         */
        template <typename T2, typename OpT, typename PropT>
        void validate_property(const PropT& prop, const OpT& op, const T2& b)
        {
            base_type::validate(_[_obj],prop,op,b);
        }

        /**
         *  @brief Report validation of existance of a member.
         *  @param member Member descriptor.
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist.
         */
        template <typename T2, typename OpT, typename MemberT>
        void validate_exists(const MemberT& member, const OpT& op, const T2& b)
        {
            base_type::validate_exists(as_member(member),op,b);
        }

        /**
         *  @brief Report normal validation of a member.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        void validate(const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
        {
            base_type::validate(as_member(member),prop,op,b);
        }

        /**
         *  @brief Report validation using other member of the same object as a reference argument for validation.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Descriptor of sample member of the same object.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        void validate_with_other_member(const MemberT& member, const PropT& prop, const OpT& op, const T2& b)
        {
            base_type::validate_with_other_member(as_member(member),prop,op,as_member(b));
        }

        /**
         *  @brief Report validation using the same member of a Sample object.
         *  @param member Member.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param member1 Member of sample object.
         *  @param b Sample object whose member must be used as argument passed to validation operator.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT, typename MemberSampleT>
        void validate_with_master_sample(const MemberT& member, const PropT& prop, const OpT& op, const MemberSampleT& member_sample, const T2& b)
        {
            base_type::validate_with_master_sample(as_member(member),prop,op,member_sample,b);
        }

    private:

        template <typename MemberT>
        auto as_member(const MemberT& member)
        {
            return member.make_super(_obj);
        }

        ObjectT _obj;
};

/**
 * @brief Make a reporter with object name and given formatter.
 * @param dst Destination object where to put reports.
 * @param formatter Formatter to use for reports formatting.
 * @param obj Name of the object under validation that must be included to report.
 * @return Reporter wrapping the destination object.
 */
template <typename DstT, typename FormatterT, typename ObjectT>
auto make_reporter_with_object_name(DstT& dst, FormatterT&& formatter, ObjectT&& obj)
{
    auto wrapper=wrap_backend_formatter(dst);
    return reporter_with_object_name<decltype(wrapper),FormatterT,ObjectT>(std::move(wrapper),std::forward<FormatterT>(formatter),std::forward<ObjectT>(obj));
}

/**
 * @brief Make a reporter with object name and default formatter.
 * @param dst Destination object where to put reports.
 * @param obj Name of the object under validation that must be included to report.
 * @return Reporter wrapping the destination object.
 */
template <typename DstT, typename ObjectT>
auto make_reporter_with_object_name(DstT& dst, ObjectT&& obj)
{
    return make_reporter_with_object_name(dst,get_default_formatter(),std::forward<ObjectT>(obj));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTER_WITH_OBJECT_NAME_HPP
