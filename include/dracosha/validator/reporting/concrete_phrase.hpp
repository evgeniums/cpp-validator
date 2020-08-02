/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/operand_formatter.hpp
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

        concrete_phrase(
            ) : _grammar_cats(0),
                _empty(true)
        {}

        /**
         * @brief Conctructor
         * @param text Text of the phrase
         */
        concrete_phrase(
                std::string text
            ) : _text(std::move(text)),
                _grammar_cats(0),
                _empty(false)
        {}

        /**
         * @brief Conctructor
         * @param text Text of the phrase
         * @param grammar_cats Bitmask of grammatical categories of the phrase
         */
        concrete_phrase(
                std::string text,
                grammar_categories grammar_cats
            ) : _text(std::move(text)),
                _grammar_cats(grammar_cats),
                _empty(false)
        {}

        /**
         * @brief Conctructor
         * @param phrase Other phrase to construct from
         * @param grammar_cats Bitmask of grammatical categories of the phrase
         */
        concrete_phrase(
                concrete_phrase&& phrase,
                grammar_categories grammar_cats
            ) : _text(std::move(phrase._text)),
                _grammar_cats(grammar_cats),
                _empty(false)
        {}

        /**
         * @brief Conctructor
         * @param text Text of the phrase
         * @param grammar_cat Grammatic category of the phrase
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
         * @brief Conctructor
         * @param text Other phrase to construct from
         * @param grammar_cats Grammatical categories of the phrase
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
         * @brief Conctructor
         * @param text Other phrase to construct from
         * @param grammar_cats grammatical categories of the phrase
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
         * @brief Get grammatical categories of the phrase
         * @return Bitmask of grammatical categories
         */
        grammar_categories grammar_cats() const noexcept
        {
            return _grammar_cats;
        }

        /**
         * @brief Get text of the phrase
         * @return Text
         */
        std::string text() const
        {
            return _text;
        }

        /**
         * @brief Convert the phrase to string
         * @return Text
         */
        operator std::string() const
        {
            return _text;
        }

        /**
         * @brief Set text of the phrase
         * @param Text
         */
        void set_text(std::string text)
        {
            _text=std::move(text);
            _empty=false;
        }

        /**
         * @brief Set grammatical categories of the phrase
         * @param grammar_cats Bitmask of grammatical categories
         */
        void set_grammar_cats(grammar_categories grammar_cats) noexcept
        {
            _grammar_cats=grammar_cats;
            _empty=false;
        }

        /**
         * @brief Friend operator << for phrase formatting to ostream
         * @param os Output stream
         * @param ph Phrase
         * @return Output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const concrete_phrase& ph)
        {
            return os << ph.text();
        }

        bool empty() const noexcept
        {
            return _empty;
        }

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

namespace detail
{
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

template <typename T>
struct phrase_grammar_cats_t<
            T,
            hana::when<std::is_same<grammar_categories,std::decay_t<T>>::value>
        >
{
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

template <typename T>
struct phrase_grammar_cats_t<
            T,
            hana::when<std::is_same<concrete_phrase,std::decay_t<T>>::value>
        >
{
    template <typename T1>
    grammar_categories operator() (T1&& phrase) const
    {
        return phrase.grammar_cats();
    }

    template <typename T1>
    void set(T1& phrase, grammar_categories cats) const
    {
        phrase.set_grammar_cats(cats);
    }
};

template <typename T>
constexpr phrase_grammar_cats_t<T> phrase_grammar_cats_inst{};
}

/**
 * @brief Get phrase grammar_cats
 * @param phrase Phrase
 * @return Phrase grammar_cats if applicabe or 0
 */
template <typename T>
auto phrase_grammar_cats(T&& phrase) -> decltype(auto)
{
    return detail::phrase_grammar_cats_inst<T>(std::forward<T>(phrase));
}

template <typename T>
void set_phrase_grammar_cats(T& phrase, grammar_categories cats)
{
    detail::phrase_grammar_cats_inst<T>.set(phrase,cats);
}

template <typename Ts, typename T>
auto last_grammar_categories(Ts&& ts, T&& fallback)
{
    return hana::if_(
        hana::is_empty(ts),
        [&fallback](auto &&) {return phrase_grammar_cats(fallback);},
        [](auto&& ts) {return phrase_grammar_cats(hana::back(ts));}
    )(std::forward<Ts>(ts));
}

template <typename Ts, typename T>
auto first_grammar_categories(Ts&& ts, T&& fallback)
{
    return hana::if_(
        hana::is_empty(ts),
        [&fallback](auto &&) {return phrase_grammar_cats(fallback);},
        [](auto&& ts) {return phrase_grammar_cats(hana::front(ts));}
    )(std::forward<Ts>(ts));
}

template <typename Ts>
void set_last_grammar_categories(Ts&& ts, grammar_categories cats)
{
    return hana::if_(
        hana::is_empty(ts),
        [&](auto &&) {},
        [&cats](auto&& ts) {return set_phrase_grammar_cats(hana::back(ts),cats);}
    )(std::forward<Ts>(ts));
}

//-------------------------------------------------------------

template <typename T, typename=hana::when<true>>
struct adjust_storable_concrete_phrase
{
};

template <typename T>
struct adjust_storable_concrete_phrase<T,
                        hana::when<std::is_same<concrete_phrase,std::decay_t<T>>::value>
                    >
{
    using type=concrete_phrase;
};

template <typename T>
struct adjust_storable_concrete_phrase<T,
                        hana::when<!std::is_same<concrete_phrase,std::decay_t<T>>::value>
                    >
{
    using type=std::decay_t<T>;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONCRETE_PHRASE_HPP
