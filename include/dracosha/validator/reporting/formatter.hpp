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
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>

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

/**
 * @brief Create formatter that doesn't own member names but holds a const reference instead
 * @param mn Const reference to member names
 * @param strings Const reference to strings
 * @return Formatter
 */
template <typename MemberNamesT,typename StringsT>
auto formatter_reference_names(const MemberNamesT& mn, const StringsT& strings)
{
    return formatter_t<
                const MemberNamesT&,
                StringsT
            >
            {mn,strings};
}

/**
 * @brief Create formatter that owns member names
 * @param mn Member names
 * @param strings Strings
 * @return Formatter
 */
template <typename MemberNamesT,typename StringsT>
auto formatter(MemberNamesT&& mn, const StringsT& strings)
{
    return formatter_t<
                std::decay_t<MemberNamesT>,
                StringsT
            >
            {std::forward<MemberNamesT>(mn),strings};
}

/**
 * @brief Create formatter that owns member names and uses default strings
 * @param mn Member names
 * @return Formatter
 */
template <typename MemberNamesT>
auto formatter(MemberNamesT&& mn)
{
    return formatter(std::forward<MemberNamesT>(mn),default_strings);
}

/**
 * @brief Create formatter that bypasses member names
 * @param strings Strings
 * @return Formatter
 */
template <typename StringsT>
auto formatter(const StringsT& strings)
{
    return formatter(member_names(strings),strings);
}

/**
 * @brief Create formatter that bypasses member names and uses default strings
 * @return Formatter
 */
inline auto formatter()
{
    return formatter(member_names(),default_strings);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_HPP
