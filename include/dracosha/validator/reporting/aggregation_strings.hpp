/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/aggregation_strings.hpp
*
*  Defines aggregation strings formatter
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATION_STRINGS_HPP
#define DRACOSHA_VALIDATOR_AGGREGATION_STRINGS_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/aggregation.hpp>
#include <dracosha/validator/reporting/member_name.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct aggregation_strings_tag;

struct aggregation_strings_t
{
    using hana_tag=aggregation_strings_tag;

    static std::string open(const aggregation_op& aggregation)
    {
        return aggregation.open_token;
    }

    static std::string close(const aggregation_op& aggregation)
    {
        return aggregation.close_token;
    }

    static std::string conjunction(const aggregation_op& aggregation)
    {
        return aggregation.conjunction_token;
    }

    static std::string member_names_conjunction()
    {
        return string_member_name_conjunction;
    }
};
constexpr aggregation_strings_t aggregation_strings{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_STRINGS_HPP
