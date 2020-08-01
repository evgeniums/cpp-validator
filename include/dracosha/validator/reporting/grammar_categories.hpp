/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/grammar_categories.hpp
*
* Defines grammatical categories of words and phrases
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GRAMMAR_CATEGORIES_HPP
#define DRACOSHA_VALIDATOR_GRAMMAR_CATEGORIES_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/make_types_tuple.hpp>
#include <dracosha/validator/utils/feature_bitmask.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Grammatical category of words and phrases.
 *
 * A grammatical category is a property of items within the grammar of a language (see https://en.wikipedia.org/wiki/Grammatical_category).
 * In some languages different grammatical categories of some words can affect the forms of the successive words in a phrase.
 * To construct a translated phrase properly one needs to use such a form of a word or a subphrase that corresponds to grammatical categories
 * of the preceding words or subphrases in the phrase.
 * A enum of grammatic catigories can have up to 32 categories to be used with a word.
 * Default grammar enum defines only a few most common grammatical categories leaving the rest categories reserved.
 * For custom set of grammatical categories a custom enum must be defined that can be used with validator API instead of the default enum.
 */
template <typename T>
constexpr feature_bitmask_t<T> grammar_category{};

/**
 * @brief Default set of grammatical categories
 */
enum class grammar : uint8_t
{
    neuter, //!< neuter gender
    feminine, //!< feminine gender
    masculine, //!< masculine gender

    // items from 3 to 15 are reserved

    plural=16, //!< plural form

    END_=32 //!< special end tag
};


/**
 * @brief Type to hold bitmask of grammatical categories
 */
using grammar_categories=typename feature_bitmask_t<grammar>::bitmask;

constexpr inline size_t count_grammar_categories(grammar_categories cats)
{
    return feature_bitmask_t<grammar>::count(cats);
}

constexpr inline size_t count_grammar_categories(grammar_categories cats, grammar_categories cats_mask)
{
    return feature_bitmask_t<grammar>::count(cats,cats_mask);
}

template <typename ...Args>
grammar_categories grammar_categories_bitmask(Args&&... args)
{
    return feature_bitmask_t<grammar>::bits(std::forward<Args>(args)...);
}

template <typename T>
constexpr inline bool is_grammar_category_set(grammar_categories cats, T cat)
{
    return feature_bitmask_t<grammar>::is_set(cats,cat);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GRAMMAR_CATEGORIES_HPP
