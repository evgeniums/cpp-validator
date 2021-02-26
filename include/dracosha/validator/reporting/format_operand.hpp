/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/format_operand.hpp
*
* Defines helper for operands formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_OPERAND_HPP
#define DRACOSHA_VALIDATOR_FORMAT_OPERAND_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/translate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default helper for operands formatting that returns operand as is.
 */
template <typename T, typename =hana::when<true>>
struct format_operand_t
{
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT&, T1&& val, grammar_categories, bool postprocess=true) const -> decltype(auto)
    {
        std::ignore=postprocess;
        return hana::id(std::forward<T1>(val));
    }
};

/**
 * @brief Formatter of operands from which the std::string can be constructed.
 *
 * Formatter uses input string and then translates and decorates it
 * if "postprocess" argument is not false.
 */
template <typename T>
struct format_operand_t<T,hana::when<
            std::is_constructible<std::string,T>::value
            ||
            std::is_same<concrete_phrase,std::decay_t<T>>::value
        >>
{
    /**
     * @brief Format operand with optional postprocessing.
     * @param traits Formatter traits.
     * @param val Operand value.
     * @param postprocess If true then try to translate and decorate string after formatting.
     * @return Formatted string.
     */
    template <typename TraitsT, typename T1>
    concrete_phrase operator () (const TraitsT& traits, T1&& val, grammar_categories cats, bool postprocess) const
    {
        auto phrase=std::string(std::forward<T1>(val));
        if (!postprocess)
        {
            return phrase;
        }
        if (TraitsT::translate_string_operands && !std::is_same<concrete_phrase,std::decay_t<T>>::value)
        {
            phrase=translate(traits,std::move(phrase),cats);
        }
        return decorate(traits,std::move(phrase));
    }

    /**
     * @brief Format operand with post processing.
     * @param traits Formatter traits.
     * @param val Operand value.
     * @return Formatted string.
     */
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const -> decltype(auto)
    {
        auto phrase=std::string(std::forward<T1>(val));
        if (TraitsT::translate_string_operands && !std::is_same<concrete_phrase,std::decay_t<T>>::value)
        {
            phrase=translate(traits,std::move(phrase),cats);
        }
        return decorate(traits,std::move(phrase));
    }
};

/**
 * @brief Default formatter of operands.
 */
template <typename T>
constexpr format_operand_t<T> format_operand{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_OPERAND_HPP
