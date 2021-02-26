/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/phrase_grammar_cats.hpp
*
* Defines Helpers for working with grammar categories of phrases.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PHRASE_GRAMMAR_CATS_HPP
#define DRACOSHA_VALIDATOR_PHRASE_GRAMMAR_CATS_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/grammar_categories.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
/**
 * @brief Default helper for working with grammar categories of a phrase in case the type is neither grammar_categories nor a concrete phrase.
 */
template <typename T, typename = hana::when<true>>
struct phrase_grammar_cats_t
{
    template <typename T1>
    constexpr grammar_categories operator() (T1&&) const
    {
        return 0;
    }

    template <typename T1>
    void set(T1&, grammar_categories) const
    {
    }
};

/**
 * @brief Helper for working with grammar categories of a phrase in case the type is grammar_categories.
 */
template <typename T>
struct phrase_grammar_cats_t<
            T,
            hana::when<std::is_same<grammar_categories,std::decay_t<T>>::value>
        >
{
    /**
     * @brief Get grammatical categories.
     * @param grammar_cats Grammar categories to return.
     * @return Input grammar_cats.
     */
    template <typename T1>
    auto operator() (T1&& grammar_cats) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(grammar_cats));
    }

    template <typename T1>
    void set(T1&, grammar_categories) const
    {
    }
};

/**
 * @brief Helper for working with grammar categories of a phrase in case the type is a concrete_phrase.
 */
template <typename T>
struct phrase_grammar_cats_t<
            T,
            hana::when<std::is_same<concrete_phrase,std::decay_t<T>>::value>
        >
{
    /**
     * @brief Get grammatical categories from concrete_phrase.
     * @param phrase Phrase.
     * @return Grammatical categories of the phrase.
     */
    template <typename T1>
    grammar_categories operator() (T1&& phrase) const
    {
        return phrase.grammar_cats();
    }

    /**
     * @brief Set grammatical categories in concrete_phrase.
     * @param phrase Phrase.
     * @param cats Grammatical categories to set.
     */
    template <typename T1>
    void set(T1& phrase, grammar_categories cats) const
    {
        phrase.set_grammar_cats(cats);
    }
};

/**
 * Template instance of helper for working with grammar categories.
 */
template <typename T>
constexpr phrase_grammar_cats_t<T> phrase_grammar_cats_inst{};
}

/**
 * @brief Get grammatical categories of a phrase.
 * @param phrase Phrase.
 * @return Grammatical categories of the phrase if applicabe or 0.
 */
template <typename T>
auto phrase_grammar_cats(T&& phrase) -> decltype(auto)
{
    return detail::phrase_grammar_cats_inst<T>(std::forward<T>(phrase));
}

/**
 * @brief Set grammatical categories in a phrase if applicable.
 * @param phrase Phrase.
 * @param cats Grammatical categories to set.
 */
template <typename T>
void set_phrase_grammar_cats(T& phrase, grammar_categories cats)
{
    detail::phrase_grammar_cats_inst<T>.set(phrase,cats);
}

/**
 * @brief Get grammatical categories of the last phrase in tuple if applicable.
 * @param ts Tuple.
 * @param fallback Fallback grammatical categories to use if operation is not applicable.
 * @return Grammatical categories of the last phrase in the tuple if applicable or fallback otherwise.
 */
template <typename Ts, typename T>
auto last_grammar_categories(Ts&& ts, T&& fallback)
{
    return hana::if_(
        hana::is_empty(ts),
        [&fallback](auto &&) {return phrase_grammar_cats(fallback);},
        [](auto&& ts) {return phrase_grammar_cats(hana::back(ts));}
    )(std::forward<Ts>(ts));
}

/**
 * @brief Get grammatical categories of the first phrase in tuple if applicable.
 * @param ts Tuple.
 * @param fallback Fallback grammatical categories to use if operation is not applicable.
 * @return Grammatical categories of the first phrase in the tuple if applicable or fallback otherwise.
 */
template <typename Ts, typename T>
auto first_grammar_categories(Ts&& ts, T&& fallback)
{
    return hana::if_(
        hana::is_empty(ts),
        [&fallback](auto &&) {return phrase_grammar_cats(fallback);},
        [](auto&& ts) {return phrase_grammar_cats(hana::front(ts));}
    )(std::forward<Ts>(ts));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PHRASE_GRAMMAR_CATS_HPP
