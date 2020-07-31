/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/property_member_name.hpp
*
* Defines helper to format property of a member.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PROPERTY_MEMBER_NAME_HPP
#define DRACOSHA_VALIDATOR_PROPERTY_MEMBER_NAME_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/reporting/nested_member_name.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Check if traits do not implement method member_property(id,traits)
 */
template <typename T, typename TraitsT,
          typename =void>
struct has_member_property
{
    constexpr static const bool value=false;
};

/**
 * @brief Check if traits implement method nested(id,traits,translator)
 */
template <typename T, typename TraitsT>
struct has_member_property<T,TraitsT,
                  decltype(
                    (void)std::declval<std::decay_t<TraitsT>>().member_property(
                        std::declval<std::decay_t<T>>()
                    )
                  )
        >
{
    constexpr static const bool value=true;
};

template <typename T, typename TraitsT, typename =void>
struct property_with_separator_t
{
    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto name=single_member_name(std::forward<T>(id),traits,grammar_cats);
        auto attrs=phrase_grammar_cats(name);
        return hana::make_tuple(std::move(name),translate(traits,detail::to_string(traits.member_property_conjunction()),attrs));
    }

    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<!TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto separator=translate(traits,detail::to_string(traits.member_property_conjunction()),grammar_cats);
        auto name=single_member_name(std::forward<T>(id),traits,phrase_grammar_cats(separator));
        return hana::make_tuple(std::move(separator),std::move(name));
    }
};

template <typename T, typename TraitsT>
struct property_with_separator_t<T,TraitsT,
        decltype(
            (void)std::declval<std::decay_t<T>>().property_with_separator(std::declval<std::decay_t<T>>(),std::declval<grammar_categories>())
        )>
{
    auto operator() (T&& id, const TraitsT& traits, const grammar_categories& grammar_cats) const
    {
        return traits.property_with_separator(std::forward<T>(id),grammar_cats);
    }
};

template <typename T, typename TraitsT>
constexpr property_with_separator_t<T,TraitsT> property_with_separator_inst{};

template <typename T, typename TraitsT>
auto property_with_separator(T&& id, const TraitsT& traits, const grammar_categories& grammar_cats=0)
{
    return property_with_separator_inst<T,TraitsT>(std::forward<T>(id),traits,grammar_cats);
}

}

template <typename T, typename TraitsT, typename =hana::when<true>>
struct property_member_name_t
{
    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto prop=detail::property_with_separator(id.property,traits,grammar_cats);
        auto attrs=phrase_grammar_cats(prop);
        auto parts=hana::concat(
                        std::move(prop),
                        detail::list_member_names(id.member,traits,attrs)
                    );

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           parts
        );
        return concrete_phrase(dst,last_grammar_categories(parts,attrs));
    }

    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<!TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto member_parts=list_member_names(id.member,traits,grammar_cats);
        auto prop=detail::property_with_separator(id.property,traits,last_grammar_categories(member_parts,grammar_cats));
        auto attrs=phrase_grammar_cats(prop);

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           hana::concat(std::move(member_parts),std::move(prop))
        );
        return concrete_phrase(dst,attrs);
    }
};

template <typename T, typename TraitsT>
struct property_member_name_t<T,TraitsT,
            hana::when<detail::has_member_property<T,TraitsT>::value>
        >
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const -> decltype(auto)
    {
        return traits.member_property(id,traits,grammar_cats);
    }
};

template <typename T, typename TraitsT>
constexpr property_member_name_t<T,TraitsT> property_member_name_inst{};

template <typename T, typename TraitsT>
constexpr auto property_member_name(const T& id, const TraitsT& traits, grammar_categories grammar_cats=0) -> decltype(auto)
{
    return property_member_name_inst<T,TraitsT>(id,traits,grammar_cats);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTY_MEMBER_NAME_HPP
