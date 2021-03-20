/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/locale/sample_locale.hpp
*
*   Defines sample file to use for translations of strings used in validator library to different languages.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP
#define DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP

#include <mutex>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/operators/lexicographical.hpp>
#include <dracosha/validator/operators/in.hpp>
#include <dracosha/validator/operators/regex.hpp>
#include <dracosha/validator/operators/string_patterns.hpp>
#include <dracosha/validator/operators/number_patterns.hpp>

#include <dracosha/validator/range.hpp>
#include <dracosha/validator/interval.hpp>

#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/aggregation_strings.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>
#include <dracosha/validator/reporting/phrase_translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
/**
 * @brief Make translator for sample locale of phrases defined in validator library.
 * @return Translator of phrases for sample locale
 *
 * To add new language rename this function like translator_of_<locale_name> (e.g. translator_of_de) and replace
 * phrases below with corresponding translated phrases.
 *
 * If translation of a phrase depends on grammatical categories of the
 * previous phrase then add translation with taking into account that dependency as follows:
 * @code
 * m[original]={
 *                  {"default translation"},
 *                  {"translation for grammatical category 1", grammatical_category1},
 *                  {"translation for combination of grammatical category 1 and grammatical category 2", grammatical_category1, grammatical_category2}
 *             };
 * @endcode
 *
 * Example for value and plural values:
 * @code
 *   m[value.name()]={
 *                      {"value"},
 *                      {"values",grammar::plural} // use "values" translation if preceding grammatical category is grammar::plural
 *                   };
 * @endcode
 *
 * If translation of successive phrase can depend on grammatical categories of current translated phrase then translation must be added as follows:
 * @code
 * m[original]={
 *                  {{"default translation",grammatical_category_to_use_for_next_phrase}},
 *                  {{"translation for grammatical_category_to_select_current_translation", grammatical_category_to_use_for_next_phrase}, grammatical_category_to_select_current_translation},
 *                  {
 *                      {
 *                          "translation for combination of grammatical_category_to_select_current_translation1 and grammatical_category_to_select_current_translation2",
 *                          grammatical_category_to_use_for_next_phrase3, grammatical_category_to_use_for_next_phrase4
 *                      },
 *                      grammatical_category_to_select_current_translation1,
 *                      grammatical_category_to_select_current_translation2
 *                  }
 *             };
 * @endcode
 *
 * Example for plural values:
 * @code
 *   m["values"]={
 *                  {{"values",grammar::plural}} // use this grammatical category grammar::plural for next phrase
 *               };
 * @endcode
 *
 * Example for value and plural values:
 * @code
 *   m["value"]={
 *                  {"value"}
 *                  {
 *                      {"values",grammar::plural}, // use grammatical category grammar::plural for next phrase
 *                      grammar::plural // use "values" translation if preceding grammatical category is grammar::plural
 *                  }
 *               };
 * @endcode
 *
 * For convenience all translations must be in UTF-8 format.
 */
inline const phrase_translator& validator_translator_sample()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    static phrase_translator m;
    if (m.empty())
    {
        // specials
        m[string_true]="true"; // "true"
        m[string_false]="false"; // "false"
        m[string_master_sample]="sample"; // "sample"
        m[string_empty]="must be empty"; // "must be empty"
        m[string_not_empty]="must be not empty"; // "must be not empty"
        m[string_conjunction_of]="of"; // "of"
        m[string_member_name_conjunction]=" of "; // " of "
        m[string_element]="element #"; // "element #"
        m[string_any]="at least one element"; // "at least one element"
        m[string_all]="each element"; // "each element"
        m[string_all.base_phrase_str()]="each"; // "each"
        m[string_all.iterator_description_str()]="each iterator"; // "each iterator"
        m[string_all.key_description_str()]="each key"; // "each key"
        m[string_all.tree_description_str()]="each tree node"; // "each tree node"
        m[string_all.name_str()]="ALL"; // "ALL"
        m[string_any.base_phrase_str()]="at least one"; // "at least one"
        m[string_any.iterator_description_str()]="at least one iterator"; // "at least one iterator"
        m[string_any.key_description_str()]="at least one key"; // "at least one key"
        m[string_any.tree_description_str()]="at least one tree node"; // "at least one tree node"
        m[string_any.name_str()]="ANY"; // "ANY"

        // flag descriptions
        m[flag_true_false.str()]="must be true"; // "must be true"
        m[flag_true_false.n_str()]="must be false"; // "must be false"
        m[flag_on_off.str()]="must be on"; // "must be on"
        m[flag_on_off.n_str()]="must be off"; // "must be off"
        m[flag_checked_unchecked.str()]="must be checked"; // "must be checked"
        m[flag_checked_unchecked.n_str()]="must be unchecked"; // "must be unchecked"
        m[flag_set_unset.str()]="must be set"; // "must be set"
        m[flag_set_unset.n_str()]="must be unset"; // "must be unset"
        m[flag_enable_disable.str()]="must be enabled"; // "must be enabled"
        m[flag_enable_disable.n_str()]="must be disabled"; // "must be disabled"

        // properties
        m[value.name()]="value"; // "value"
        m[empty.name()]="empty"; // "empty"
        m[size.name()]="size"; // "size"
        m[length.name()]="length"; // "length"
        m[h_size.name()]="heterogeneous size"; // "heterogeneous size"

        // existance
        m[string_exists]="must exist"; // "must exist"
        m[string_not_exists]="must not exist"; // "must not exist"
        m[contains.str()]="must contain"; // "must contain"
        m[contains.n_str()]="must not contain"; // "must not contain"

        // logical
        m[string_and.conjunction_str()]=" AND "; // " AND "
        m[string_or.conjunction_str()]=" OR "; // " OR "
        m[string_not.open_str()]="NOT "; // "NOT "
        m[string_invert_op]="NOT"; // "NOT"

        // comparison
        m[eq]="must be equal to"; // "must be equal to"
        m[ne]="must be not equal to"; // "must be not equal to"
        m[lt]="must be less than"; // "must be less than"
        m[lte]="must be less than or equal to"; // "must be less than or equal to"
        m[gt]="must be greater than"; // "must be greater than"
        m[gte]="must be greater than or equal to"; // "must be greater than or equal to"

        // lexicographical
        m[lex_starts_with.str()]="must start with"; // "must start with"
        m[lex_starts_with.n_str()]="must not start with"; // "must not start with"
        m[lex_ends_with.str()]="must end with"; // "must end with"
        m[lex_ends_with.n_str()]="must not end with"; // "must not end with"

        // ranges and intervals
        m[range_str]="range"; // "range"
        m[interval_str]="interval"; // "interval"
        m[in]="must be in"; // "must be in"
        m[nin]="must be not in"; // "must be not in"

        // regex and strings
        m[regex_match.str()]="must match expression"; // "must match expression"
        m[regex_match.n_str()]="must not match expression"; // "must not match expression"
        m[regex_contains.str()]="must contain expression"; // "must contain expression"
        m[regex_contains.n_str()]="must not contain expression"; // "must not contain expression"
        m[str_alpha.str()]="must contain only letters and digits"; // "must contain only letters and digits"
        m[str_alpha.n_str()]="must contain not only letters and digits"; // "must contain not only letters and digits"
        m[str_hex.str()]="must be a hexadecimal number"; // "must be a hexadecimal number"
        m[str_hex.n_str()]="must be not a hexadecimal number"; // "must be not a hexadecimal number"
        m[str_int.str()]="must be integer"; // "must be integer"
        m[str_int.n_str()]="must not be integer"; // "must not be integer"
        m[str_float.str()]="must be a floating point number"; // "must be a floating point number"
        m[str_float.n_str()]="must be not a floating point number"; // "must be not a floating point number"
    }

    return m;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP
