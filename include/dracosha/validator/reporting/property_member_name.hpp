/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/property_member_name.hpp
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
 * @brief Check if traits do not implement method member_property(id,member_names_formatter,grammar_categories).
 */
template <typename T, typename FormatterT,
          typename =hana::when<true>>
struct has_member_property
{
    constexpr static const bool value=false;
};

/**
 * @brief Check if traits implement method member_property(id,member_names_formatter,grammar_categories).
 */
template <typename T, typename FormatterT>
struct has_member_property<T,FormatterT,
            hana::when<
                  std::is_same<
                        decltype(
                            (void)std::declval<typename std::decay_t<typename FormatterT::traits_type>>().member_property(
                                    std::declval<std::decay_t<T>>(),
                                    std::declval<FormatterT>(),
                                    std::declval<grammar_categories>()
                                )
                        ),
                        void
                    >::value
            >
        >
{
    constexpr static const bool value=true;
};

template <typename T>
struct formatter_traits
{
    using type=std::decay_t<typename std::decay_t<T>::traits_type>;
};

}

/**
 * @brief Helper for constructing name of member's property when the member has explicit name.
 */
template <typename T, typename FormatterT, typename = hana::when<true>>
struct property_member_name_t
{
    /**
     * @brief Reverse order of property name of a member like "property of member".
     * @param id Pair of property and member.
     * @param mn Member names formatter.
     * @param grammar_cats Grammatical categories of preceding phrase.
     * @return Formatted property name of a member.
     */
    template <typename FormatterT1>
    auto operator() (const T& id, const FormatterT1& mn, grammar_categories grammar_cats,
                       std::enable_if_t<detail::formatter_traits<FormatterT1>::type::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto&& prop=single_member_name(id.property,mn.traits,grammar_cats);
        auto next_cats=phrase_grammar_cats(prop);
        auto&& sep=translate(mn.traits,to_string(mn.traits.member_property_conjunction()));
        auto sep_cats=phrase_grammar_cats(sep);
        auto&& mmbr=mn(id.member,sep_cats);

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           hana::make_tuple(std::move(prop),std::move(sep),std::move(mmbr))
        );
        return concrete_phrase(dst,next_cats);
    }

    /**
     * @brief Direct order of property name of a member like "member.property".
     * @param id Pair of property and member.
     * @param mn Member names formatter.
     * @param grammar_cats Grammatical categories of preceding phrase.
     * @return Formatted property name of a member.
     */
    template <typename FormatterT1>
    auto operator() (const T& id, const FormatterT& mn, grammar_categories grammar_cats,
                       std::enable_if_t<!detail::formatter_traits<FormatterT1>::type::is_reverse_member_property_order,void*> =nullptr
            ) const
    {
        auto&& mmbr=mn(id.member,grammar_cats);
        auto&& prop=single_member_name(id.property,mn.traits,phrase_grammar_cats(mmbr,grammar_cats));
        auto next_cats=phrase_grammar_cats(prop);
        auto&& sep=translate(mn.traits,to_string(mn.traits.member_property_conjunction()));

        std::string dst;
        backend_formatter.append_join(
           dst,
           "",
           hana::make_tuple(std::move(mmbr),std::move(sep),std::move(prop))
        );
        return concrete_phrase(dst,next_cats);
    }
};

/**
 * @brief Helper for constructing name of member's property when traits have custom formatting method member_property.
 */
template <typename T, typename FormatterT>
struct property_member_name_t<T,FormatterT,
            hana::when<detail::has_member_property<T,FormatterT>::value>
        >
{
    template <typename FormatterT1>
    auto operator() (const T& id, const FormatterT1& mn, grammar_categories grammar_cats) const -> decltype(auto)
    {
        return mn.traits.member_property(id,mn,grammar_cats);
    }
};

/**
 * @brief Template instance of helper for constructing name of member's property.
 */
template <typename T, typename FormatterT>
constexpr property_member_name_t<T,FormatterT> property_member_name_inst{};

/**
 * @brief Implementation of property_member_name.
 */
struct property_member_name_impl
{
    template <typename T, typename FormatterT>
    auto operator () (const T& id, const FormatterT& mn, grammar_categories grammar_cats=0) const -> decltype(auto)
    {
        return property_member_name_inst<T,FormatterT>(id,mn,grammar_cats);
    }
};
/**
 * @brief Format name of member's property.
 * @param id Pair of property and member.
 * @param mn Member names formatter.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Formatted property name of a member.
 *
 * Name of member's property is a conjunction of property's name and member's name.
 */
constexpr property_member_name_impl property_member_name{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTY_MEMBER_NAME_HPP
