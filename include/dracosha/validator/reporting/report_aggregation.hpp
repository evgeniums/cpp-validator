/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/report_aggregation.hpp
*
*  Defines report aggregation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_report_aggregation_HPP
#define DRACOSHA_VALIDATOR_report_aggregation_HPP

#include <vector>
#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/aggregation.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct report_aggregation_tag;

template <typename DstT>
struct report_aggregation
{
    using hana_tag=report_aggregation_tag;

    report_aggregation(
            aggregation_op aggregation,
            DstT member=DstT()
        ) : aggregation(std::move(aggregation)),
            member(std::move(member))
    {}

    aggregation_op aggregation;

    std::string member;
    std::vector<DstT> parts;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_report_aggregation_HPP
