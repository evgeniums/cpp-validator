/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/strings.hpp
*
* Defines helper for strings formatting.
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
#include <dracosha/validator/detail/to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct strings_tag;

/**
 * @brief Helper for strings formatting.
 *
 * Strings formatting is used in failuire reporting and may also be used in logging.
 *
 * Conversion of a generic ID to string is performed as follows.
 *   - If string ID is a property then name() of the property is used.
 *   - If string ID is convertible to std::string then that conversion is used, e.g. all validating operators have operator for converting to string.
 *   - If string ID is of some scalar type that can be forwarded to std::to_string(id) then std::to_string() is used.
 *   - Otherwise <?????> token is used.
 *
 * After ID is converted to string the conversion result goes to translator. Translator returns translated or original string.
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
     * @brief Convert ID to string
     * @param id ID that must be converted to string
     */
    template <typename T>
    std::string to_string(const T& id) const
    {
        return detail::to_string<T>(id);
    }

    /**
     * @brief Convert ID to string and then translate it
     * @param id ID that must be converted to string
     * @return ID converted to string and then translated
     */
    template <typename T>
    std::string operator() (const T& id) const
    {
        return _translator(to_string(id));
    }

    std::string aggregation_open(const aggregation_op& aggregation) const
    {
        return _translator(_aggregation_strings.open(aggregation));
    }

    std::string aggregation_close(const aggregation_op& aggregation) const
    {
        return _translator(_aggregation_strings.close(aggregation));
    }

    std::string aggregation_conjunction(const aggregation_op& aggregation) const
    {
        return _translator(_aggregation_strings.conjunction(aggregation));
    }

    std::string member_names_conjunction() const
    {
        return _translator(_aggregation_strings.member_names_conjunction());
    }
};

/**
  @brief Default strings object does not perform any translation.
*/
constexpr strings<const no_translator_t&,const aggregation_strings_t&> default_strings{no_translator,aggregation_strings};

template <typename AggregationStringsT>
using translated_strings=strings<const translator&,AggregationStringsT>;

struct make_translated_strings_t
{
    /**
     * @brief Make strings object using translator found in translator repository by locale name
     * @param rep Translator repository
     * @param loc Locale name
     * @return Strings object with translator for given locale name from the repository
     */
    auto operator() (const translator_repository& rep,const std::string& loc=std::locale().name()) const
    {
        return translated_strings<const aggregation_strings_t&>{*rep.find_translator(loc),aggregation_strings};
    }

    /**
     * @brief Make strings object using translator found in translator repository by locale name
     * @param aggregation_str Formatter of aggregation strings
     * @param rep Translator repository
     * @param loc Locale name
     * @return Strings object with translator for given locale name from the repository
     */
    template <typename T>
    auto operator() (T&& aggregation_str,const translator_repository& rep,const std::string& loc=std::locale().name()) const
    {
        return translated_strings<T>{*rep.find_translator(loc),std::forward<T>(aggregation_str)};
    }
};
constexpr make_translated_strings_t make_translated_strings{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRINGS_HPP
