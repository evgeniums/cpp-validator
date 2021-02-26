/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/translate.hpp
*
* Defines helper for translating using object that can optionally have or not have translators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TRANSLATE_HPP
#define DRACOSHA_VALIDATOR_TRANSLATE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/grammar_categories.hpp>
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper for translating strings with object that can optionally have or not have translator.
 *
 * Version for object that does not have translator.
 */
template <typename T, typename Arg, typename =void>
struct translate_t
{
    template <typename T1, typename Arg1>
    constexpr auto operator() (T1&&, Arg1&& arg, grammar_categories) const -> decltype(auto)
    {
        return hana::id(std::forward<Arg1>(arg));
    }
};

/**
 * @brief Helper for translating strings with object that can optionally have or not have translator
 *
 * Version for object that has translator.
 */
template <typename T, typename Arg>
struct translate_t<T,Arg,
        decltype(
            (void)std::declval<std::decay_t<T>>().translator(std::declval<std::decay_t<Arg>>(),std::declval<grammar_categories>())
        )>
{
    template <typename T1, typename Arg1>
    auto operator() (T1&& obj, Arg1&& arg, grammar_categories grammar_cats=0) const -> decltype(auto)
    {
        return obj.translator(std::forward<Arg1>(arg),grammar_cats);
    }
};

/**
 * @brief Instance of helper for translating strings with object that can optionally have or not have translator.
 */
template <typename T, typename Arg>
constexpr translate_t<T,Arg> translate_inst{};

/**
 * @brief Translate a phrase.
 * @param obj Object that can have or not have a translator.
 * @param phrase Phrase to translate if applicable.
 * @param
 * @return If object has translator then translated phrase or phrase "as is" otherwise.
 */
template <typename T, typename PhraseT>
constexpr auto translate(T&& obj, PhraseT&& phrase, grammar_categories grammar_cats=0) -> decltype(auto)
{
    return translate_inst<T,PhraseT>(std::forward<T>(obj),std::forward<PhraseT>(phrase),grammar_cats);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRANSLATE_HPP
