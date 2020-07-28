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
#include <dracosha/validator/reporting/word_attributtes.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct concrete_phrase_tag;

class concrete_phrase
{
    public:

        using hana_tag=concrete_phrase_tag;

        concrete_phrase(
                std::string phrase
            ) : _phrase(std::move(phrase)),
                _attributes(0)
        {}

        concrete_phrase(
                std::string phrase,
                word_attributes attributes
            ) : _phrase(std::move(phrase)),
                _attributes(attributes)
        {}

        concrete_phrase(
                concrete_phrase&& phrase,
                word_attributes attributes
            ) : _phrase(std::move(phrase._phrase)),
                _attributes(attributes)
        {}

        concrete_phrase(
                std::string phrase,
                word attributes
            ) : _phrase(std::move(phrase)),
                _attributes(word_bitmask.bit(attributes))
        {}

        concrete_phrase(
                std::string phrase,
                const std::initializer_list<word>& attributes
            ) : _phrase(std::move(phrase)),
                _attributes(word_bitmask.bits(attributes))
        {}

        template <typename ... Attributes>
        concrete_phrase(
                std::string phrase,
                Attributes&&... attributes
            ) : _phrase(std::move(phrase)),
                _attributes(word_bitmask.bits(std::forward<Attributes>(attributes)...))
        {}

        word_attributes attributes() const noexcept
        {
            return _attributes;
        }

        std::string phrase() const
        {
            return _phrase;
        }

        const std::string& phrase_ref() const
        {
            return _phrase;
        }

        operator std::string() const
        {
            return _phrase;
        }

        void set_phrase(std::string phrase)
        {
            _phrase=std::move(phrase);
        }

        void set_attributes(word_attributes attributes) noexcept
        {
            _attributes=attributes;
        }

        bool empty() const noexcept
        {
            return _phrase.empty();
        }

        friend std::ostream& operator<<(std::ostream& os, const concrete_phrase& ph)
        {
            return os << ph.phrase();
        }

    private:

        std::string _phrase;
        word_attributes _attributes;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONCRETE_PHRASE_HPP
