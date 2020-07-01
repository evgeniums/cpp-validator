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
#include <dracosha/validator/reporting/translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct member_names_tag;

struct bypass_member_names_traits
{
};

template <typename StringsT,typename TraitsT>
struct member_names_t
{
    using hana_tag=member_names_tag;

    const StringsT& _strings;
    TraitsT _traits;

    template <typename T>
    constexpr std::string operator() (const T& name) const
    {
        auto res=_traits(name);
        if (!res.empty())
        {
            return res;
        }
        return _strings(name);
    }
};

template <typename StringsT>
struct member_names_t<StringsT,bypass_member_names_traits>
{
    using hana_tag=member_names_tag;

    const StringsT& _strings;

    template <typename T>
    constexpr std::string operator() (const T& name) const
    {
        return _strings(name);
    }
};

template <typename TraitsT,typename StringsT>
constexpr auto member_names(TraitsT&& traits, const StringsT& strings,
                            std::enable_if_t<hana::is_a<strings_tag,StringsT>,void*> =nullptr)
{
    return member_names_t<
                std::decay_t<StringsT>,
                std::decay_t<TraitsT>
            >
            {strings,std::forward<TraitsT>(traits)};
}

template <typename StringsT>
constexpr auto member_names(const StringsT& strings,
                                std::enable_if_t<hana::is_a<strings_tag,StringsT>,void*> =nullptr
                            )
{
    return member_names_t<
                std::decay_t<StringsT>,
                bypass_member_names_traits
            >
            {strings};
}

template <typename TraitsT>
constexpr auto member_names(T&& traits,
                                std::enable_if_t<(!hana::is_a<strings_tag,TraitsT> && !hana::is_a<translator_tag,TraitsT>),void*> =nullptr
                            )
{
    return member_names(default_strings,traits);
}

constexpr auto member_names()
{
    return member_names(default_strings);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAMES_HPP
