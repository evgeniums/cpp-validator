/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/member_names.hpp
*
* Defines member names formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
#define DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/member_property.hpp>
#include <dracosha/validator/reporting/translator.hpp>
#include <dracosha/validator/reporting/no_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/single_member_name.hpp>
#include <dracosha/validator/reporting/nested_member_name.hpp>
#include <dracosha/validator/reporting/property_member_name.hpp>
#include <dracosha/validator/reporting/decorator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct member_names_tag;

/**
 * @brief Member names formatter.
 */
template <typename TraitsT>
struct member_names
{
    using hana_tag=member_names_tag;
    using traits_type=TraitsT;

    TraitsT traits;

    /**
     * @brief Format object of member or make_member_with_name types.
     * @param member Member object.
     * @param grammar_cats Gramatical categories to use for translation.
     * @return Formatted name of the member.
     */
    template <typename T>
    auto operator() (const T& member,
                     grammar_categories grammar_cats=0,
                     std::enable_if_t<hana::is_a<member_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return nested_member_name(member,traits,grammar_cats);
    }

    /**
     * @brief Format object of member_property type.
     * @param member Pair of member and property.
     * @param grammar_cats Gramatical categories to use for translation.
     * @return Formatted name of the member's property.
     */
    template <typename T>
    auto operator() (const T& member_prop,
                     grammar_categories grammar_cats=0,
                     std::enable_if_t<hana::is_a<member_property_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return property_member_name(member_prop,*this,grammar_cats);
    }

    /**
     * @brief Format single key of member path.
     * @param id Single key of member path.
     * @param grammar_cats Gramatical categories to use for translation.
     * @return Formatted name the key.
     */
    template <typename T>
    auto operator() (const T& id,
                     grammar_categories grammar_cats=0,
                     std::enable_if_t<
                            (
                                !hana::is_a<member_tag,T>
                                &&
                                !hana::is_a<member_property_tag,T>
                             )
                     ,void*> =nullptr
            ) const -> decltype(auto)
    {
        return single_member_name(id,traits,grammar_cats);
    }
};

/**
 * @brief Create member names.
 * @param traits Actual implementer of names formatting.
 * @return Formatter of member names.
 */
template <typename TraitsT>
auto make_member_names(TraitsT&& traits)
{
    return member_names<TraitsT>{std::forward<TraitsT>(traits)};
}

/**
 * @brief Default traits for member names formatter.
 *
 * Default traits join member names in reverse order using of conjunction,
 * e.g. ["field1"]["subfield1_1"]["subfield1_1_1"] will be formatted as "subfield1_1_1 of subfield1_1 of field1".
 */
struct default_member_names_traits_t
{
    /**
     * @brief Get string for conjunction of nested member names.
     * @return String to use to join nested member names in the member's path.
     *
     * Default is " of ".
     */
    static auto member_names_conjunction() -> decltype(auto)
    {
        return string_member_name_conjunction;
    }

    /**
     * @brief Get string for conjunction of member and its property.
     * @return String to use to join property and its property.
     *
     * Default is the same as member_names_conjunction().
     */
    static auto member_property_conjunction() -> decltype(auto)
    {
        return string_member_name_conjunction;
    }

    /**
     * @brief Check if nested memebr names must be joined in reverse order.
     *
     * Member names can be joined either in direct order (e.g. ["field1"]["subfield1_1"]["subfield1_1_1"] joined as field1.subfield1_1.subfield1_1_1)
     * or in reverse order (e.g. ["field1"]["subfield1_1"]["subfield1_1_1"] joined as subfield1_1_1 of subfield1_1 of field1).
     *
     * Default is reverse order.
     */
    constexpr static const bool is_reverse_member_names_order=true;

    /**
     * @brief Check if property of a memebr must be joined in reverse order.
     *
     * Default is reverse order.
     */
    constexpr static const bool is_reverse_member_property_order=true;
};
constexpr default_member_names_traits_t default_member_names_traits{};

/**
 * @brief Get default member names formatter.
 * @return Default formatter of member names.
 */
inline auto get_default_member_names()
    -> std::add_lvalue_reference_t<
            std::add_const_t<decltype(make_member_names(default_member_names_traits))>
        >
{
    static const auto default_member_names=make_member_names(default_member_names_traits);
    return default_member_names;
}

inline auto make_default_member_names()
{
    return make_member_names(default_member_names_traits_t{});
}

/**
 * @brief Traits with translator for member names formatter.
 */
template <typename TraitsT>
struct translate_member_names_traits_t : public TraitsT
{
    translate_member_names_traits_t(
                TraitsT&& traits,
                translator_cref translator
            ) : TraitsT(std::move(traits)),
                translator(translator)
    {}

    translator_cref translator;
};

/**
 * @brief Create translated member names from other member names using translator.
 * @param original_mn Original member names formatter.
 * @param translator Translator.
 * @return Member names formatter that first uses original formatter and then translate member names.
 */
template <typename OriginalMemberNamesT>
auto make_translated_member_names(OriginalMemberNamesT&& original_mn, translator_cref translator)
{
    return make_member_names(
                translate_member_names_traits_t<typename std::decay_t<OriginalMemberNamesT>::traits_type>(
                    std::move(original_mn.traits),
                    translator
                )
            );
}

/**
 * @brief Create translated member names from other member names using translator from translator repository.
 * @param original_mn Original member names formatter.
 * @param rep Translator repository.
 * @param loc Locale.
 * @return Member names formatter that first uses original formatter and then translate member names.
 */
template <typename OriginalMemberNamesT>
auto make_translated_member_names(OriginalMemberNamesT&& original_mn, const translator_repository& rep, const std::string& loc=std::locale().name())
{
    return make_translated_member_names(std::move(original_mn),*rep.find_translator(loc));
}

/**
 * @brief Create member names with default traits using translator
 * @param translator Translator
 * @return Member names formatter that first uses original formatter and then translate member names
 */
inline auto make_translated_member_names(translator_cref translator)
{
    return make_translated_member_names(make_member_names(default_member_names_traits_t{}),translator);
}

/**
 * @brief Create member names with default traits using translator from translator repository.
 * @param rep Translator repository.
 * @param loc Locale.
 * @return Member names formatter that first uses original formatter and then translate member names.
 */
inline auto make_translated_member_names(const translator_repository& rep, const std::string& loc=std::locale().name())
{
    return make_translated_member_names(*rep.find_translator(loc));
}

/**
 * @brief Traits for member names formatter that decorates member name.
 */
template <typename TraitsT, typename DecoratorT>
struct decorate_member_names_traits_t : public TraitsT
{
    decorate_member_names_traits_t(
                TraitsT&& traits,
                DecoratorT&& decorator
            ) : TraitsT(std::move(traits)),
                decorator(std::forward<DecoratorT>(decorator))
    {}

    DecoratorT decorator;
};

/**
 * @brief Make decorated member names from other member names using decorator.
 * @param original_mn Original member names formatter.
 * @param decorator Decorator.
 * @return Member names formatter that first uses original formatter and then decorates member names.
 */
template <typename OriginalMemberNamesT, typename DecoratorT>
auto make_decorated_member_names(OriginalMemberNamesT&& original_mn, DecoratorT&& decorator)
{
    return make_member_names(
                decorate_member_names_traits_t<typename std::decay_t<OriginalMemberNamesT>::traits_type,DecoratorT>(
                    std::move(original_mn.traits),
                    std::forward<DecoratorT>(decorator)
                )
            );
}

/**
 * @brief Traits to format nested members as dot separated names decorated with brackets.
 *
 * For example: ["field1"]["subfield1_1"]["subfield1_1_1"] will be formatted as [field1].[subfield1_1].[subfield1_1_1]
 */
struct dotted_member_names_traits_t
{
    /**
     * @brief Get string for conjunction of nested member names.
     * @return String "." to use to join nested member names in the member's path.
     */
    static auto member_names_conjunction()
    {
        return ".";
    }

    static auto member_property_conjunction()
    {
        return ".";
    }

    /**
     * @brief Check if nested memebr names must be joined in reverse order.
     */
    constexpr static const bool is_reverse_member_names_order=false;

    /**
     * @brief Check if property of a memebr must be joined in reverse order.
     */
    constexpr static const bool is_reverse_member_property_order=false;

    brackets_decorator_t decorator;
};

/**
  @brief Formatter of member names in direct dotted format.

  For example: ["field1"]["subfield1_1"]["subfield1_1_1"] will be formatted as [field1].[subfield1_1].[subfield1_1_1]
*/
constexpr member_names<dotted_member_names_traits_t> dotted_member_names{dotted_member_names_traits_t{}};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
