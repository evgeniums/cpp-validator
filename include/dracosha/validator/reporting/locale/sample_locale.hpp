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
#include <dracosha/validator/reporting/values.hpp>

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
    m[string_empty]="is empty";
    m[string_not_empty]="is not empty";

    // properties
    m[value.name()]="value";
    m[empty.name()]="empty";
    m[size.name()]="size";
    m[length.name()]="length";

    // existance
    m[string_exists]="exists";
    m[string_not_exists]="does not exist";

    // logical
    m[string_and]="some of the following conditions are not met";
    m[string_or]="none of the following conditions are met";
    m[string_not]="the following condition must be false";

    // comparison
    m[eq]="is equal to";
    m[ne]="is not equal to";
    m[lt]="is less than";
    m[lte]="is less than or equal to";
    m[gt]="is greater than";
    m[gte]="is greater than or equal to";

    return m;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SAMPLE_LOCALE_HPP
