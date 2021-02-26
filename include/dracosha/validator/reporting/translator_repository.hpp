/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/translator_repository.hpp
*
*   Defines repository of translators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TRANSLATOR_REPOSITORY_HPP
#define DRACOSHA_VALIDATOR_TRANSLATOR_REPOSITORY_HPP

#include <locale>
#include <string>
#include <set>
#include <map>
#include <memory>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Translator repository is a repository of translators mapped to names of locales.
 *
 * A name of locale can be either in more generic form like "en" or in more specific form like "en_US.UTF-8".
 * When looking for a translator the repository will first try to use the most specific name of locale,
 * and if that is not found then the repository will try to use further less specific names down to the name of language only.
 * If still no translator is found then the default translator will be used.
 *
 * A translator should be added to the repository with the full list of all forms of locale names
 * this translator is suitable for. See example below.

\code{.cpp}

  // translator_en_us will be used for all "en_US" locales as well as for generic "en" language
  repository.add_translator(translator_en_us,{"en","en_US","en_US.UTF-8"});
  // translator_en_gb will be used for all "en_GB" locales but for the generic "en" language the translator_en_us will be used
  repository.add_translator(translator_en_gb,{"en_GB","en_GB.UTF-8"});

\endcode

 */
class translator_repository
{
    public:

        /**
         * @brief Full constructor.
         * @param default_translator Default translator to be used if requested locale is not found.
         * @param translators Map of translators.
         */
        translator_repository(
                std::shared_ptr<translator> default_translator,
                std::map<std::string,std::shared_ptr<translator>> translators
            ) : _default_translator(std::move(default_translator)),
                _translators(std::move(translators))
        {}

        /**
         * @brief Constructor with default translator.
         * @param default_translator Default translator to be used if requested locale is not found.
         */
        translator_repository(
                std::shared_ptr<translator> default_translator
            ) : _default_translator(std::move(default_translator))
        {}

        /**
         * @brief Constructor with map of translators.
         * @param translators Map of translators.
         */
        translator_repository(
                std::map<std::string,std::shared_ptr<translator>> translators
            ) : _default_translator(std::make_shared<translator>()),
                _translators(std::move(translators))
        {}

        /**
         * @brief Default consturctor.
         */
        translator_repository(
            ) : _default_translator(std::make_shared<translator>())
        {}

        /**
         * @brief Add translator to repository.
         * @param tr Translator.
         * @param locales Names of locales this translator can be used for.
         */
        void add_translator(
                const std::shared_ptr<translator>& tr,
                const std::set<std::string>& locales
            )
        {
            for (auto&& it:locales)
            {
                _translators[it]=tr;
            }
        }

        /**
         * @brief Find translator for locale
         * @param loc Locale name, examples: "en_US.UTF-8", "en_US", "en". Default is name of global locale.
         * @param Translator suitable of this locale.
         *
         * First, it tries to find the most specific name of the locale.
         * The the name is repeatedly truncated down to the name of language only.
         * If still no translator is found then the default translator is returned.
         */
        std::shared_ptr<translator> find_translator(std::string loc=std::locale().name()) const
        {
            size_t i=0;
            while(!loc.empty() && i<3)
            {
                auto it=_translators.find(loc);
                if (it!=_translators.end())
                {
                    return it->second;
                }
                std::string delimiter=(i++==0)?".":"_";
                loc=loc.substr(0,loc.find(delimiter));
            }
            return _default_translator;
        }

        /**
         * @brief Set default translator.
         * @param default_translator Default translator.
         */
        void set_default_translator(std::shared_ptr<translator> default_translator) noexcept
        {
            _default_translator=std::move(default_translator);
        }

        /**
         * @brief Get default translator.
         * @return Default translator.
         */
        std::shared_ptr<translator> default_translator() const noexcept
        {
            return _default_translator;
        }

        /**
         * @brief Clear repository.
         */
        void clear() noexcept
        {
            _translators.clear();
        }

    private:

        std::shared_ptr<translator> _default_translator;
        std::map<std::string,std::shared_ptr<translator>> _translators;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRANSLATOR_REPOSITORY_HPP
