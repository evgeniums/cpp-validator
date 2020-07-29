/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/nested_member_name.hpp
*
* Defines helper to format nested member names.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP
#define DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/reporting/single_member_name.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief String to use for joining nested member names in member's path
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
 * @brief Check if traits do not implement method nested(id,traits,translator)
 */
template <typename T, typename TraitsT,
          typename =void>
struct has_nested
{
    constexpr static const bool value=false;
};

/**
 * @brief Check if traits implement method nested(id,traits,translator)
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

template <typename T, typename TraitsT>
constexpr auto member_names_path(T&& path, const TraitsT&,
                       std::enable_if_t<!TraitsT::is_reverse_member_names_order,void*> =nullptr) -> decltype(auto)
{
    return hana::id(std::forward<T>(path));
}

template <typename T, typename TraitsT>
constexpr auto member_names_path(T&& path, const TraitsT&,
                       std::enable_if_t<TraitsT::is_reverse_member_names_order,void*> =nullptr)
{
    return hana::reverse(std::forward<T>(path));
}

template <typename T, typename TraitsT, typename =void>
struct member_name_with_separator_t
{
    auto operator() (T&& id, const TraitsT& traits, const word_attributes& attributes) const
    {
        auto name=single_member_name(std::forward<T>(id),traits,attributes);
        auto name_attributes=phrase_attributes(name);
        return hana::make_tuple(std::move(name),translate(traits,detail::to_string(traits.member_names_conjunction()),name_attributes));
    }
};

template <typename T, typename TraitsT>
struct member_name_with_separator_t<T,TraitsT,
        decltype(
            (void)std::declval<std::decay_t<T>>().member_name_with_separator(std::declval<std::decay_t<T>>(),std::declval<word_attributes>())
        )>
{
    auto operator() (T&& id, const TraitsT& traits, const word_attributes& attributes) const
    {
        return traits.member_name_with_separator(std::forward<T>(id),attributes);
    }
};

template <typename T, typename TraitsT>
constexpr member_name_with_separator_t<T,TraitsT> member_name_with_separator_inst{};

template <typename T, typename TraitsT>
auto member_name_with_separator(T&& id, const TraitsT& traits, const word_attributes& attributes=0)
{
    return member_name_with_separator_inst<T,TraitsT>(std::forward<T>(id),traits,attributes);
}

template <typename T, typename TraitsT>
auto intermediate_member_names(const T& id, const TraitsT& traits, word_attributes attributes)
{
    // format intermediate keys without the last key
    return hana::fold(
        member_names_path(hana::drop_back(id.path),traits),
        hana::tuple<>(),
        [&traits,&attributes](auto&& prev_parts, auto&& current_key)
        {
            auto&& attrs=last_word_attributes(prev_parts,attributes);
            return hana::concat(
                        std::forward<decltype(prev_parts)>(prev_parts),
                        member_name_with_separator(current_key,traits,attrs)
                   );
        }
    );
}

template <typename T, typename TraitsT, typename Ts>
auto reverse_member_names(const T& id, const TraitsT& traits, Ts&&,
                          std::enable_if_t<
                          std::is_same<decltype(hana::tuple<>()),std::decay_t<Ts>>::value
                          ,void*> =nullptr)
{
    // format last key
    return hana::make_tuple(single_member_name(hana::back(id.path),traits));
}

template <typename T, typename TraitsT, typename Ts>
auto reverse_member_names(const T& id, const TraitsT& traits, Ts&& ts,
                          std::enable_if_t<
                          !std::is_same<decltype(hana::tuple<>()),std::decay_t<Ts>>::value
                          ,void*> =nullptr)
{
    // format last key and prepend to the list
    return hana::concat(
                member_name_with_separator(hana::back(id.path),traits),
                std::forward<Ts>(ts)
           );
}

template <typename T, typename TraitsT>
auto list_member_names(const T& id, const TraitsT& traits, word_attributes attributes,
                       std::enable_if_t<
                       TraitsT::is_reverse_member_names_order
                       ,void*> =nullptr)
{
    // construct intermediate list and drop last separator
    auto parts=hana::drop_back(intermediate_member_names(id,traits,attributes));

    // construct reversed list
    return reverse_member_names(id,traits,std::move(parts));
}

template <typename T, typename TraitsT>
auto list_member_names(const T& id, const TraitsT& traits, word_attributes attributes,
                       std::enable_if_t<
                       !TraitsT::is_reverse_member_names_order
                       ,void*> =nullptr)
{
    // construct intermediate list
    auto parts=intermediate_member_names(id,traits,attributes);

    // format last key and append to the internmediate list
    auto attrs=last_word_attributes(parts,attributes);
    return hana::append(
                std::move(parts),
                single_member_name(hana::back(id.path),traits,attrs)
           );
}

template <typename T, typename TraitsT>
auto join_member_names(const T& id, const TraitsT& traits, word_attributes attributes)
{
    auto parts=list_member_names(id,traits,attributes);
    std::string dst;
    backend_formatter.append_join(
       dst,
       "",
       parts
    );
    return concrete_phrase(dst,last_word_attributes(parts,attributes));
}

}

/**
 * @brief Default formatter of a member name to be used when member doesn't have explicit name and traits do not implement method nested(id,traits)
 */
template <typename T, typename TraitsT, typename =hana::when<true>>
struct nested_member_name_t
{
    auto operator() (const T& id, const TraitsT& traits, word_attributes attributes) const
    {
        return detail::join_member_names(id,traits,attributes);
    }
};

/**
 * @brief Formatter of a member name to be used when the member has explicit name
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
            hana::when<T::has_name>
        >
{
    auto operator() (const T& id, const TraitsT& traits, word_attributes) const
    {
        return decorate(traits,id.name());
    }
};

/**
 * @brief Formatter of a member name that uses method nested(id,traits) of traits object
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
            hana::when<detail::has_nested<T,TraitsT>::value>
        >
{
    auto operator() (const T& id, const TraitsT& traits, word_attributes attributes) const -> decltype(auto)
    {
        return traits.nested(id,traits,attributes);
    }
};

template <typename T, typename TraitsT>
constexpr nested_member_name_t<T,TraitsT> nested_member_name_inst{};

/**
 * @brief Process nested member name
 * @param id Member
 * @param traits Traits of member names formatter
 * @return Processed member name
 */
template <typename T, typename TraitsT>
constexpr auto nested_member_name(const T& id, const TraitsT& traits, word_attributes attributes=0) -> decltype(auto)
{
    return nested_member_name_inst<T,TraitsT>(id,traits,attributes);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP
