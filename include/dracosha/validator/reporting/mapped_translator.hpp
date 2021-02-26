/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/mapped_translator.hpp
*
*   Defines translator class that uses map of strings.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAPPED_TRANSLATOR_HPP
#define DRACOSHA_VALIDATOR_MAPPED_TRANSLATOR_HPP

#include <map>
#include <vector>
#include <algorithm>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Mapped translator uses map of translated strings.
 *
 * Translator can be constructed with prepared map as
 * @code
 * mapped_translator{map};
 * @endcode
 */
class mapped_translator : public translator
{
    public:

        mapped_translator()=default;

        /**
         * @brief Constructor.
         * @param Translated strings.
         */
        mapped_translator(std::map<std::string,std::string> strings):_strings(std::move(strings))
        {}

        /**
         * @brief Reset map.
         */
        virtual void reset() override
        {
            _strings.clear();
        }

        /**
         * @brief Translate a string.
         * @param id String id.
         * @return Translated string or id if such string not found.
         */
        virtual translation_result translate(const std::string& id, grammar_categories =0) const override
        {
            auto it=_strings.find(id);
            if (it!=_strings.end())
            {
                return translation_result{it->second,true};
            }
            return translation_result{id,false};
        }

        /**
         * @brief Get map of translated strings.
         * @return Map of translated strings.
         */
        std::map<std::string,std::string>& strings() noexcept
        {
            return _strings;
        }

    private:

        std::map<std::string,std::string> _strings;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAPPED_TRANSLATOR_HPP
