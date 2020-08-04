/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/format_bool_operand.hpp
*
* Defines helper for formatting of boolean operands.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_BOOL_OPERAND_HPP
#define DRACOSHA_VALIDATOR_FORMAT_BOOL_OPERAND_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/format_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String presentation of boolean true
 */
struct string_true_t : public enable_to_string<string_true_t>
{
    constexpr static const char* description="true";
};
constexpr string_true_t string_true{};

/**
 * @brief String presentation of boolean false
 */
struct string_false_t : public enable_to_string<string_false_t>
{
    constexpr static const char* description="false";
};
constexpr string_false_t string_false{};

/**
 * @brief  Formatter of boolean operands
 */
template <typename T>
struct format_operand_t<T,hana::when<std::is_same<bool,std::decay_t<T>>::value>>
{
    template <typename TraitsT, typename T1>
    concrete_phrase operator () (const TraitsT& traits, T1&& val, grammar_categories cats, bool postprocess) const
    {
        auto phrase=val?std::string(string_true):std::string(string_false);
        if (!postprocess)
        {
            return phrase;
        }
        return decorate(traits,translate(traits,std::move(phrase),cats));
    }

    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const
    {
        auto phrase=val?std::string(string_true):std::string(string_false);
        return decorate(traits,translate(traits,std::move(phrase),cats));
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_BOOL_OPERAND_HPP
