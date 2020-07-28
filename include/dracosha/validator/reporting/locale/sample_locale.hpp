/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/locale/sample_locale.hpp
*
*   Defines sample file to use for translations of default validator strings to different languages
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP
#define DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP

#include <string>
#include <map>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/operators/invert_op.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/aggregation_strings.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
/**
 * @brief Construct map of translated strings for sample locale.
 * @return Map with translated strings
 *
 * To add new language rename this function like strings_<locale_name> (e.g. strings_de) and replace
 * strings below with corresponding translated strings.
 * For convenience all translations must be in UTF-8 format.
 */
inline std::map<std::string,std::string> strings_sample_locale()
{
    std::map<std::string,std::string> m;

    // specials
    m[string_true]="true";
    m[string_false]="false";
    m[string_master_sample]="sample";
    m[string_empty]="must be empty";
    m[string_not_empty]="must be not empty";
    m[string_conjunction_of]="of";
    m[string_conjunction_aggregate]=": ";
    m[string_member_name_conjunction]=" of ";
    m[string_element]="element #";
    m[string_any]="at least one element";
    m[string_all]="each element";

    // flag descriptions
    m[flag_true_false_t::description]="must be true";
    m[flag_true_false_t::n_description]="must be false";
    m[flag_on_off_t::description]="must be on";
    m[flag_on_off_t::n_description]="must be off";
    m[flag_checked_unchecked_t::description]="must be checked";
    m[flag_checked_unchecked_t::n_description]="must be unchecked";
    m[flag_set_unset_t::description]="must be set";
    m[flag_set_unset_t::n_description]="must be unset";
    m[flag_enable_disable_t::description]="must be enabled";
    m[flag_enable_disable_t::n_description]="must be disabled";

    // properties
    m[value.name()]="value";
    m[empty.name()]="empty";
    m[size.name()]="size";
    m[length.name()]="length";

    // existance
    m[string_exists]="must exist";
    m[string_not_exists]="must not exist";

    // logical
    m[string_and_t::conjunction_token]=" AND ";
    m[string_or_t::conjunction_token]=" OR ";
    m[string_not_t::open_token]="NOT ";
    m[string_invert_op]="NOT";

    // comparison
    m[eq]="must be equal to";
    m[ne]="must be not equal to";
    m[lt]="must be less than";
    m[lte]="must be less than or equal to";
    m[gt]="must be greater than";
    m[gte]="must be greater than or equal to";

    return m;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP
