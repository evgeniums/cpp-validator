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

#include <vector>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/report_aggregation.hpp>
#include <dracosha/validator/reporting/formatter.hpp>

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
class reporter
{
    public:

        using hana_tag=reporter_tag;

        reporter(
                    DstT& dst,
                    FormatterT&& formatter
                ) : _dst(dst),
                    _formatter(std::forward<FormatterT>(formatter))
        {}

        template <typename AggregationT>
        void aggregate_open(AggregationT&& aggregation)
        {
            if (!_stack.empty())
            {
                _stack.back().single=false;
            }
            _stack.emplace_back(std::forward<AggregationT>(aggregation));
        }
        template <typename AggregationT, typename MemberT>
        void aggregate_open(AggregationT&& aggregation, MemberT&& member)
        {
            if (!_stack.empty())
            {
                _stack.back().single=false;
            }
            _stack.emplace_back(std::forward<AggregationT>(aggregation),
                                _formatter.member_to_string(std::forward<MemberT>(member)));
        }

        void aggregate_close(bool ok)
        {
            if (!_stack.empty())
            {
                if (!ok)
                {
                    _formatter.aggregate(report_dst(),_stack.back());
                }
                _stack.pop_back();
            }
        }

        /**
         *  @brief Report validation of object at one level without member nesting
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         */
        template <typename T2, typename OpT>
        void validate_operator(const OpT& op, const T2& b)
        {
            _formatter.validate_operator(current(),op,b);
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
            _formatter.validate_property(current(),prop,op);
        }

        /**
         *  @brief Report validation of existance of a member
         *  @param member Member descriptor
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
         */
        template <typename T2, typename MemberT>
        void validate_exists(const MemberT& member, const T2& b)
        {
            _formatter.validate_exists(current(),member,b);
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
            _formatter.validate(current(),member,prop,op,b);
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
            _formatter.validate_with_other_member(current(),member,prop,op,b);
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
            _formatter.validate_with_master_sample(current(),member,prop,op,b);
        }

        DstT& destination()
        {
            return _dst;
        }

    private:

        DstT& current()
        {
            if (!_stack.empty())
            {
                return _stack.back().parts.emplace_back();
            }
            return _dst;
        }

        DstT& report_dst()
        {
            if (_stack.size()>1)
            {
                return _stack.at(_stack.size()-2).parts.emplace_back();
            }
            return _dst;
        }

        DstT& _dst;
        FormatterT _formatter;
        std::vector<report_aggregation<DstT>> _stack;
};

template <typename DstT, typename FormatterT>
auto make_reporter(DstT& dst, FormatterT&& formatter)
{
    return reporter<DstT,FormatterT>(dst,std::forward<FormatterT>(formatter));
}

template <typename DstT>
auto make_reporter(DstT& dst)
{
    return make_reporter(dst,get_default_formatter());
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTER_HPP
