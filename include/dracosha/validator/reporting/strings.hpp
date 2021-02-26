/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/strings.hpp
*
* Defines formatter of miscellaneous strings.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRINGS_HPP
#define DRACOSHA_VALIDATOR_STRINGS_HPP

#include <string>
#include <locale>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/reporting/translator.hpp>
#include <dracosha/validator/reporting/no_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/reporting/aggregation_strings.hpp>
#include <dracosha/validator/utils/to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename T, typename = hana::when<true>>
struct strings_helper_t
{
    template <typename TranslatorT, typename T1>
    auto operator() (TranslatorT&& translator, T1&& val, grammar_categories grammar_cats) const
    {
        return translator(val,grammar_cats);
    }
};

template <typename T>
struct strings_helper_t<
            T,
            hana::when<std::is_same<T,concrete_phrase>::value>
        >
{
    template <typename TranslatorT, typename T1>
    auto operator() (TranslatorT&&, T1&& val, grammar_categories) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(val));
    }
};
template <typename T>
constexpr strings_helper_t<T> strings_helper_inst{};

template <typename TranslatorT, typename T>
auto strings_helper(TranslatorT&& translator, T&& val, grammar_categories grammar_cats) -> decltype(auto)
{
    return strings_helper_inst<std::decay_t<T>>(std::forward<TranslatorT>(translator),std::forward<T>(val),grammar_cats);
}

}

struct strings_tag;

/**
 * @brief Formatter of miscellaneous strings.
 *
 * This formatter is used to format operators and some other strings.
 *
 * Conversion of a generic ID to string is performed as follows.
 *   - If string ID is a property then name() of the property is used.
 *   - If string ID is convertible to std::string then that conversion is used, e.g. all validating operators have operator for converting to string.
 *   - If string ID is of some scalar type that can be forwarded to std::to_string(id) then std::to_string() is used.
 *   - Otherwise <?????> token is used.
 *
 * After ID is converted to string the conversion result goes to translator. Translator returns either translated or original string.
 */
template <typename TranslatorT, typename AggregationStringsT>
struct strings
{
    using hana_tag=strings_tag;

    TranslatorT _translator;
    AggregationStringsT _aggregation_strings;

    ~strings()=default;
    strings(strings&&)=default;
    strings(const strings&)=delete;
    strings& operator =(strings&&)=default;
    strings& operator =(const strings&)=delete;

    /**
     * @brief Convert ID to string and then translate it.
     * @param id ID that must be converted to string.
     * @return ID converted to string and then translated.
     */
    template <typename T>
    concrete_phrase operator() (const T& id, grammar_categories grammar_cats=0
                                ,
                                    std::enable_if_t<
                                       !(
                                        std::is_base_of<wrap_op_with_string_tag,T>::value
                                       )
                                    ,void*> =nullptr
                                ) const
    {
        return _translator(to_string(id),grammar_cats);
    }

    /**
     * @brief Convert ID to concrete phrase for wrapped operator with explicit string.
     * @param id Wrapped operator with explicit string.
     * @return ID converted to string and then translated.
     */
    template <typename T>
    concrete_phrase operator() (const T& id, grammar_categories grammar_cats=0,
                                std::enable_if_t<
                                   (
                                    std::is_base_of<wrap_op_with_string_tag,T>::value
                                   )
                                ,void*> =nullptr
                                ) const
    {
        return detail::strings_helper(_translator,id.str(),grammar_cats);
    }

    /**
     * @brief Get string for step that opens aggregation operator.
     * @param aggregation Aggregation descriptor.
     * @return String to use for opening aggregation operator.
     */
    std::string aggregation_open(const aggregation_op& aggregation) const
    {
        return _translator(_aggregation_strings.open(aggregation));
    }

    /**
     * @brief Get string for step that closes aggregation operator.
     * @param aggregation Aggregation descriptor.
     * @return String to use for closing aggregation operator.
     */
    std::string aggregation_close(const aggregation_op& aggregation) const
    {
        return _translator(_aggregation_strings.close(aggregation));
    }

    /**
     * @brief Get string for conjunction of intermediate validators within aggregation operator.
     * @param aggregation Aggregation descriptor.
     * @return String to use to join intermediate validators within aggregation operator.
     */
    std::string aggregation_conjunction(const aggregation_op& aggregation) const
    {
        return _translator(_aggregation_strings.conjunction(aggregation));
    }
};

/**
  @brief Default strings object that does not perform any translation.
*/
constexpr strings<const no_translator_t&,const aggregation_strings_t&> default_strings{no_translator,aggregation_strings};

template <typename AggregationStringsT>
using translated_strings=strings<const translator&,AggregationStringsT>;

/**
 * @brief Helper for making translated strings.
 */
struct make_translated_strings_t
{
    /**
     * @brief Make strings object using a given translator.
     * @param tr Translator to use.
     * @return Strings object.
     */
    auto operator() (const translator& tr) const
    {
        return translated_strings<const aggregation_strings_t&>{tr,aggregation_strings};
    }

    /**
     * @brief Make strings object using translator found in translator repository by locale name.
     * @param rep Translator repository.
     * @param loc Locale name.
     * @return Strings object with translator for given locale name from the repository.
     */
    auto operator() (const translator_repository& rep,const std::string& loc=std::locale().name()) const
    {
        return translated_strings<const aggregation_strings_t&>{*rep.find_translator(loc),aggregation_strings};
    }

    /**
     * @brief Make strings object using translator found in translator repository by locale name.
     * @param aggregation_str Formatter of aggregation strings.
     * @param rep Translator repository.
     * @param loc Locale name.
     * @return Strings object with translator for given locale name from the repository.
     */
    template <typename T>
    auto operator() (T&& aggregation_str,const translator_repository& rep,const std::string& loc=std::locale().name()) const
    {
        return translated_strings<T>{*rep.find_translator(loc),std::forward<T>(aggregation_str)};
    }
};
/**
  Callable object for creating translated strings formatter.
 */
constexpr make_translated_strings_t make_translated_strings{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRINGS_HPP
