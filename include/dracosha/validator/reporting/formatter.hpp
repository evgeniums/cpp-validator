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
#include <dracosha/validator/cref.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/reporting/order_and_presentation.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#include <dracosha/validator/detail/formatter_fmt.hpp>
#else
//! \todo use std string formatting
#endif

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

#ifdef DRACOSHA_VALIDATOR_FMT
constexpr detail::fmt_append_t format_append{};
constexpr detail::fmt_prepend_t format_prepend{};
#else
//! \todo use std string formatting
#endif

//-------------------------------------------------------------

struct formatter_tag;

/**
 *  @brief Formatter that uses fmt library as backend.
 *
 * Destination object is normally a std::string.
 * Also it can be a container of chars that can deal with inserter iterators,
 * i.e. it must be suitable for using std::back_inserter(dst), dst.insert(), dst.begin(), dst.end().
 *
 */
template <typename MemberNamesT, typename ValuesT, typename StringsT, typename OrderAndPresentationT>
struct formatter_t
{
    using hana_tag=formatter_tag;

    MemberNamesT _member_names;
    ValuesT _values;
    const StringsT& _strings;
    OrderAndPresentationT _order_and_presentation;

    template <typename DstT, typename T2, typename OpT>
    void validate_operator(DstT& dst,const OpT& op, const T2& b) const
    {
        append(dst,
               make_cref_tuple(_strings,_values),
               op,b
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT>
    void validate_property(DstT& dst,const PropT& prop, const OpT& op, const T2& b) const
    {
        append(dst,
               make_cref_tuple(_member_names,_strings,_values),
               prop,op,b
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b) const
    {
        append(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_values),
               prop,member,op,b
               );
    }

    template <typename DstT, typename T2, typename MemberT>
    void validate_exists(DstT& dst, const MemberT& member, const T2& b) const
    {
        if (b)
        {
            append(dst,
                   make_cref_tuple(_member_names,_strings),
                   member,string_exists
                   );
        }
        else
        {
            append(dst,
                   make_cref_tuple(_member_names,_strings),
                   member,string_not_exists
                   );
        }
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_other_member(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b) const
    {
        append(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_member_names),
               prop,member,op,b
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_master_sample(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2&) const
    {
        append(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_strings),
               prop,member,op,string_master_sample
               );
    }

    template <typename DstT>
    void validate_and(DstT& dst) const
    {
        prepend(dst,
                make_cref_tuple(_strings),
                string_and
                );
    }

    template <typename DstT, typename MemberT>
    void validate_and(DstT& dst, const MemberT& member) const
    {
        prepend(dst,
                make_cref_tuple(_strings,_member_names),
                string_and,member
                );
    }

    template <typename DstT>
    void validate_or(DstT& dst) const
    {
        prepend(dst,
                make_cref_tuple(_strings),
                string_or
                );
    }

    template <typename DstT, typename MemberT>
    void validate_or(DstT& dst, const MemberT& member) const
    {
        prepend(dst,
                make_cref_tuple(_strings,_member_names),
                string_or,member
                );
    }

    template <typename DstT>
    void validate_not(DstT& dst) const
    {
        prepend(dst,
                make_cref_tuple(_strings),
                string_not
                );
    }

    template <typename DstT, typename MemberT>
    void validate_not(DstT& dst, const MemberT& member) const
    {
        prepend(dst,
                make_cref_tuple(_strings,_member_names),
                string_not,member
                );
    }

    private:

        template <typename DstT,typename ...Args>
        void append(DstT& dst, Args&&... args) const
        {
            _order_and_presentation(
                        hana::partial(format_append,hana::type_c<DstT>,std::ref(dst)),
                        std::forward<Args>(args)...
                        );
        }

        template <typename DstT,typename ...Args>
        void prepend(DstT& dst, Args&&... args) const
        {
            _order_and_presentation(
                        hana::partial(format_prepend,hana::type_c<DstT>,std::ref(dst)),
                        std::forward<Args>(args)...
                        );
        }
};

/**
 * @brief Create formatter that doesn't own member names but holds a const reference instead
 * @param mn Const reference to formatter of member names
 * @param vs Const reference to formatter of values
 * @param strings Const reference to strings
 * @param order Reordering and presentation helper
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT,typename StringsT, typename OrderAndPresentationT>
auto formatter_with_references(const MemberNamesT& mn, const ValuesT& vs, const StringsT& strings, OrderAndPresentationT&& order)
{
    return formatter_t<
                const MemberNamesT&,
                const ValuesT&,
                StringsT,
                std::decay_t<OrderAndPresentationT>
            >
            {mn,vs,strings,std::forward<OrderAndPresentationT>(order)};
}

/**
 * @brief Create formatter that owns member names and value strings
 * @param mn Formatter of member names
 * @param vs Formatter of values
 * @param strings Strings
 * @param order Reordering and presentation helper
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT,typename StringsT, typename OrderAndPresentationT>
auto formatter(MemberNamesT&& mn, ValuesT&& vs, const StringsT& strings, OrderAndPresentationT&& order)
{
    return formatter_t<
                std::decay_t<MemberNamesT>,
                std::decay_t<ValuesT>,
                StringsT,
                std::decay_t<OrderAndPresentationT>
            >
            {std::forward<MemberNamesT>(mn),std::forward<ValuesT>(vs),strings,std::forward<OrderAndPresentationT>(order)};
}

/**
 * @brief Create formatter that doesn't own member names but holds a const reference instead
 * @param mn Const reference to formatter of member names
 * @param vs Const reference to formatter of values
 * @param strings Const reference to strings
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT,typename StringsT>
auto formatter_with_references(const MemberNamesT& mn, const ValuesT& vs, const StringsT& strings)
{
    return formatter_with_references(mn,vs,strings,default_order_and_presentation);
}

/**
 * @brief Create formatter that owns member names and value strings
 * @param mn Formatter of member names
 * @param vs Formatter of values
 * @param strings Strings
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT,typename StringsT>
auto formatter(MemberNamesT&& mn, ValuesT&& vs, const StringsT& strings)
{
    return formatter(std::forward<MemberNamesT>(mn),std::forward<ValuesT>(vs),strings,default_order_and_presentation);
}

/**
 * @brief Create formatter that owns member names and uses default strings
 * @param mn Formatter of member names
 * @param vs Formatter of values
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT>
auto formatter(MemberNamesT&& mn, ValuesT&& vs)
{
    return formatter(std::forward<MemberNamesT>(mn),std::forward<ValuesT>(vs),default_strings);
}

/**
 * @brief Create formatter that owns member names and uses default strings and value strings
 * @param mn Formatter of member names
 * @return Formatter
 */
template <typename MemberNamesT>
auto formatter(MemberNamesT&& mn,
               std::enable_if_t<hana::is_a<member_names_tag,MemberNamesT>,void*> =nullptr)
{
    return formatter(std::forward<MemberNamesT>(mn),values,default_strings);
}

/**
 * @brief Create formatter that bypasses member names and uses default value strings
 * @param strings Strings
 * @return Formatter
 */
template <typename StringsT>
auto formatter(const StringsT& strings,
               std::enable_if_t<hana::is_a<strings_tag,StringsT>,void*> =nullptr)
{
    return formatter(member_names(strings),values,strings);
}

/**
 * @brief Create formatter that bypasses member names and uses default strings
 * @return Formatter
 */
inline auto formatter()
{
    return formatter(member_names(),values,default_strings);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_HPP
