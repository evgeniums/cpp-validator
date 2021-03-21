/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/nested_member_name.hpp
*
* Defines helper to format nested member names.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP
#define DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/reporting/phrase_grammar_cats.hpp>
#include <dracosha/validator/reporting/single_member_name.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/member_with_name.hpp>
#include <dracosha/validator/member_with_name_list.hpp>
#include <dracosha/validator/make_member.hpp>
#include <dracosha/validator/compact_variadic_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String to use for joining nested member names in member's path.
 */
struct string_member_name_conjunction_t : public enable_to_string<string_member_name_conjunction_t>
{
    constexpr static const char* description=" of ";
};
constexpr string_member_name_conjunction_t string_member_name_conjunction{};

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Check if traits do not implement method nested(id,traits).
 */
template <typename T, typename TraitsT,
          typename =void>
struct has_nested
{
    constexpr static const bool value=false;
};

/**
 * @brief Check if traits implement method nested(id,traits,translator).
 */
template <typename T, typename TraitsT>
struct has_nested<T,TraitsT,
                  decltype(
                    (void)std::declval<std::decay_t<TraitsT>>().nested(
                        std::declval<std::decay_t<T>>()
                    )
                  )
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Construct member path for member name formating in case of direct order of levels in the path.
 * @param path Original member path.
 * @return Original member path.
 */
template <typename T, typename TraitsT>
constexpr auto member_names_path(T&& path, const TraitsT&,
                       std::enable_if_t<!TraitsT::is_reverse_member_names_order,void*> =nullptr) -> decltype(auto)
{
    return hana::id(std::forward<T>(path));
}

/**
 * @brief Construct member path for member name formating in case of reverse order of levels in the path.
 * @param path Original member path.
 * @return Member path with reversed order.
 */
template <typename T, typename TraitsT>
constexpr auto member_names_path(T&& path, const TraitsT&,
                       std::enable_if_t<TraitsT::is_reverse_member_names_order,void*> =nullptr)
{
    return hana::reverse(std::forward<T>(path));
}

/**
 * @brief Default helper to format a single key of member path with separator.
 */
template <typename T, typename TraitsT, typename =void>
struct member_name_with_separator_t
{
    auto operator() (T&& id, const TraitsT& traits, const grammar_categories& grammar_cats) const
    {
        auto name=single_member_name(std::forward<T>(id),traits,grammar_cats);
        auto name_grammar_cats=phrase_grammar_cats(name);
        return hana::make_tuple(std::move(name),translate(traits,to_string(traits.member_names_conjunction()),name_grammar_cats));
    }
};

/**
 * @brief Helper to format a single key of member path with separator when traits implement method member_name_with_separator(id,grammar_cats).
 */
template <typename T, typename TraitsT>
struct member_name_with_separator_t<T,TraitsT,
        decltype(
            (void)std::declval<std::decay_t<TraitsT>>().member_name_with_separator(std::declval<std::decay_t<T>>(),std::declval<grammar_categories>())
        )>
{
    auto operator() (T&& id, const TraitsT& traits, const grammar_categories& grammar_cats) const
    {
        return traits.member_name_with_separator(std::forward<T>(id),grammar_cats);
    }
};

template <typename T, typename TraitsT>
constexpr member_name_with_separator_t<T,TraitsT> member_name_with_separator_inst{};

/**
 * @brief Format a single key of member path with separator.
 * @param id Single key of member path.
 * @param traits Formatter traits.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Formatted phrase.
 */
template <typename T, typename TraitsT>
auto member_name_with_separator(T&& id, const TraitsT& traits, const grammar_categories& grammar_cats=0)
{
    return member_name_with_separator_inst<T,TraitsT>(std::forward<T>(id),traits,grammar_cats);
}

/**
 * @brief Format names of intermediate keys of member path without the last key.
 * @param id Member.
 * @param traits Formatter traits.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Tuple of formatted names of intermediate keys.
 */
template <typename T, typename TraitsT>
auto intermediate_member_names(const T& id, const TraitsT& traits, grammar_categories grammar_cats)
{
    return hana::fold(
        member_names_path(hana::drop_back(member_path(id)),traits),
        hana::tuple<>(),
        [&traits,&grammar_cats](auto&& prev_parts, auto&& current_key)
        {
            auto&& cats=last_grammar_categories(prev_parts,grammar_cats);
            return hana::concat(
                        std::forward<decltype(prev_parts)>(prev_parts),
                        member_name_with_separator(current_key,traits,cats)
                   );
        }
    );
}

/**
 * @brief Concat list of intermediate keys names with last key in case when the list is empty.
 * @param id Member.
 * @param traits Formatter traits.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Tuple with formatted last key of member path.
 */
template <typename T, typename TraitsT, typename Ts>
auto member_names_with_last_key(const T& id, const TraitsT& traits, Ts&&,
                          grammar_categories grammar_cats,
                          std::enable_if_t<
                          std::is_same<decltype(hana::tuple<>()),std::decay_t<Ts>>::value
                          ,void*> =nullptr)
{
    // format last key
    return hana::make_tuple(single_member_name(hana::back(member_path(id)),traits,grammar_cats));
}

/**
 * @brief Concat list of intermediate keys names with last key.
 * @param id Member.
 * @param traits Formatter traits.
 * @param ts Tuple of formatted names of intermediate keys.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Cpncatenation of formatted last key of member path and formatted names of intermediate keys.
 */
template <typename T, typename TraitsT, typename Ts>
auto member_names_with_last_key(const T& id, const TraitsT& traits, Ts&& ts,
                          grammar_categories grammar_cats,
                          std::enable_if_t<
                          !std::is_same<decltype(hana::tuple<>()),std::decay_t<Ts>>::value
                          ,void*> =nullptr)
{
    // format last key and prepend to the list
    return hana::concat(
                member_name_with_separator(hana::back(member_path(id)),traits,grammar_cats),
                std::forward<Ts>(ts)
           );
}

/**
 * @param Construct list of formatted names of keys of member path in reverse order.
 * @param id Member.
 * @param traits Formatter traits.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Tuple of formatted names of keys of member path.
 */
template <typename T, typename TraitsT>
auto list_member_names(const T& id, const TraitsT& traits, grammar_categories grammar_cats,
                       std::enable_if_t<
                       TraitsT::is_reverse_member_names_order
                       ,void*> =nullptr)
{
    // separator
    //! @todo Use grammar categories from formatted id to format separator instead of grammar_cats because the formatted id will be preceding to the separator.
    auto sep=translate(traits,to_string(traits.member_names_conjunction()),grammar_cats);

    // construct intermediate list using first grammar categories from the separator and drop last separator
    auto parts=hana::drop_back(intermediate_member_names(id,traits,phrase_grammar_cats(sep)));

    // construct final list
    return member_names_with_last_key(id,traits,std::move(parts),grammar_cats);
}

/**
 * @param Construct list of formatted names of keys of member path in direct order.
 * @param id Member.
 * @param traits Formatter traits.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Tuple of formatted names of keys of member path.
 */
template <typename T, typename TraitsT>
auto list_member_names(const T& id, const TraitsT& traits, grammar_categories grammar_cats,
                       std::enable_if_t<
                       !TraitsT::is_reverse_member_names_order
                       ,void*> =nullptr)
{
    // construct intermediate list
    auto parts=intermediate_member_names(id,traits,grammar_cats);

    // format last key and append to the internmediate list
    auto cats=last_grammar_categories(parts,grammar_cats);
    return hana::append(
                std::move(parts),
                single_member_name(hana::back(member_path(id)),traits,cats)
           );
}

/**
 * @brief Join list of formatted names of keys of member path to formatted string.
 * @param id Member.
 * @param traits Formatter traits.
 * @param grammar_cats Grammatical categories of preceding phrase.
 * @return Formatted full member name.
 */
template <typename T, typename TraitsT>
auto join_member_names(T&& id, const TraitsT& traits, grammar_categories grammar_cats)
{
    // extract member path if member has explicit names
    auto&& member=hana::if_(
                std::is_base_of<member_with_name_list_tag,std::decay_t<T>>{},
                [](auto&& id)
                {
                    // make pseudo member with preset names in path
                    return make_member(id.name());
                },
                [](auto&& id) -> decltype(auto)
                {
                    // compact member path if variadic property
                    return hana::if_(
                        typename std::decay_t<decltype(id)>::is_with_varg{},
                        [](auto&& id)
                        {
                            return inherit_member(compact_variadic_property(id.path()),id);
                        },
                        [](auto&& id) -> decltype(auto)
                        {
                            return hana::id(std::forward<decltype(id)>(id));
                        }
                    )(std::forward<decltype(id)>(id));
                }
            )(std::forward<T>(id));

    // list name parts
    auto parts=list_member_names(member,traits,grammar_cats);

    // join parts
    std::string dst;
    backend_formatter.append_join(
       dst,
       "",
       parts
    );

    // return result with grammar categories
    if (!TraitsT::is_reverse_member_names_order)
    {
        return concrete_phrase(dst,last_grammar_categories(parts,grammar_cats));
    }
    return concrete_phrase(dst,first_grammar_categories(parts,grammar_cats));
}

}

/**
 * @brief Default formatter of a member name to be used when member doesn't have explicit name and traits do not implement method nested(id,traits).
 */
template <typename T, typename TraitsT, typename =hana::when<true>>
struct nested_member_name_t
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const
    {
        return detail::join_member_names(id,traits,grammar_cats);
    }
};

/**
 * @brief Formatter of a member name to be used when the member has explicit name and it is not a concrete_phrase.
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
            hana::when<
                        std::is_base_of<member_with_name_tag,T>::value
                        &&
                        !std::is_base_of<member_with_name_list_tag,T>::value
                        &&
                        !std::is_same<decltype(std::declval<T>().name()),concrete_phrase>::value
                     >
        >
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const
    {
        return decorate(traits,translate(traits,id.name(),grammar_cats));
    }
};

/**
 * @brief Formatter of a member name to be used when the member has explicit name and it is a concrete_phrase.
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
            hana::when<
                std::is_base_of<member_with_name_tag,T>::value
                &&
                std::is_same<decltype(std::declval<T>().name()),concrete_phrase>::value
            >
        >
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories) const
    {
        return decorate(traits,id.name());
    }
};

/**
 * @brief Formatter of a member name that uses method nested(id,traits) of traits object.
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
            hana::when<detail::has_nested<T,TraitsT>::value>
        >
{
    auto operator() (const T& id, const TraitsT& traits, grammar_categories grammar_cats) const -> decltype(auto)
    {
        return traits.nested(id,traits,grammar_cats);
    }
};

/**
 * @brief Template instance of member name formatter.
 */
template <typename T, typename TraitsT>
constexpr nested_member_name_t<T,TraitsT> nested_member_name_inst{};

/**
 * @brief Process nested member name.
 * @param id Member.
 * @param traits Traits of member names formatter.
 * @return Processed member name.
 */
template <typename T, typename TraitsT>
constexpr auto nested_member_name(const T& id, const TraitsT& traits, grammar_categories grammar_cats=0) -> decltype(auto)
{
    return nested_member_name_inst<T,TraitsT>(id,traits,grammar_cats);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP
