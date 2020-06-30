/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/formatter_fmt.hpp
*
*  Defines formatter that uses fmt library for strings formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
#define DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP

#include <fmt/format.h>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
    template <typename DstT, typename ...Args>
    void format_append(DstT& dst, Args&&... args)
    {
        if (!dst.empty())
        {
            fmt::format_to(std::back_inserter(dst),", ");
        }
        fmt::format_to(std::back_inserter(dst),std::forward<Args>(args)...);
    }

    template <typename DstT, typename ...Args>
    void format_prepend(DstT& dst, Args&&... args)
    {
        typename std::decay<DstT>::type tmp;
        format_append(tmp,std::forward<Args>(args)...);
        dst.insert(dst.begin(),tmp.begin(),tmp.end());
    }
}

struct formatter_tag;

/**
 *  @brief Formatter that uses fmt library as backend.
 *
 * Destination object is normally a std::string.
 * Also it can be a container of chars that can deal with inserter iterators,
 * i.e. it must be suitable for using std::back_inserter(dst), dst.insert(), dst.begin(), dst.end().
 *
 */
template <typename StringsT, typename MemberNamesT>
struct formatter_fmt_t
{
    using hana_tag=formatter_tag;

    const StringsT& _strings;
    const MemberNamesT& _member_names;

    template <typename DstT, typename T2, typename OpT>
    void validate_operator(DstT& dst,const OpT& op, const T2& b) const
    {
        detail::format_append(dst,"{} {}",_strings(op),b);
    }

    template <typename DstT, typename OpT, typename PropT>
    void validate_property(DstT& dst,const PropT& prop, const OpT& op, const T2& b) const
    {
        detail::format_append(dst,"{} {} {}",_member_names(prop),_strings(op),b);
    }

    template <typename DstT, typename T2, typename MemberT>
    void validate_exists(DstT& dst, const MemberT& member, const T2& b) const
    {
        if (b)
        {
            detail::format_append(dst,"{} {}",_member_names(member),_strings(string_exists));
        }
        else
        {
            detail::format_append(dst,"{} {}",_member_names(member),_strings(string_not_exists));
        }
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b) const
    {
        detail::format_append(dst,"{} {} {} {}",_member_names(prop),_member_names(member),_strings(op),b);
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_other_member(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b) const
    {
        detail::format_append(dst,"{} {} {} {}",_member_names(prop),_member_names(member),_strings(op),_member_names(b));
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_master_reference(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2&) const
    {
        detail::format_append(dst,"{} {} {} {}",_member_names(prop),_member_names(member),_strings(op),_strings(string_master_reference));
    }

    template <typename DstT>
    void validate_and(DstT& dst) const
    {
        detail::format_prepend(dst,"{}: ",_strings(string_and));
    }

    template <typename DstT, typename MemberT>
    void validate_and(DstT& dst, const MemberT& member) const
    {
        detail::format_prepend(dst,"{} {}: ",_strings(string_and),_member_names(member));
    }

    template <typename DstT>
    void validate_or(DstT& dst) const
    {
        detail::format_prepend(dst,"{}: ",_strings(string_or));
    }

    template <typename DstT, typename MemberT>
    void validate_or(DstT& dst, const MemberT& member) const
    {
        detail::format_prepend(dst,"{} {}: ",_strings(string_or),_member_names(member));
    }

    template <typename DstT>
    void validate_not(DstT& dst) const
    {
        detail::format_prepend(dst,"{}: ",_strings(string_not));
    }

    template <typename DstT, typename MemberT>
    void validate_not(DstT& dst, const MemberT& member) const
    {
        detail::format_prepend(dst,"{} {}: ",_strings(string_not),_member_names(member));
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
