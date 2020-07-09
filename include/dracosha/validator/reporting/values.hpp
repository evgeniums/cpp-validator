/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/values.hpp
*
* Defines traits for value formatting.
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
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/detail/format_value.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct values_tag;

/**
 * @brief Traits for value formatting.
 *
 * Sometimes values require special formatting.
 * The obvious example is a boolean value which can be displayed
 * in different ways, e.g. as 1/0, true/false, yes/not, checked/unchecked, etc.
 * Also values can be decorated, e.g. with font or HTML tags.
 *
 * Default implementation of value formatting handles only boolean types in special manner,
 * all the rest types are bypassed "as is". No special decorating is used.
 */
template <typename TranslatorT, typename DecoratorT>
struct values
{
    using hana_tag=values_tag;

    using translator_type=TranslatorT;

    TranslatorT _translator;
    DecoratorT _decorator;

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
        return detail::format_value<T>(std::forward<T>(val),_translator,_decorator);
    }
};

/**
  @brief Default values formatter does not perform any translation.
*/
constexpr values<const no_translator_t&,const no_decorator_t&> default_values{no_translator,no_decorator};

/**
 * @brief Make values
 * @param decorator Decorator to decorate values
 * @return Values formatter
 */
template <typename DecoratorT=const no_decorator_t&>
inline auto make_values(DecoratorT&& decorator=no_decorator)
{
    return values<const no_translator_t&,DecoratorT>{no_translator,std::forward<DecoratorT>(decorator)};
}

template <typename DecoratorT>
using translated_values=values<const translator&,DecoratorT>;

/**
 * @brief Make values formatter using translator found in translator repository by locale name
 * @param rep Translator repository
 * @param loc Locale name
 * @param decorator Decorator to decorate values after translating
 * @return Values formatter with translator for given locale name from the repository
 */
template <typename DecoratorT=const no_decorator_t&>
inline auto make_translated_values(const translator_repository& rep,const std::string& loc=std::locale().name(),
                                   DecoratorT&& decorator=no_decorator
                                   )
{
    return translated_values<DecoratorT>{*rep.find_translator(loc),std::forward<DecoratorT>(decorator)};
}

/**
 * @brief Make values formatter using a given translator
 * @param tr Translator to use
 * @param decorator Decorator to decorate values after translating
 * @return Values formatter
 */
template <typename DecoratorT=const no_decorator_t&>
inline auto make_translated_values(const translator& tr,DecoratorT&& decorator=no_decorator)
{
    return translated_values<DecoratorT>{tr,std::forward<DecoratorT>(decorator)};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALUES_HPP
