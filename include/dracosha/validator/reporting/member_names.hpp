/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/member_names.hpp
*
*
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
#define DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/strings.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct member_names_tag;

template <typename TraitsT,typename StringsT>
struct member_names_t
{
    using hana_tag=member_names_tag;

    const TraitsT& _traits;
    const StringsT& _strings;

    template <typename T>
    constexpr const char* operator() (const T& name) const
    {
        auto res=_traits(name);
        if (res!=nullptr)
        {
            return res;
        }
        return _strings(name);
    }
};

struct default_member_names_traits
{
    template <typename T>
    constexpr const char* operator() (T&&) const
    {
        return nullptr;
    }
};

template <typename TraitsT,typename StringsT>
constexpr auto member_names(TraitsT&& traits, StringsT&& strings) -> decltype(auto)
{
    return member_names_t<
                std::decay_t<TraitsT>,
                std::decay_t<StringsT>
            >
            {std::forward<TraitsT>(traits),std::forward<StringsT>(strings)};
}

template <typename TraitsT>
constexpr auto member_names(TraitsT&& traits) -> decltype(auto)
{
    return member_names(std::forward<TraitsT>(traits),default_strings);
}

constexpr auto default_member_names=member_names(default_member_names_traits{});

constexpr auto member_names() -> decltype(std::add_lvalue_reference_t<
                                        std::add_const_t<
                                            std::decay_t<decltype(default_member_names)>
                                        >
                                >)
{
    return default_member_names;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
