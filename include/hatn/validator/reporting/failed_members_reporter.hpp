/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/failed_members_reporter.hpp
*
*  Defines reporter that collects failed memners.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_REPORTER_FAILED_MEMBERS_HPP
#define HATN_VALIDATOR_REPORTER_FAILED_MEMBERS_HPP

#include <set>

#include <hatn/validator/config.hpp>
#include <hatn/validator/reporting/reporter.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Reporter collects failed member names.
 *
 */
class failed_members_reporter
{
    public:

        using hana_tag=reporter_tag;

        /**
         * @brief Constructor.
         */
        failed_members_reporter(
            ) : _not_count(0),
                _explicit_reporting_count(0)
        {}

        void reset()
        {
            _not_count=0;
            _explicit_reporting_count=0;
            _members.clear();
            _stack.clear();
        }

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
                    update_brackets();
                    report_dst();
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
        void validate_operator(const OpT&, const T2&)
        {
            if (skip_part() || skip_explicit_report())
            {
                return;
            }

            current();
        }

        /**
         *  @brief Report validation of object's property at one level without member nesting.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         */
        template <typename T2, typename OpT, typename PropT>
        void validate_property(const PropT&, const OpT&, const T2&)
        {
            if (skip_part() || skip_explicit_report())
            {
                return;
            }
            current();
        }

        /**
         *  @brief Report validation of existance of a member.
         *  @param member Member descriptor.
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist.
         */
        template <typename T2, typename OpT, typename MemberT>
        void validate_exists(const MemberT& member, const OpT&, const T2&)
        {
            if (skip_part())
            {
                return;
            }

            add_failed_member(member);

            if (skip_explicit_report())
            {
                return;
            }

            current();
        }

        /**
         *  @brief Report normal validation of a member.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        void validate(const MemberT& member, const PropT&, const OpT&, const T2&)
        {
            if (skip_part())
            {
                return;
            }

            add_failed_member(member);

            if (skip_explicit_report())
            {
                return;
            }

            current();
        }

        template <typename MemberT>
        void member_ok(const MemberT& member)
        {
            drop_failed_member(member);
        }

        /**
         *  @brief Report validation using other member of the same object as a reference argument for validation.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Descriptor of sample member of the same object.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        void validate_with_other_member(const MemberT& member, const PropT&, const OpT&, const T2&)
        {
            if (skip_part())
            {
                return;
            }

            add_failed_member(member);

            if (skip_explicit_report())
            {
                return;
            }

            current();
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
        void validate_with_master_sample(const MemberT& member, const PropT&, const OpT&, const MemberSampleT&, const T2&)
        {
            if (skip_part())
            {
                return;
            }

            add_failed_member(member);

            if (skip_explicit_report())
            {
                return;
            }

            current();
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
        void end_explicit_report(const std::string&)
        {
            --_explicit_reporting_count;
        }

        template <typename MemberT>
        void add_failed_member(const MemberT& member)
        {
            std::string m=dotted_member_names(member);
            if (std::find(std::begin(_members),std::end(_members),m)==std::end(_members))
            {
                _members.push_back(std::move(m));
            }
        }

        template <typename MemberT>
        void drop_failed_member(const MemberT& member)
        {
            std::string m=dotted_member_names(member);
            auto it=std::find(std::begin(_members),std::end(_members),m);
            if (it!=std::end(_members))
            {
                _members.erase(it);
            }
        }

        const std::vector<std::string>& failed_members() const
        {
            return _members;
        }

    private:

        void current()
        {
            if (!_stack.empty())
            {
                _stack.back().parts_count++;
            }
        }

        void report_dst()
        {
            if (_stack.size()>1)
            {
                _stack.at(_stack.size()-2).parts_count++;
            }
        }

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
                    return back.parts_count!=0;
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

        void update_brackets()
        {
            if (_stack.size()>1
                &&
                (
                    (_stack.at(_stack.size()-2).parts_count>1
                     ||
                     _stack.at(_stack.size()-2).aggregation.id==aggregation_id::NOT
                     )
                    &&
                    _stack.back().parts_count>1
                    )
                )
            {
                _stack.back().single=false;
            }
        }

        std::vector<empty_report_aggregation> _stack;
        size_t _not_count;
        size_t _explicit_reporting_count;

        std::vector<std::string> _members;
};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_REPORTER_FAILED_MEMBERS_HPP
