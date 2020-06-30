/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/translator.hpp
*
*
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TRANSLATOR_HPP
#define DRACOSHA_VALIDATOR_TRANSLATOR_HPP

#include <string>
#include <locale>
#include <map>
#include <memory>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct translator_tag;

struct no_translator_t
{
    using hana_tag=translator_tag;

    constexpr std::string operator() (const std::string& id) const
    {
        return hana::id(id);
    }
};
constexpr no_translator_t no_translator{};

class translator
{
    public:

        using hana_tag=translator_tag;

        std::string operator() (const std::string& id) const
        {
            return translate(id);
        }

        virtual void reset()
        {}

        virtual std::string translate(const std::string& id) const
        {
            return id;
        }
};

class mapped_translator : public translator
{
    public:

        virtual void reset() override
        {
            _strings.clear();
        }

        virtual std::string translate(const std::string& id) const override
        {
            auto it=_strings.find(id);
            if (it!=_strings.end())
            {
                return it.second;
            }
            return id;
        }

        std::map<std::string,std::string>& strings() noexcept
        {
            return _strings;
        }

    private:

        std::map<std::string,std::string> _strings;
};

class translator_factory
{
    public:

        translator_factory(
                std::shared_ptr<translator> default_translator,
                std::map<std::locale,std::shared_ptr<translator>> translators
            ) : _default_translator(std::move(default_translator)),
                _translators(std::move(translators))
        {}

        translator_factory(
                std::shared_ptr<translator> default_translator,
            ) : _default_translator(std::move(default_translator))
        {}

        translator_factory(
                std::map<std::locale,std::shared_ptr<translator>> translators
            ) : _translators(std::move(translators))
        {}

        translator_factory(
            ) : _default_translator(std::make_shared<translator>())
        {}

        void add_translator(
                std::shared_ptr<translator> tr,
                std::locale loc=std::locale()
            )
        {
            _translators[std::move(loc)]=std::move(tr);
        }
        std::shared_ptr<translator> find_translator(const std::locale& loc=std::locale()) const
        {
            auto it=_translators.find(loc);
            if (it!=_translators.end())
            {
                return it.second;
            }
            return _default_translator;
        }

        void set_default_translator(std::shared_ptr<translator> default_translator) noexcept
        {
            _default_translator=std::move(default_translator);
        }
        std::shared_ptr<translator> default_translator() const noexcept
        {
            return _default_translator;
        }

    private:

        std::shared_ptr<translator> _default_translator;
        std::map<std::locale,std::shared_ptr<translator>> _translators;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TRANSLATOR_HPP
