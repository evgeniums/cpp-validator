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
constexpr auto member_names_path(const T& id, const TraitsT&,
                       std::enable_if_t<!TraitsT::is_reverse_member_names_order(),void*> =nullptr)
                -> std::add_lvalue_reference_t<
                        std::add_const_t<
                            std::decay_t<decltype(id.path)>
                        >
                    >
{
    return id.path;
}

template <typename T, typename TraitsT>
constexpr auto member_names_path(const T& id, const TraitsT&,
                       std::enable_if_t<TraitsT::is_reverse_member_names_order(),void*> =nullptr) -> decltype(auto)
{
    return hana::reverse(id.path);
}

template <typename T, typename TraitsT>
auto join_member_names(const T& id, const TraitsT& traits)
{
    std::string dst;
    backend_formatter.append_join(
       dst,
       translate(traits,detail::to_string(traits.member_names_conjunction())),
       hana::transform(
                       member_names_path(id,traits),
                       [&](const auto& current_key)
                       {
                           return single_member_name(current_key,traits);
                       }
                     )
    );
    return dst;
}

}

/**
 * @brief Default formatter of a member name to be used when member doesn't have explicit name and traits do not implement method nested(id,traits,translator)
 */
template <typename T, typename TraitsT, typename =hana::when<true>>
struct nested_member_name_t
{
    auto operator() (const T& id, const TraitsT& traits) const
    {
        return detail::join_member_names(id,traits);
    }
};

/**
 * @brief Formatter of a member name to be used when the member has explicit name.
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
            hana::when<T::has_name()>
        >
{
    auto operator() (const T& id, const TraitsT& traits) const
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
    auto operator() (const T& id, const TraitsT& traits) const -> decltype(auto)
    {
        return traits.nested(id,traits);
    }
};

/**
 * @brief Formatter of a member name to be used when name of the last key of the member can be converted to concrete phrase.
 */
template <typename T, typename TraitsT>
struct nested_member_name_t<T,TraitsT,
        hana::when<
            !detail::has_nested<T,TraitsT>::value
            &&
            std::is_same<
                decltype(
                    single_member_name(
                        std::declval<std::decay_t<decltype(std::declval<T>().key())>>(),
                        std::declval<TraitsT>()
                        )
                ),
                concrete_phrase
            >::value
        >>
{
    auto operator() (const T& id, const TraitsT& traits) const
    {
        return concrete_phrase(detail::join_member_names(id,traits),single_member_name(id.key(),traits).attributes());
    }
};

template <typename T, typename TraitsT>
constexpr nested_member_name_t<T,TraitsT> nested_member_name_inst{};

template <typename T, typename TraitsT>
constexpr auto nested_member_name(const T& id, const TraitsT& traits) -> decltype(auto)
{
    return nested_member_name_inst<T,TraitsT>(id,traits);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP
