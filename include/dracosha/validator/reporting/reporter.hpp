/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/reporter.hpp
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
 * formatter backends can also support other container types. Destination object
 * is wrapped into backend formatter that knows how to format data to that object.
 *
 * Actual formatting is performed by the formatter object.
 */
template <typename DstT, typename FormatterT>
class reporter
{
    public:

        using hana_tag=reporter_tag;

        /**
         * @brief Constructor.
         * @param dst Destination object wrapped into backend formatter.
         * @param formatter Formatter to use for reports formatting.
         */
        reporter(
                    DstT dst,
                    FormatterT&& formatter
                ) : _dst(std::move(dst)),
                    _formatter(std::forward<FormatterT>(formatter)),
                    _not_count(0),
                    _explicit_reporting_count(0)
        {}

        /**
         * @brief Open validation step for aggregation operator.
         * @param aggregation Descriptor of aggregation operator.
         */
        template <typename AggregationT>
        void aggregate_open(AggregationT&& aggregation)
        {
            if (skip_aggregate_open() || skip_explicit_report())
            {
                return;
            }
            if (aggregation.id==aggregation_id::NOT)
            {
                ++_not_count;
            }
            _stack.emplace_back(std::forward<AggregationT>(aggregation));
        }

        /**
         * @brief Open validation step for aggregation operator with member.
         * @param aggregation Descriptor of aggregation operator.
         * @param member Member the validation operation is performed for.
         */
        template <typename AggregationT, typename MemberT>
        void aggregate_open(AggregationT&& aggregation, MemberT&& member)
        {
            if (skip_aggregate_open() || skip_explicit_report())
            {
                return;
            }
            if (aggregation.id==aggregation_id::NOT)
            {
                ++_not_count;
            }
            _stack.emplace_back(std::forward<AggregationT>(aggregation),
                                _formatter.member_to_string(std::forward<MemberT>(member)));
        }

        /**
         * @brief Close validation step for aggregation operator.
         * @param ok Validation status of the aggregation operator.
         */
        void aggregate_close(bool ok)
        {
            if (skip_explicit_report())
            {
                return;
            }

            if (!_stack.empty())
            {
                auto& back=_stack.back();
                if (skip_part())
                {
                    --back.any_all_count;
                    if (back.any_all_count!=0)
                    {
                        return;
                    }
                }

                if (!ok || current_not())
                {
                    updateBrackets();
                    auto wrapper=wrap_backend_formatter(report_dst(),_dst);
                    _formatter.aggregate(wrapper,back);
                }
                if (back.aggregation.id==aggregation_id::NOT)
                {
                    --_not_count;
                }
                _stack.pop_back();
            }
        }

        /**
         *  @brief Report validation of object at one level without member nesting.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         */
        template <typename T2, typename OpT>
        void validate_operator(const OpT& op, const T2& b)
        {
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            auto wrapper=wrap_backend_formatter(current(),_dst);
            _formatter.validate_operator(wrapper,op,b);
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
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            auto wrapper=wrap_backend_formatter(current(),_dst);
            _formatter.validate_property(wrapper,prop,op,b);
        }

        /**
         *  @brief Report validation of existance of a member.
         *  @param member Member descriptor.
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist.
         */
        template <typename T2, typename OpT, typename MemberT>
        void validate_exists(const MemberT& member, const OpT& op, const T2& b)
        {
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            auto wrapper=wrap_backend_formatter(current(),_dst);
            _formatter.validate_exists(wrapper,member,op,b);
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
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            auto wrapper=wrap_backend_formatter(current(),_dst);
            _formatter.validate(wrapper,member,prop,op,b);
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
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            auto wrapper=wrap_backend_formatter(current(),_dst);
            _formatter.validate_with_other_member(wrapper,member,prop,op,b);
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
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            auto wrapper=wrap_backend_formatter(current(),_dst);
            _formatter.validate_with_master_sample(wrapper,member,prop,op,member_sample,b);
        }

        /**
         * @brief Get object where the reports go to.
         * @return Reference to destination object.
         */
        template <typename T>
        T& destination()
        {
            return _dst;
        }

        /**
         * @brief Check if current validation step is within NOT operator.
         * @return True if NOT aggregation operator is opened at any parent level.
         */
        bool current_not() const
        {
            return _not_count!=0;
        }

        /**
         * @brief Begin report that uses reporting hint and ignores reportings from all next levels.
         */
        void begin_explicit_report()
        {
            ++_explicit_reporting_count;
        }

        /**
         * @brief End report that uses reporting hint and ignores reportings from all next levels.
         * @param description Reporting hint that overrides report of the current level.
         */
        void end_explicit_report(const std::string& description)
        {
            --_explicit_reporting_count;
            if (skip_part())
            {
                return;
            }
            if (_explicit_reporting_count==0)
            {
                auto wrapper=wrap_backend_formatter(current(),_dst);
                wrapper.append(description);
            }
        }

    private:

        bool skip_explicit_report() const noexcept
        {
            return _explicit_reporting_count!=0;
        }

        bool skip_part() const noexcept
        {
            if (!_stack.empty())
            {
                const auto& back=_stack.back();
                if (back.aggregation.id==aggregation_id::ANY
                        ||
                    back.aggregation.id==aggregation_id::ALL
                    )
                {
                    return !back.parts.empty();
                }
            }
            return false;
        }

        bool skip_aggregate_open()
        {
            if (skip_part())
            {
                ++_stack.back().any_all_count;
                return true;
            }
            return false;
        }

        typename DstT::type& current()
        {
            if (!_stack.empty())
            {
                _stack.back().parts.emplace_back();
                return _stack.back().parts.back();
            }
            return _dst;
        }

        typename DstT::type& report_dst()
        {
            if (_stack.size()>1)
            {
                _stack.at(_stack.size()-2).parts.emplace_back();
                return _stack.at(_stack.size()-2).parts.back();
            }
            return _dst;
        }

        void updateBrackets()
        {
            if (_stack.size()>1
                    &&
                (
                    (_stack.at(_stack.size()-2).parts.size()>1
                     ||
                     _stack.at(_stack.size()-2).aggregation.id==aggregation_id::NOT
                     )
                        &&
                    _stack.back().parts.size()>1
                )
            )
            {
                _stack.back().single=false;
            }
        }

        DstT _dst;
        FormatterT _formatter;
        std::vector<report_aggregation<typename DstT::type>> _stack;
        size_t _not_count;
        size_t _explicit_reporting_count;
};

/**
 * @brief Make a reporter with formatter.
 * @param dst Destination object where to put reports.
 * @param formatter Formatter to use for reports formatting.
 * @return Reporter wrapping the destination object.
 */
template <typename DstT, typename FormatterT>
auto make_reporter(DstT& dst, FormatterT&& formatter)
{
    auto wrapper=wrap_backend_formatter(dst);
    return reporter<decltype(wrapper),FormatterT>(std::move(wrapper),std::forward<FormatterT>(formatter));
}

/**
 * @brief Make a reporter with default formatter.
 * @param dst Destination object where to put reports.
 * @return Reporter wrapping the destination object.
 */
template <typename DstT>
auto make_reporter(DstT& dst)
{
    return make_reporter(dst,get_default_formatter());
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTER_HPP
