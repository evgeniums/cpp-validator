/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/operand_formatter.hpp
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
#include <dracosha/validator/detail/format_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct operand_formatter_tag;

/**
 * @brief Traits for operands formatting.
 *
 * Sometimes operands require special formatting.
 * The obvious example is a boolean value which can be displayed
 * in different ways, e.g. as 1/0, true/false, yes/not, checked/unchecked, etc.
 * Also operands can be decorated, e.g. with quotes or HTML tags.
 *
 * Default implementation of operand formatting handles and decorates only boolean and to-string-convertible types.
 * All the rest types are bypassed "as is" and no special decorating is used.
 */
template <typename TranslatorT, typename DecoratorT>
struct operand_formatter
{
    using hana_tag=operand_formatter_tag;

    using translator_type=TranslatorT;

    TranslatorT _translator;
    DecoratorT _decorator;

    /**
     * @brief Format operand
     * @param val Operand to be formatted
     * @return Formatted operand or original operand "as is"
     *
     * Note, that return type can be either a string or the same as input type.
     */
    template <typename T>
    auto operator () (T&& val, word_attributes=0) const -> decltype(auto)
    {
        return detail::format_operand<T>(std::forward<T>(val),_translator,_decorator);
    }
};

/**
  @brief Default operand formatter does not perform any translation or decoration.
*/
constexpr operand_formatter<const no_translator_t&,const no_decorator_t&> default_operand_formatter{no_translator,no_decorator};

/**
 * @brief Make operand formatter
 * @param decorator Decorator to decorate operands
 * @return Operands formatter
 */
template <typename DecoratorT=const no_decorator_t&>
inline auto make_operand_formatter(DecoratorT&& decorator=no_decorator)
{
    return operand_formatter<const no_translator_t&,DecoratorT>{no_translator,std::forward<DecoratorT>(decorator)};
}

template <typename DecoratorT>
using translated_operand_formatter=operand_formatter<const translator&,DecoratorT>;

/**
 * @brief Make operands formatter using translator found in translator repository by locale name
 * @param rep Translator repository
 * @param loc Locale name
 * @param decorator Decorator to decorate operands after translating
 * @return Operands formatter with translator for given locale name from the repository
 */
template <typename DecoratorT=const no_decorator_t&>
inline auto make_translated_operand_formatter(const translator_repository& rep,const std::string& loc=std::locale().name(),
                                   DecoratorT&& decorator=no_decorator
                                   )
{
    return translated_operand_formatter<DecoratorT>{*rep.find_translator(loc),std::forward<DecoratorT>(decorator)};
}

/**
 * @brief Make operands formatter using a given translator
 * @param tr Translator to use
 * @param decorator Decorator to decorate operands after translating
 * @return Operands formatter
 */
template <typename DecoratorT=const no_decorator_t&>
inline auto make_translated_operand_formatter(const translator& tr,DecoratorT&& decorator=no_decorator)
{
    return translated_operand_formatter<DecoratorT>{tr,std::forward<DecoratorT>(decorator)};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERAND_FORMATTER_HPP
