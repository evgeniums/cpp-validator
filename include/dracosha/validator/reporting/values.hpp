/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/values.hpp
*
* Defines helper for value formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALUES_HPP
#define DRACOSHA_VALIDATOR_VALUES_HPP

#include <string>
#include <locale>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/translator.hpp>
#include <dracosha/validator/reporting/no_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/detail/format_value.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct values_tag;

/**
 * @brief Helper for value formatting.
 *
 * Sometimes values require special formatting.
 * The obvious example is a boolean value which can be displayed
 * in different ways, e.g. as 1/0, true/false, yes/not, checked/unchecked, etc.
 *
 * Default implementation of value formatting handles only boolean types in special manner,
 * all the rest types are bypassed "as is" without changing the original type.
 */
template <typename TranslatorT>
struct values
{
    using hana_tag=values_tag;

    const TranslatorT& _translator;

    /**
     * @brief Format a value
     * @param val Value to be formatted
     * @return Formatted value or original value "as is"
     *
     * Note, that return type can be either a string or the same as input type.
     */
    template <typename T>
    auto operator () (T&& val) const -> decltype(auto)
    {
        return detail::format_value<T>(std::forward<T>(val),_translator);
    }
};

/**
  @brief Default values formatter does not perform any translation.
*/
constexpr values<no_translator_t> default_values{no_translator};

using translated_values=values<translator>;

/**
 * @brief Make values formatter using translator found in translator repository by locale name
 * @param rep Translator repository
 * @param loc Locale name
 * @return Values formatter with translator for given locale name from the repository
 */
inline translated_values make_translated_values(const translator_repository& rep,const std::string& loc=std::locale().name())
{
    return translated_values{*rep.find_translator(loc)};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALUES_HPP
