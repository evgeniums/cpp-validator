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
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_name.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/reporting/order_and_presentation.hpp>
#include <dracosha/validator/reporting/report_aggregation.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct formatter_tag;

/**
 *  @brief Formatter that formats report and puts it to destination object.
 *
 *  Formatter uses four different traits objects that can be customized:
 *  - formatter of member names (MemberNamesT);
 *  - formatter of values (ValuesT);
 *  - formatter of miscellaneous strings such as operator descriptions, etc. (StringsT);
 *  - traits that perform final ordering and presentation (OrderAndPresentationT).
 *
 * Actual string formatting is performed by a backend formatter that wraps destination object which
 * is given as the first argument to formatting methods of this formatter (i.e., DstT& dst). By default
 * either std::stringstream or libfmt backend formatter is used depending on the configuration. Libfmt is
 * a preferred backend formatter but it requires a fmt library available in include paths and DRACOSHA_VALIDATOR_FMT
 * macro defined.
 *
 */
template <typename MemberNamesT, typename ValuesT, typename StringsT, typename OrderAndPresentationT>
struct formatter
{
    using hana_tag=formatter_tag;

    using member_names_type=MemberNamesT;
    using values_type=ValuesT;
    using strings_type=StringsT;
    using order_and_presentation_type=OrderAndPresentationT;

    MemberNamesT _member_names;
    ValuesT _values;
    StringsT _strings;
    OrderAndPresentationT _order_and_presentation;

    template <typename DstT, typename T2, typename OpT>
    void validate_operator(DstT& dst,const OpT& op, const T2& b) const
    {
        format(dst,
               make_cref_tuple(_strings,_values),
               op,b
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT>
    void validate_property(DstT& dst,const PropT& prop, const OpT& op, const T2& b) const
    {
        format(dst,
               make_cref_tuple(_member_names,_strings,_values),
               prop,op,b
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b) const
    {
        format(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_values),
               member,prop,op,b
               );
    }

    template <typename DstT, typename T2, typename OpT, typename MemberT>
    void validate_exists(DstT& dst, const MemberT& member, const OpT& op, const T2& b) const
    {
        format(dst,
               make_cref_tuple(_member_names,_strings),
               member,op.str(b)
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_other_member(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b) const
    {
        format(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_member_names),
               member,prop,op,make_member_name(b)
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_master_sample(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2&,
                                     std::enable_if_t<!hana::is_a<operand_tag,T2>,void*> =nullptr) const
    {
        format(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_strings),
               member,prop,op,string_master_sample
               );
    }

    template <typename DstT, typename T2, typename OpT, typename PropT, typename MemberT>
    void validate_with_master_sample(DstT& dst, const MemberT& member, const PropT& prop, const OpT& op, const T2& b,
                                     std::enable_if_t<hana::is_a<operand_tag,T2>,void*> =nullptr) const
    {
        format(dst,
               make_cref_tuple(_member_names,_member_names,_strings,_values),
               member,prop,op,b
               );
    }

    template <typename DstT, typename ItemT>
    void aggregate(DstT& dst, const ItemT& item) const
    {
        format(dst,_strings,item);
    }

    template <typename MemberT>
    std::string member_to_string(const MemberT& member) const
    {
        return _member_names(member);
    }

    private:

        template <typename DstT,typename ...Args>
        void format(DstT& dst, Args&&... args) const
        {
            _order_and_presentation(
                        dst,
                        std::forward<Args>(args)...
                        );
        }
};

/**
 * @brief Create formatter
 * @param mn Formatter of member names
 * @param vs Formatter of values
 * @param strings Strings traits
 * @param order Reordering and presentation traits
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT,typename StringsT, typename OrderAndPresentationT>
auto make_formatter(MemberNamesT&& mn, ValuesT&& vs, StringsT&& strings, OrderAndPresentationT&& order)
{
    return formatter<
                MemberNamesT,
                ValuesT,
                StringsT,
                OrderAndPresentationT
            >
            {std::forward<MemberNamesT>(mn),std::forward<ValuesT>(vs),std::forward<StringsT>(strings),std::forward<OrderAndPresentationT>(order)};
}

/**
 * @brief Create formatter with default order and presentation traits
 * @param mn Formatter of member names
 * @param vs Formatter of values
 * @param strings Strings traits
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT,typename StringsT>
auto make_formatter(MemberNamesT&& mn, ValuesT&& vs, StringsT&& strings)
{
    return make_formatter(std::forward<MemberNamesT>(mn),std::forward<ValuesT>(vs),std::forward<StringsT>(strings),default_order_and_presentation);
}

/**
 * @brief Create formatter with default strings traits and order and presentation traits
 * @param mn Formatter of member names
 * @param vs Formatter of values
 * @return Formatter
 */
template <typename MemberNamesT,typename ValuesT>
auto make_formatter(MemberNamesT&& mn, ValuesT&& vs)
{
    return make_formatter(std::forward<MemberNamesT>(mn),std::forward<ValuesT>(vs),default_strings);
}

/**
 * @brief Create formatter with default values formatter, strings traits and order and presentation traits
 * @param mn Formatter of member names
 * @return Formatter
 */
template <typename MemberNamesT>
auto make_formatter(MemberNamesT&& mn,
               std::enable_if_t<hana::is_a<member_names_tag,MemberNamesT>,void*> =nullptr)
{
    return make_formatter(std::forward<MemberNamesT>(mn),default_values,default_strings);
}

/**
 * @brief Create formatter with default values formatter, member names formatter and order and presentation traits
 * @param strings Strings traits
 * @return Formatter
 */
template <typename StringsT>
auto make_formatter(const StringsT& strings,
               std::enable_if_t<hana::is_a<strings_tag,StringsT>,void*> =nullptr)
{
    return make_formatter(make_member_names(strings),default_values,strings);
}

/**
 * @brief Get defautt formatter with default traits
 * @return Default formatter
 */
inline auto get_default_formatter() ->
    std::add_lvalue_reference_t<
        std::add_const_t<decltype(make_formatter(get_default_member_names(),default_values,default_strings))>
    >
{
    static const auto default_formatter=make_formatter(get_default_member_names(),default_values,default_strings);
    return default_formatter;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_HPP
