/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/aggregation_strings.hpp
*
*  Defines description helpers for aggregation operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATION_STRINGS_HPP
#define DRACOSHA_VALIDATOR_AGGREGATION_STRINGS_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/aggregation/aggregation.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct aggregation_strings_tag;

/**
 * @brief Helpers for aggregation strings.
 *
 * The helpers are used by strings object in formatting of aggreagation operators.
 * If customization of such strings is needed then another aggregation strings traits must be implemented
 * with the same methods signatures as in aggregation_strings_t.
 */
struct aggregation_strings_t
{
    using hana_tag=aggregation_strings_tag;

    /**
     * @brief Get token for opening aggregation operator.
     * @param aggregation Descriptor of aggregation operation.
     * @return Opening token.
     */
    static std::string open(const aggregation_op& aggregation)
    {
        return aggregation.open_token;
    }

    /**
     * @brief Get token for closing aggregation operator.
     * @param aggregation Descriptor of aggregation operation.
     * @return Closing token.
     */
    static std::string close(const aggregation_op& aggregation)
    {
        return aggregation.close_token;
    }

    /**
     * @brief Get token for joining child validators within aggregation operator.
     * @param aggregation Descriptor of aggregation operation.
     * @return Separator token for join operation.
     */
    static std::string conjunction(const aggregation_op& aggregation)
    {
        return aggregation.conjunction_token;
    }
};
constexpr aggregation_strings_t aggregation_strings{};

//-------------------------------------------------------------

/**
 * @brief String descriptions helper for conjunction "of".
 */
struct string_conjunction_of_t : public enable_to_string<string_conjunction_of_t>
{
    constexpr static const char* description="of";
};

/**
 * @brief Instance of string descriptions helper for conjunction "of".
 */
constexpr string_conjunction_of_t string_conjunction_of{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_STRINGS_HPP
