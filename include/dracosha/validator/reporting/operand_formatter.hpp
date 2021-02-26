/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/operand_formatter.hpp
*
* Defines traits for operand formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERAND_FORMATTER_HPP
#define DRACOSHA_VALIDATOR_OPERAND_FORMATTER_HPP

#include <string>
#include <locale>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/translator.hpp>
#include <dracosha/validator/reporting/no_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/format_operand.hpp>
#include <dracosha/validator/reporting/format_bool_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct operand_formatter_tag;

/**
 * @brief Operands formatter.
 *
 * Sometimes operands require special formatting.
 * The obvious example is a boolean value which can be displayed
 * in different ways, e.g. as 1/0, true/false, yes/not, checked/unchecked, etc.
 * Also operands can be decorated, e.g. with quotes or HTML tags.
 *
 * Default implementation of operand formatting handles and decorates only boolean and to-string-convertible types.
 * All the rest types are bypassed "as is" and no special decorating is used.
 */
template <typename TraitsT>
struct operand_formatter
{
    using hana_tag=operand_formatter_tag;

    TraitsT traits;

    /**
     * @brief Format operand.
     * @param val Operand to be formatted.
     * @return Formatted operand or original operand "as is".
     *
     * Note, that return type can be either a string or the same as input type.
     */
    template <typename T>
    auto operator () (T&& val, grammar_categories cats=0) const -> decltype(auto)
    {
        return format_operand<T>(traits,std::forward<T>(val),cats);
    }
};

/**
 * @brief Traits for default operand formatter.
 */
struct default_operand_formatter_traits
{
    constexpr static const bool translate_string_operands=false;
};

/**
  @brief Default operand formatter that does not perform any translation or decoration.
*/
constexpr operand_formatter<default_operand_formatter_traits> default_operand_formatter{default_operand_formatter_traits{}};

/**
  @brief Operand formatter traits with translator and decorator.

  When TranslateAllStrings is false then only predefined strings will be translated.
  When TranslateAllStrings is true then all strings provided as operands will be translated.
*/
template <typename TranslatorT, typename DecoratorT, bool TranslateAllStrings=true>
struct operand_formatter_traits
{
    using translator_type=TranslatorT;

    constexpr static const bool translate_string_operands=TranslateAllStrings;

    TranslatorT translator;
    DecoratorT decorator;
};

/**
 * @brief Create traits for operand fromatter with decorator.
 * @param decorator Decorator.
 * @return Traits with decorator.
 */
template <typename DecoratorT>
auto make_operand_formatter_traits(DecoratorT&& decorator)
{
    return operand_formatter_traits<const no_translator_t&,DecoratorT,false>{no_translator,std::forward<DecoratorT>(decorator)};
}

/**
 * @brief Create traits for operand fromatter with translator and decorator.
 * @param tr Translator.
 * @param decorator Decorator.
 * @param translate_operands Mode of string operands translation, can by only either std::true_type() or std::false_type().
 * @return Traits with translator and decorator.
 */
template <typename DecoratorT, typename TranslateAllStringsT>
auto make_operand_formatter_traits(const translator& tr, DecoratorT&& decorator, const TranslateAllStringsT& translate_operands)
{
    std::ignore=translate_operands;
    return operand_formatter_traits<const translator&,DecoratorT,TranslateAllStringsT::value>{tr,std::forward<DecoratorT>(decorator)};
}

/**
 * @brief Make operand formatter.
 * @param decorator Decorator to decorate operands.
 * @return Operands formatter.
 */
template <typename DecoratorT>
auto make_operand_formatter(DecoratorT&& decorator)
{
    auto traits=make_operand_formatter_traits(std::forward<DecoratorT>(decorator));
    return operand_formatter<decltype(traits)>{std::move(traits)};
}

/**
 * @brief Make operands formatter using a given translator.
 * @param tr Translator to use.
 * @param decorator Decorator to decorate operands after translating.
 * @param translate_operands Mode of string operands translation, can by only either std::true_type() or std::false_type().
 * @return Operands formatter.
 */
template <typename DecoratorT=const no_decorator_t&, typename TranslateAllStringsT=std::false_type>
auto make_translated_operand_formatter(const translator& tr, DecoratorT&& decorator=no_decorator, const TranslateAllStringsT& translate_operands=std::false_type())
{
    auto traits=make_operand_formatter_traits(tr,std::forward<DecoratorT>(decorator),translate_operands);
    return operand_formatter<decltype(traits)>{std::move(traits)};
}

/**
 * @brief Make operands formatter using translator found in translator repository by locale name.
 * @param rep Translator repository.
 * @param loc Locale name.
 * @param decorator Decorator to decorate operands after translating.
 * @param translate_operands Mode of string operands translation, can by only either std::true_type() or std::false_type().
 * @return Operands formatter with translator for given locale name from the repository.
 */
template <typename DecoratorT=const no_decorator_t&, typename TranslateAllStringsT=std::false_type>
inline auto make_translated_operand_formatter(const translator_repository& rep,const std::string& loc=std::locale().name(),
                                   DecoratorT&& decorator=no_decorator,
                                   const TranslateAllStringsT& translate_operands=std::false_type()
                                   )
{
    return make_translated_operand_formatter(*rep.find_translator(loc),std::forward<DecoratorT>(decorator),translate_operands);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERAND_FORMATTER_HPP
