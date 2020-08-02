/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/phrase_translator.hpp
*
*   Defines translator that translates phrases awaring of grammar categories
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

struct phrase_and_grammar_cats
{
    template <typename ... GrammarCats>
    phrase_and_grammar_cats(
            const char* phrase,
            GrammarCats&&... grammar_cats
        ) : phrase(phrase),
            categories(grammar_categories_bitmask(std::forward<GrammarCats>(grammar_cats)...))
    {}

    template <typename ... GrammarCats>
    phrase_and_grammar_cats(
            concrete_phrase phrase,
            GrammarCats&&... grammar_cats
        ) : phrase(std::move(phrase)),
            categories(grammar_categories_bitmask(std::forward<GrammarCats>(grammar_cats)...))
    {}

    phrase_and_grammar_cats(concrete_phrase ph, grammar_categories cats)
        : phrase(std::move(ph)),
          categories(cats)
    {}

    concrete_phrase phrase;
    grammar_categories categories;
};

//-------------------------------------------------------------

class phrase_translator : public translator
{
    public:

        using container_type=std::map<std::string,std::vector<phrase_and_grammar_cats>>;

        phrase_translator()=default;

        /**
         * @brief Ctor
         * @param phrases Translated strings
         */
        phrase_translator(container_type phrases):_phrases(std::move(phrases))
        {}

        /**
         * @brief Reset map
         */
        virtual void reset() override
        {
            _phrases.clear();
        }

        /**
         * @brief Translate a string
         * @param id String id
         * @param cats Grammar categories to look for
         * @return Translated string or id if such string not found
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

        template <typename T>
        auto operator [] (T&& key);

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

template <typename  Enable, typename ... Args>
struct phrase_translator_setter_helper_t
{
};

template <typename ... Args>
struct phrase_translator_setter_helper_t<
            std::enable_if_t<
                std::is_constructible<phrase_and_grammar_cats,Args...>::value
            >,
            Args...
        >
{
    auto operator () (Args&&... args) const
    {
        return std::vector<phrase_and_grammar_cats>{phrase_and_grammar_cats(std::forward<Args>(args)...)};
    }
};

template <typename T>
struct phrase_translator_setter_helper_t<
            std::enable_if_t<
                std::is_same<std::initializer_list<phrase_and_grammar_cats>,T>::value
            >,
            T
        >
{
    auto operator () (T&& args) const
    {
        return std::vector<phrase_and_grammar_cats>{std::forward<T>(args)};
    }
};

template <typename ... Args>
constexpr phrase_translator_setter_helper_t<Args...> phrase_translator_setter_helper{};

}

struct phrase_translator_setter
{
    std::map<std::string,std::vector<phrase_and_grammar_cats>>::iterator it;

    template <typename ...Args>
    phrase_translator_setter& operator = (Args&&... args)
    {
        it->second=detail::phrase_translator_setter_helper<void,Args...>(std::forward<Args>(args)...);
        return *this;
    }

    phrase_translator_setter& operator = (std::initializer_list<phrase_and_grammar_cats>&& arg)
    {
        it->second=detail::phrase_translator_setter_helper<void,std::initializer_list<phrase_and_grammar_cats>>(std::move(arg));
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
