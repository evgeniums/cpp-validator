/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/operand_formatter.hpp
*
* Defines wrapper for concrete phrases.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CONCRETE_PHRASE_HPP
#define DRACOSHA_VALIDATOR_CONCRETE_PHRASE_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/grammar_categories.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct concrete_phrase_tag;

/**
 * @brief Final result of text processing that must not be altered any more.
 *
 * Concrete phrase contains text of the phrase and bitmask of grammatical categories of the phrase actual for current locale.
 */
class concrete_phrase
{
    public:

        using hana_tag=concrete_phrase_tag;

        /**
         * @brief Default constructor.
         */
        concrete_phrase(
            ) : _grammar_cats(0),
                _empty(true)
        {}

        /**
         * @brief Constructor.
         * @param text Text of the phrase.
         */
        concrete_phrase(
                std::string text
            ) : _text(std::move(text)),
                _grammar_cats(0),
                _empty(false)
        {}

        /**
         * @brief Constructor.
         * @param text Text of the phrase.
         * @param grammar_cats Bitmask of grammatical categories of the phrase.
         */
        concrete_phrase(
                std::string text,
                grammar_categories grammar_cats
            ) : _text(std::move(text)),
                _grammar_cats(grammar_cats),
                _empty(false)
        {}

        /**
         * @brief Constructor.
         * @param phrase Other phrase to construct from.
         * @param grammar_cats Bitmask of grammatical categories of the phrase.
         */
        concrete_phrase(
                concrete_phrase&& phrase,
                grammar_categories grammar_cats
            ) : _text(std::move(phrase._text)),
                _grammar_cats(grammar_cats),
                _empty(false)
        {}

        /**
         * @brief Constructor.
         * @param text Text of the phrase.
         * @param grammar_cat Grammatic category of the phrase.
         */
        template <typename T>
        concrete_phrase(
                std::string text,
                T grammar_cat
            ) : _text(std::move(text)),
                _grammar_cats(grammar_category<T>.bit(grammar_cat)),
                _empty(false)
        {}

        /**
         * @brief Constructor.
         * @param text Other phrase to construct from.
         * @param grammar_cats Grammatical categories of the phrase.
         */
        template <typename T>
        concrete_phrase(
                std::string text,
                const std::initializer_list<T>& grammar_cats
            ) : _text(std::move(text)),
                _grammar_cats(grammar_category<T>.bits(grammar_cats)),
                _empty(false)
        {}

        /**
         * @brief Constructor.
         * @param text Other phrase to construct from.
         * @param grammar_cats grammatical categories of the phrase.
         */
        template <typename ... GrammarCats>
        concrete_phrase(
                std::string text,
                GrammarCats&&... grammar_cats
            ) : _text(std::move(text)),
                _grammar_cats(grammar_category<
                                std::decay_t<typename std::tuple_element<0,std::tuple<GrammarCats...>>::type>
                              >.bits(std::forward<GrammarCats>(grammar_cats)...)),
                _empty(false)
        {}

        /**
         * @brief Get grammatical categories of the phrase.
         * @return Bitmask of grammatical categories.
         */
        grammar_categories grammar_cats() const noexcept
        {
            return _grammar_cats;
        }

        /**
         * @brief Get text of the phrase.
         * @return Text.
         */
        std::string text() const
        {
            return _text;
        }

        /**
         * @brief Convert the phrase to string.
         * @return Text.
         */
        operator std::string() const
        {
            return _text;
        }

        /**
         * @brief Set text of the phrase.
         * @param text Text.
         */
        void set_text(std::string text)
        {
            _text=std::move(text);
            _empty=false;
        }

        /**
         * @brief Set grammatical categories of the phrase.
         * @param grammar_cats Bitmask of grammatical categories.
         */
        void set_grammar_cats(grammar_categories grammar_cats) noexcept
        {
            _grammar_cats=grammar_cats;
            _empty=false;
        }

        /**
         * @brief Friend operator << for phrase formatting to ostream.
         * @param os Output stream.
         * @param ph Phrase.
         * @return Output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const concrete_phrase& ph)
        {
            return os << ph.text();
        }

        /**
         * @brief Check if phrase is not set.
         * @return Flag.
         */
        bool empty() const noexcept
        {
            return _empty;
        }

        /**
         * @brief Clear phrase.
         */
        void clear()
        {
            _text.clear();
            _grammar_cats=0;
            _empty=true;
        }

    private:

        std::string _text;
        grammar_categories _grammar_cats;
        bool _empty;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONCRETE_PHRASE_HPP
