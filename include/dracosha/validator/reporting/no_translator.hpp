/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/no_translator.hpp
*
*   Defines translator stub.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_NO_TRANSLATOR_HPP
#define DRACOSHA_VALIDATOR_NO_TRANSLATOR_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Translator stub to use when no translation is needed.
 */
struct no_translator_t
{
    using hana_tag=translator_tag;

    std::string operator() (const std::string& id, grammar_categories =0) const
    {
        return hana::id(id);
    }
};

/**
  @brief Instance of translator stub to use when no translation is needed.
*/
constexpr no_translator_t no_translator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NO_TRANSLATOR_HPP
