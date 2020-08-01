/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/translator.hpp
*
*   Defines base translator class.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TRANSLATOR_HPP
#define DRACOSHA_VALIDATOR_TRANSLATOR_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct translator_tag;

/**
 * @brief Base translator class
 */
class translator
{
    public:

        using hana_tag=translator_tag;

        translator()=default;
        virtual ~translator()=default;
        translator(const translator&)=default;
        translator(translator&&)=default;
        translator& operator= (const translator&)=default;
        translator& operator= (translator&&)=default;

        /**
         * @brief Reset translator
         */
        virtual void reset()
        {}

        /**
         * @brief Translate a string
         * @param id String ID
         * @param cats Grammar categories to look for
         * @return Translation result
         *
         * Must be overriden in derived class.
         */
        virtual concrete_phrase translate(const std::string& id, grammar_categories cats=0) const
        {
            std::ignore=cats;
            return id;
        }

        /**
         * @brief Bypass concrete phrase as is
         * @param id Concrete phrase
         * @return Input concrete phrase
         */
        concrete_phrase translate(const concrete_phrase& id, grammar_categories =0) const
        {
            return id;
        }

        /**
         * @brief Translate a string
         * @param id String ID
         * @param cats Grammar categories to look for
         * @return Translation result
         */
        concrete_phrase operator() (const std::string& id, grammar_categories cats=0) const
        {
            return translate(id,cats);
        }

        /**
         * @brief Bypass concrete phrase as is
         * @param id Concrete phrase
         * @return Input concrete phrase
         */
        concrete_phrase operator() (const concrete_phrase& id, grammar_categories =0) const
        {
            return translate(id);
        }
};

using translator_cref=const translator&;

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRANSLATOR_HPP
