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
         * @brief Translate a string
         * @param id String ID
         * @return Translation result
         */
        std::string operator() (const std::string& id) const
        {
            return translate(id);
        }

        /**
         * @brief Reset translator
         */
        virtual void reset()
        {}

        /**
         * @brief Translate a string
         * @param id String ID
         * @return Translation result
         *
         * Must be overriden in derived class.
         */
        virtual std::string translate(const std::string& id) const
        {
            return id;
        }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRANSLATOR_HPP
