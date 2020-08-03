/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/format_operand.hpp
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

struct string_true_t : public enable_to_string<string_true_t>
{
    constexpr static const char* description="true";
};
constexpr string_true_t string_true{};
struct string_false_t : public enable_to_string<string_false_t>
{
    constexpr static const char* description="false";
};
constexpr string_false_t string_false{};

namespace detail
{

/**
 * @brief Helper for operands formatting
 */
template <typename T, typename =hana::when<true>>
struct format_operand_t
{
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT&, T1&& val, grammar_categories) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(val));
    }
};

/**
 * @brief  Formatter of operands from which the std::string can be constructed
 */
template <typename T>
struct format_operand_t<T,hana::when<
            std::is_constructible<std::string,T>::value
            ||
            std::is_same<concrete_phrase,std::decay_t<T>>::value
        >>
{
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
 * @brief  Formatter of boolean values
 */
template <typename T>
struct format_operand_t<T,hana::when<std::is_same<bool,std::decay_t<T>>::value>>
{
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const -> decltype(auto)
    {
        return decorate(traits,translate(traits,val?std::string(string_true):std::string(string_false),cats));
    }
};

/**
 * @brief Default formatter of values
 */
template <typename T>
constexpr format_operand_t<T> format_operand{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_OPERAND_HPP
