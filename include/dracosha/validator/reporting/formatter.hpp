/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/formatter.hpp
*
*  Defines formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMATTER_HPP
#define DRACOSHA_VALIDATOR_FORMATTER_HPP

#include <dracosha/validator/config.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#include <dracosha/validator/reporting/formatter_fmt.hpp>
#else
//! \todo use std string formatting
#endif

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

#ifdef DRACOSHA_VALIDATOR_FMT
template <typename MemberNamesT, typename StringsT>
using formatter_t=formatter_fmt_t<MemberNamesT,StringsT>;
#else
//! \todo use std string formatting
#endif

//-------------------------------------------------------------

template <typename MemberNamesT,typename StringsT>
constexpr auto formatter_reference_names(const MemberNamesT& mn, const StringsT& strings)
{
    return formatter_t<
                const MemberNamesT&,
                StringsT
            >
            {mn,strings};
}

template <typename MemberNamesT,typename StringsT>
constexpr auto formatter(MemberNamesT&& mn, const StringsT& strings)
{
    return formatter_t<
                std::decay_t<MemberNamesT>,
                StringsT
            >
            {std::forward(mn),strings};
}

template <typename MemberNamesT>
constexpr auto formatter(MemberNamesT&& mn)
{
    return formatter(std::move<MemberNamesT>(mn),default_strings);
}

template <typename StringsT>
constexpr auto formatter(const StringsT& strings)
{
    return formatter(member_names(strings),strings);
}

constexpr auto formatter()
{
    return formatter(member_names(),default_strings);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_HPP
