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

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/single_member_name.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename T, typename TraitsT, typename StringsT,
          typename =void>
struct has_nested
{
    constexpr static const bool value=false;
};

template <typename T, typename TraitsT, typename StringsT>
struct has_nested<T,TraitsT,StringsT,
                  decltype(
                    (void)std::declval<std::decay_t<TraitsT>>().nested(
                        std::declval<std::decay_t<T>>(),
                        std::declval<std::decay_t<StringsT>>()
                    )
                  )
        >
{
    constexpr static const bool value=true;
};

}

template <typename T, typename TraitsT, typename StringsT, typename =hana::when<true>>
struct nested_member_name_t
{
    std::string operator() (const T& id, const TraitsT& traits, const StringsT& strings) const
    {
        std::string dst;
        backend_formatter.append_join(
            dst,
            strings.member_names_conjunction(),
            hana::transform(
                            hana::reverse(id.path),
                            [&](const auto& current_key)
                            {
                                return single_member_name<decltype(current_key),TraitsT>(current_key,traits,strings);
                            }
                          )
        );
        return dst;
    }
};

template <typename T, typename TraitsT, typename StringsT>
struct nested_member_name_t<T,TraitsT,StringsT,
        hana::when<detail::has_nested<T,TraitsT,StringsT>::value>>
{
    std::string operator() (const T& id, const TraitsT& traits, const StringsT& strings) const
    {
        return traits.nested(id,traits,strings);
    }
};

template <typename T, typename TraitsT, typename StringsT>
constexpr nested_member_name_t<T,TraitsT,StringsT> nested_member_name{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NESTED_MEMBER_NAME_HPP
