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

#ifndef HATN_VALIDATOR_report_aggregation_HPP
#define HATN_VALIDATOR_report_aggregation_HPP

#include <vector>
#include <string>

#include <hatn/validator/config.hpp>
#include <hatn/validator/aggregation/aggregation.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct report_aggregation_tag;

/**
 * @brief Descriptor of base aggregation operator used in validation report.
 */
struct empty_report_aggregation
{
    using hana_tag=report_aggregation_tag;

    /**
     * @brief Constructor.
     * @param aggregation Aggregation operation.
     * @param member Member the operation is applied to.
     */
    empty_report_aggregation(
            aggregation_op aggregation,
            std::string member=std::string()
        ) : aggregation(std::move(aggregation)),
            member(std::move(member)),
            single(true),
            any_all_count(
                static_cast<size_t>(aggregation.id==aggregation_id::ANY || aggregation.id==aggregation_id::ALL)
            ),
            parts_count(0)
    {}

    aggregation_op aggregation;

    std::string member;

    bool single;
    size_t any_all_count;

    int parts_count;
};


/**
 * @brief Descriptor of aggregation operator used in validation report.
 */
template <typename DstT>
struct report_aggregation : public empty_report_aggregation
{
    using empty_report_aggregation::empty_report_aggregation;

    std::vector<DstT> parts;
};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_report_aggregation_HPP
