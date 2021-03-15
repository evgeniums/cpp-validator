/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/single_member_name.hpp
*
* Defines helper for singe member name formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP
#define DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/decorator.hpp>
#include <dracosha/validator/reporting/translate.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>
#include <dracosha/validator/utils/to_string.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/heterogeneous_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String to use for indexed elements.
 */
struct string_element_t : public enable_to_string<string_element_t>
{
    constexpr static const char* description="element #";
};
constexpr string_element_t string_element{};

//-------------------------------------------------------------

/**
 * @brief Check if traits have suitable operator ()(const T&) for name transforming in case when they don't.
 */
template <typename T, typename TraitsT, typename=void>
struct can_single_member_name
{
    constexpr static const bool value=false;
};

/**
 * @brief Check if traits have suitable operator ()(const T&) for name transforming.
 */
template <typename T, typename TraitsT>
struct can_single_member_name<T,TraitsT,
            std::enable_if_t<
                std::is_same<
                    decltype((void)std::declval<TraitsT>()(std::declval<T>())),
                    void
                >::value
            >
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Default helper for single member name formatting when traits can not be used.
 *
 * Default helper converts ID to string, then translates it and after all decorates the result.
 */
template <typename T, typename TraitsT, typename =hana::when<true>>
struct single_member_name_t
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const -> decltype(auto)
    {
        return hana::eval_if(
            std::is_base_of<variadic_property_tag,unwrap_object_t<T>>{},
            [&](auto&& _)
            {
                auto format=[&](auto&& v, bool with_cats=false, bool with_decorate=false)
                {
                    auto res=with_cats ?
                                translate(_(traits),to_string(v),_(grammar_cats))
                                :
                                translate(_(traits),to_string(v));
                    return with_decorate ? decorate(_(traits),res) : res;
                };
                return _(id).name(format);
            },
            [&](auto&& _)
            {
                return decorate(_(traits),translate(_(traits),to_string(_(id)),_(grammar_cats)));
            }
        );
    }
};

/**
 * @brief Helper for single member name formatting when traits can be used.
 *
 * ID is forwarded to traits, then the result goes to decorator.
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,hana::when<can_single_member_name<unwrap_object_t<T>,TraitsT>::value>>
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const -> decltype(auto)
    {
        return decorate(traits,translate(traits,traits(unwrap_object(id)),grammar_cats));
    }
};

/**
 * @brief Helper for single member name formatting when traits can not be used and id is of integral type.
 *
 * Integral ID is interpreted as index and formatted like "element #id", then it is translated and after all the result is decorated.
 */
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,
                                hana::when<
                                    !can_single_member_name<unwrap_object_t<T>,TraitsT>::value
                                    &&
                                    std::is_integral<unwrap_object_t<T>>::value
                                >
                            >
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const
    {
        std::string dst;
        backend_formatter.append(dst,translate(traits,std::string(string_element),grammar_cats),unwrap_object(id));
        return decorate(traits,dst);
    }
};

/**
 * @brief Template instance of for singe member name formatting.
 */
template <typename T, typename TraitsT>
constexpr single_member_name_t<T,TraitsT> single_member_name_inst{};

/**
 * @brief Process key of a member at one level.
 * @param id Member key.
 * @param traits Traits of member names formatter.
 * @param grammar_cats Grammatical categories to use in name translation.
 * @return Processed member name.
 */
template <typename T, typename TraitsT>
constexpr auto single_member_name(const T& id, const TraitsT& traits, grammar_categories grammar_cats=0) -> decltype(auto)
{
    return single_member_name_inst<T,TraitsT>(id,traits,grammar_cats);
}

//! @todo Move it to separate file.
template <typename T, typename TraitsT>
struct single_member_name_t<T,TraitsT,
                                hana::when<
                                    !can_single_member_name<unwrap_object_t<T>,TraitsT>::value
                                    &&
                                    std::is_base_of<heterogeneous_property_just_index_tag,unwrap_object_t<T>>::value
                                >
                            >
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const
    {
        return single_member_name(id.name(),traits,grammar_cats);
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SINGLE_MEMBER_NAME_HPP
