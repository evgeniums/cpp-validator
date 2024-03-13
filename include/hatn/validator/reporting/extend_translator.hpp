/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/extend_translator.hpp
*
*   Defines extended translator.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_EXTEND_TRANSLATOR_HPP
#define HATN_VALIDATOR_EXTEND_TRANSLATOR_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/reporting/translator.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Translator estension.
 *
 * First try to translate using extension translator. If no translation found then use base translator.
 */
template <typename BaseTranslatorT, typename ExtentionTranslatorT>
struct extend_translator_t : public translator
{
    /**
     * @brief Constructor.
     * @param base Base translator.
     * @param extension Translator to extend with.
     */
    extend_translator_t(
            BaseTranslatorT&& base,
            ExtentionTranslatorT&& extension
        ) : base(std::forward<BaseTranslatorT>(base)),
            extension(std::forward<ExtentionTranslatorT>(extension))
    {}

    /**
     * @brief Translate a string.
     * @param id String id.
     * @param cats Grammar categories to look for.
     * @return Translated string or id if such string not found.
     */
    virtual translation_result translate(const std::string& id, grammar_categories cats=0) const override
    {
        auto result=extension.translate(id,cats);
        if (result)
        {
            return result;
        }
        return base.translate(id,cats);
    }

    BaseTranslatorT base;
    ExtentionTranslatorT extension;
};

/**
 * @brief Create extended translator.
 * @param base Base translator.
 * @param extension Translator to extend with.
 * @return Translator with extension.
 *
 * Translator with extentions first tries to translate using extension. If no translation found then base translator is used.
 */
template <typename BaseTranslatorT, typename ExtentionTranslatorT>
auto extend_translator(BaseTranslatorT&& base, ExtentionTranslatorT&& extension)
{
    return extend_translator_t<BaseTranslatorT,ExtentionTranslatorT>{std::forward<BaseTranslatorT>(base),std::forward<ExtentionTranslatorT>(extension)};
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_EXTEND_TRANSLATOR_HPP
