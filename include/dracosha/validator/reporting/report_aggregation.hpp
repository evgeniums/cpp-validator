/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/report_aggregation.hpp
*
*  Defines descriptor of report aggregation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_report_aggregation_HPP
#define DRACOSHA_VALIDATOR_report_aggregation_HPP

#include <vector>
#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/aggregation/aggregation.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct report_aggregation_tag;

/**
 * @brief Descriptor of aggregation operator used in validation report.
 */
template <typename DstT>
struct report_aggregation
{
    using hana_tag=report_aggregation_tag;

    /**
     * @brief Constructor.
     * @param aggregation Aggregation operation.
     * @param member Member the operation is applied to.
     */
    report_aggregation(
            aggregation_op aggregation,
            std::string member=std::string()
        ) : aggregation(std::move(aggregation)),
            member(std::move(member)),
            single(true),
            any_all_count(
                    static_cast<size_t>(aggregation.id==aggregation_id::ANY || aggregation.id==aggregation_id::ALL)
                )
    {}

    aggregation_op aggregation;

    std::string member;
    std::vector<DstT> parts;

    bool single;
    size_t any_all_count;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_report_aggregation_HPP
