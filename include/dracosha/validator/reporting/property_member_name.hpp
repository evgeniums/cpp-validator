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

}

template <typename T, typename TraitsT, typename =hana::when<true>>
struct property_member_name_t
{
    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto prop=single_member_name(id.property,traits,grammar_cats);
        auto next_cats=phrase_grammar_cats(prop);
        auto sep=translate(traits,detail::to_string(traits.member_property_conjunction()));
        auto sep_cats=phrase_grammar_cats(sep);

        auto members=detail::list_member_names(id.member,traits,sep_cats);

        auto parts=hana::concat(
                        hana::make_tuple(std::move(prop),std::move(sep)),
                        std::move(members)
                    );

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           parts
        );
        return concrete_phrase(dst,next_cats);
    }

    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<!TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto member_parts=detail::list_member_names(id.member,traits,grammar_cats);
        auto prop=single_member_name(id.property,traits,last_grammar_categories(member_parts,grammar_cats));
        auto next_cats=phrase_grammar_cats(prop);
        auto sep=translate(traits,detail::to_string(traits.member_property_conjunction()));

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           hana::concat(std::move(member_parts),hana::make_tuple(std::move(sep),std::move(prop)))
        );
        return concrete_phrase(dst,next_cats);
    }
};

template <typename T, typename TraitsT>
struct property_member_name_t<T,TraitsT,
            hana::when<
                !detail::has_member_property<T,TraitsT>::value
                &&
                std::decay_t<typename T::member_type>::has_name
                &&
                !std::is_same<decltype(std::declval<std::decay_t<typename T::member_type>>().name()),concrete_phrase>::value
            >
        >
{
    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto prop=single_member_name(id.property,traits,grammar_cats);
        auto next_cats=phrase_grammar_cats(prop);
        auto sep=translate(traits,detail::to_string(traits.member_property_conjunction()));
        auto sep_cats=phrase_grammar_cats(sep);

        auto member=decorate(traits,translate(traits,id.member.name(),sep_cats));

        auto parts=hana::make_tuple(std::move(prop),std::move(sep),std::move(member));

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           parts
        );
        return concrete_phrase(dst,next_cats);
    }

    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<!TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto member=decorate(traits,translate(traits,id.member.name(),grammar_cats));
        auto prop=single_member_name(id.property,traits,phrase_grammar_cats(member,grammar_cats));
        auto next_cats=phrase_grammar_cats(prop);
        auto sep=translate(traits,detail::to_string(traits.member_property_conjunction()));

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           hana::make_tuple(std::move(member),std::move(sep),std::move(prop))
        );
        return concrete_phrase(dst,next_cats);
    }
};

template <typename T, typename TraitsT>
struct property_member_name_t<T,TraitsT,
            hana::when<
                !detail::has_member_property<T,TraitsT>::value
                &&
                std::decay_t<typename T::member_type>::has_name
                &&
                std::is_same<decltype(std::declval<std::decay_t<typename T::member_type>>().name()),concrete_phrase>::value
            >
        >
{
    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto prop=single_member_name(id.property,traits,grammar_cats);
        auto next_cats=phrase_grammar_cats(prop);
        auto sep=translate(traits,detail::to_string(traits.member_property_conjunction()));

        auto member=decorate(traits,id.member.name());

        auto parts=hana::make_tuple(std::move(prop),std::move(sep),std::move(member));

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           parts
        );
        return concrete_phrase(dst,next_cats);
    }

    template <typename TraitsT1>
    auto operator() (const T& id, const TraitsT1& traits, grammar_categories grammar_cats,
                       std::enable_if_t<!TraitsT1::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto member=decorate(traits,id.member.name(),grammar_cats);
        auto prop=single_member_name(id.property,traits,phrase_grammar_cats(member,grammar_cats));
        auto next_cats=phrase_grammar_cats(prop);
        auto sep=translate(traits,detail::to_string(traits.member_property_conjunction()));

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           hana::make_tuple(std::move(member),std::move(sep),std::move(prop))
        );
        return concrete_phrase(dst,next_cats);
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
