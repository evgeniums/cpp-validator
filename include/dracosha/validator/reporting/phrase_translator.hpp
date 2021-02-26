/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/phrase_translator.hpp
*
*   Defines translator that translates phrases awaring of grammar categories.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PHRASE_TRANSLATOR_HPP
#define DRACOSHA_VALIDATOR_PHRASE_TRANSLATOR_HPP

#include <map>
#include <vector>
#include <algorithm>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/grammar_categories.hpp>
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Pair of concrete phrase and grammatical categories of preceding phrase this phrase is suitable for.
 */
struct phrase_and_grammar_cats
{
    /**
     * @brief Constructor.
     * @param phrase Text of the phrase.
     * @param grammar_cats Grammatical categories of preceding phrase this phrase is suitable for.
     */
    template <typename ... GrammarCats>
    phrase_and_grammar_cats(
            const char* phrase,
            GrammarCats&&... grammar_cats
        ) : phrase(phrase),
            categories(grammar_categories_bitmask(std::forward<GrammarCats>(grammar_cats)...))
    {}

    /**
     * @brief Constructor.
     * @param phrase Phrase.
     * @param grammar_cats Grammatical categories of preceding phrase this phrase is suitable for.
     */
    template <typename ... GrammarCats>
    phrase_and_grammar_cats(
            concrete_phrase phrase,
            GrammarCats&&... grammar_cats
        ) : phrase(std::move(phrase)),
            categories(grammar_categories_bitmask(std::forward<GrammarCats>(grammar_cats)...))
    {}

    /**
     * @brief Constructor.
     * @param phrase Phrase.
     * @param grammar_cats Grammatical categories of preceding phrase this phrase is suitable for.
     */
    phrase_and_grammar_cats(
            concrete_phrase phrase,
            grammar_categories grammar_cats
        ) : phrase(std::move(phrase)),
            categories(grammar_cats)
    {}

    concrete_phrase phrase;
    grammar_categories categories;
};

//-------------------------------------------------------------

/**
 * @brief Translator that is aware of grammatical categories.
 *
 * Each string in translator can be bound to two types of grammatical categories:
 *  - grammatical categories of preceding phrase that should be use to select current phrase;
 *  - grammatical categories of this phrase that should be used for translation of successive phrase.
 *
 * Gramattical categories of the latter type are stored within concrete_phrase. Grammatical categories of the former type
 * are used as selectors of the most suitable phrase translation of given string in the phrase_translator.
 * Translator will select the phrase with the maximum number of matching grammatical categories of the former type.
 *
 * @see {validator_translator_sample()} for examples.
 */
class phrase_translator : public translator
{
    public:

        using container_type=std::map<std::string,std::vector<phrase_and_grammar_cats>>;

        phrase_translator()=default;

        /**
         * @brief Constructor.
         * @param phrases Translated strings.
         */
        phrase_translator(container_type phrases):_phrases(std::move(phrases))
        {}

        /**
         * @brief Reset map.
         */
        virtual void reset() override
        {
            _phrases.clear();
        }

        /**
         * @brief Translate a string.
         * @param id String id.
         * @param cats Grammar categories to look for.
         * @return Translated string or id if such string not found.
         *
         * Translator will select the phrase with the maximum number of matching grammatical categories
         * (cats match phrase_and_grammar_cats.categories).
         *
         */
        virtual translation_result translate(const std::string& id, grammar_categories cats=0) const override
        {
            auto it=_phrases.find(id);
            if (it!=_phrases.end())
            {
                if (cats==0)
                {
                    return translation_result{it->second.front().phrase,true};
                }
                // find element with max number of matching categories
                auto max_el=std::max_element(it->second.begin(),it->second.end(),
                                 [&cats](const phrase_and_grammar_cats& left, const phrase_and_grammar_cats& right)
                                 {
                                    auto l=count_grammar_categories(left.categories,cats);
                                    auto r=count_grammar_categories(right.categories,cats);
                                    return l<r;
                                 }
                            );
                return translation_result{max_el->phrase,true};
            }
            return translation_result{id,false};
        }

        /**
         * @brief Override operator [] for filling the translator.
         * @param key Translatable string.
         * @return phrase_translator_setter used to put translations of the given string to translator.
         */
        template <typename T>
        auto operator [] (T&& key);

        /**
         * @brief Check if translator is empty.
         * @return Boolean flag.
         */
        bool empty() const noexcept
        {
            return _phrases.empty();
        }

    private:

        container_type _phrases;
};

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Helper of phrase translator setter
 */
template <typename T, typename =hana::when<true>>
struct phrase_translator_setter_helper_t
{
};

/**
 * @brief Helper of phrase translator setter for case when phrase_and_grammar_cats is constructible with input type
 */
template <typename T>
struct phrase_translator_setter_helper_t<
            T,
            hana::when<std::is_constructible<phrase_and_grammar_cats,T>::value>
        >
{
    auto operator () (T&& arg) const
    {
        return std::vector<phrase_and_grammar_cats>{phrase_and_grammar_cats(std::forward<T>(arg))};
    }
};

/**
 * @brief Helper of phrase translator setter for case when phrase_and_grammar_cats is initializer_list
 */
template <typename T>
struct phrase_translator_setter_helper_t<
            T,
            hana::when<std::is_same<std::initializer_list<phrase_and_grammar_cats>,T>::value>
        >
{
    auto operator () (T&& arg) const
    {
        return std::vector<phrase_and_grammar_cats>{std::forward<T>(arg)};
    }
};

template <typename T>
constexpr phrase_translator_setter_helper_t<T> phrase_translator_setter_helper{};

}

/**
 * @brief Setter of phrase translation
 */
struct phrase_translator_setter
{
    std::map<std::string,std::vector<phrase_and_grammar_cats>>::iterator it;

    /**
     * @brief Default assignment operator
     */
    template <typename T>
    phrase_translator_setter& operator = (T&& arg)
    {
        it->second=detail::phrase_translator_setter_helper<T>(std::forward<T>(arg));
        return *this;
    }

    /**
     * @brief Assignment from initializer_list
     */
    phrase_translator_setter& operator = (std::initializer_list<phrase_and_grammar_cats>&& arg)
    {
        it->second=detail::phrase_translator_setter_helper<std::initializer_list<phrase_and_grammar_cats>>(std::move(arg));
        return *this;
    }
};

template <typename T>
auto phrase_translator::operator [] (T&& key)
{
    auto item=_phrases.emplace(std::forward<T>(key),std::vector<phrase_and_grammar_cats>());
    return phrase_translator_setter{std::move(item.first)};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PHRASE_TRANSLATOR_HPP
