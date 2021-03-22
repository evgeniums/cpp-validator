/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/reorder_and_present.hpp
*
*  Defines helpers for adjusting strings order and presentation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
#define DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/operators/op_report_without_operand.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_operand.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/report_aggregation.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>
#include <dracosha/validator/reporting/format_join_grammar_cats.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

//-------------------------------------------------------------

// Hepers for getting single formatters form formatters tuple.

template <typename FormatterTs>
auto arp_first_formatter(FormatterTs&& formatters) -> decltype(auto)
{
    return hana::at(formatters,hana::size_c<0>);
}
template <typename FormatterTs>
auto arp_second_formatter(FormatterTs&& formatters) -> decltype(auto)
{
    return hana::at(formatters,hana::size_c<1>);
}

template <typename FormatterTs>
auto arp_mn_formatter(FormatterTs&& formatters) -> decltype(auto)
{
    return arp_first_formatter(formatters);
}
template <typename FormatterTs>
auto arp_strings_formatter(FormatterTs&& formatters) -> decltype(auto)
{
    return arp_second_formatter(formatters);
}
template <typename FormatterTs>
auto arp_operands_formatter(FormatterTs&& formatters) -> decltype(auto)
{
    return hana::at(formatters,hana::size_c<2>);
}

//-------------------------------------------------------------

/**
 * @brief Apply presentation and order of validation report for 1 argument, which must be an aggregation operator.
 */
template <typename AggregationItemT>
struct apply_reorder_present_1arg_t
{
    template <typename DstT, typename StringsT>
    void operator () (
                        DstT& dst,
                        const StringsT& strings,
                        const AggregationItemT& aggregation_item
                     ) const
    {
        if ((!aggregation_item.single && aggregation_item.parts.size()>1)
                ||
                aggregation_item.aggregation.id==aggregation_id::NOT
                ||
                aggregation_item.aggregation.id==aggregation_id::ANY
                ||
                aggregation_item.aggregation.id==aggregation_id::ALL
            )
        {
            backend_formatter.append(
                dst,
                strings.aggregation_open(aggregation_item.aggregation)
            );
        }
        backend_formatter.append_join(
            dst,
            strings.aggregation_conjunction(aggregation_item.aggregation),
            aggregation_item.parts
        );
        if (!aggregation_item.single && aggregation_item.parts.size()>1)
        {
            backend_formatter.append(
                dst,
                strings.aggregation_close(aggregation_item.aggregation)
            );
        }
    }
};

//-------------------------------------------------------------

/**
 * @brief Apply presentation and order of validation report for 2 arguments without property and without member.
 */
template <typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_2args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst,
                        FormatterTs&& formatters,
                        const OpT& op,
                        const T2& b
                     ) const
    {
        format_join_grammar_cats(dst,
            fmt_pair(arp_first_formatter(formatters),op),
            fmt_pair(arp_second_formatter(formatters),b)
        );
    }
};

template <typename OpT, typename T2>
struct apply_reorder_present_2args_t<
                        OpT,T2,
                        hana::when<
                            std::is_base_of<op_report_without_operand_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const OpT& op, const T2& b
                    ) const
    {
        format_join_grammar_cats(dst,
            fmt_pair(arp_first_formatter(formatters),op.str(b))
        );
    }
};

template <typename OpT, typename T2>
struct apply_reorder_present_2args_t<
                        OpT,T2,
                        hana::when<
                            std::is_base_of<flag_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const OpT& op, const T2& b
                    ) const
    {
        format_join_grammar_cats(dst,
            fmt_pair(arp_first_formatter(formatters),op.str(value,b,arp_first_formatter(formatters)))
        );
    }
};

//-------------------------------------------------------------

/**
 * @brief Apply presentation and order of validation report for 3 arguments with property but without member.
 */
template <typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_3args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const PropT& prop, const OpT& op, const T2& b
                     ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_operands_formatter(formatters),b)
                );
            },
            [&](auto)
            {
                // prop op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),prop),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_operands_formatter(formatters),b)
                );
            }
        );
    }
};

/**
 * @brief Apply presentation and order of validation report for 3 arguments when operand's description must not be used in report.
 */
template <typename PropT, typename OpT, typename T2>
struct apply_reorder_present_3args_t<
                        PropT,OpT,T2,
                        hana::when<std::is_base_of<op_report_without_operand_t,std::decay_t<OpT>>::value>
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_strings_formatter(formatters),op.str(b))
                );
            },
            [&](auto)
            {
                // prop op.str(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),prop),
                    fmt_pair(arp_strings_formatter(formatters),op.str(b))
                );
            }
        );
    }
};

/**
 * @brief Apply presentation and order of validation report for 3 arguments when operator is a flag.
 */
template <typename PropT, typename OpT, typename T2>
struct apply_reorder_present_3args_t<
                        PropT,OpT,T2,
                        hana::when<std::is_base_of<flag_t,std::decay_t<OpT>>::value>
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::decay_t<OpT>::prepend_property(prop),
            [&dst,&formatters,&op,&prop,&b](auto&&)
            {
                // prop op.str(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),prop),
                    fmt_pair(arp_strings_formatter(formatters),op.str(prop,b,arp_mn_formatter(formatters)))
                );
            },
            [&dst,&formatters,&op,&prop,&b](auto&&)
            {
                // prop op.str(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_strings_formatter(formatters),op.str(prop,b,arp_mn_formatter(formatters)))
                );
            }
        );
    }
};

/**
 * @brief Apply presentation and order of validation report for 3 arguments when operand is a member operand.
 */
template <typename PropT, typename OpT, typename T2>
struct apply_reorder_present_3args_t<
                        PropT,OpT,T2,
                        hana::when<hana::is_a<member_operand_tag,T2>>
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_mn_formatter(formatters),b.get())
                );
            },
            [&](auto)
            {
                // prop op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),prop),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_mn_formatter(formatters),b.get())
                );
            }
        );
    }
};

//-------------------------------------------------------------

/**
 * @brief Apply presentation and order of validation report for 4 arguments with member.
 */
template <typename MemberT, typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_4args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),member),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_operands_formatter(formatters),b)
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(member,prop)),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_operands_formatter(formatters),b)
                );
            }
        );
    }
};

/**
 * @brief Apply presentation and order of validation report for 4 arguments with a member is compared to other member.
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                MemberT,PropT,OpT,T2,hana::when<hana::is_a<member_operand_tag,T2>>
            >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                    DstT& dst, FormatterTs&& formatters,
                    const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op member_operand(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),member),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_mn_formatter(formatters),b.get())
                );
            },
            [&](auto)
            {
                // prop of member op prop of member_operand(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(member,prop)),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(b.get(),prop))
                );
            }
        );
    }
};

/**
 * @brief Apply presentation and order of validation report when operand's description must not be used in report.
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                        MemberT,PropT,OpT,T2,
                        hana::when<
                            std::is_base_of<op_report_without_operand_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op.str(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),member),
                    fmt_pair(arp_strings_formatter(formatters),op.str(b))
                );
            },
            [&](auto)
            {
                // prop of member op.str(b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(member,prop)),
                    fmt_pair(arp_strings_formatter(formatters),op.str(b))
                );
            }
        );
    }
};

/**
 * @brief Apply presentation and order of validation report for flag operator.
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                        MemberT,PropT,OpT,T2,
                        hana::when<
                            std::is_base_of<flag_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            !std::decay_t<OpT>::prepend_property(prop),
            [&dst,&member,&formatters,&op,&prop,&b](auto&&)
            {
                // member flag.str(prop,b)
                hana::eval_if(
                    !std::is_base_of<variadic_property_tag,std::decay_t<PropT>>::value,
                    [&](auto &&)
                    {
                        format_join_grammar_cats(dst,
                            fmt_pair(arp_mn_formatter(formatters),member),
                            fmt_pair(arp_strings_formatter(formatters),op.str(prop,b,arp_mn_formatter(formatters)))
                        );
                    },
                    [&](auto &&)
                    {
                        // prop of member flag.str(prop,b)
                        format_join_grammar_cats(dst,
                            fmt_pair(arp_mn_formatter(formatters),make_member_property(member,prop)),
                            fmt_pair(arp_strings_formatter(formatters),op.str(prop,b,arp_mn_formatter(formatters),true))
                        );
                    }
                );
            },
            [&dst,&member,&formatters,&op,&prop,&b](auto&&)
            {
                // prop of member flag.str(prop,b)
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(member,prop)),
                    fmt_pair(arp_strings_formatter(formatters),op.str(prop,b,arp_mn_formatter(formatters)))
                );
            }
        );
    }
};

//-------------------------------------------------------------

/**
 * @brief Apply presentation and order of validation report for 4 arguments with a member is compared to the same member of sample object.
 */
template <typename MemberT, typename PropT, typename OpT, typename MemberSampleT, typename T2>
struct apply_reorder_present_5args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const MemberSampleT& member_sample, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op member of sample
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),member),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_mn_formatter(formatters),member_sample),
                    fmt_pair(arp_strings_formatter(formatters),string_conjunction_of),
                    fmt_pair(arp_operands_formatter(formatters),b)
                );
            },
            [&](auto)
            {
                // prop of member op prop of member of sample
                format_join_grammar_cats(dst,
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(member,prop)),
                    fmt_pair(arp_strings_formatter(formatters),op),
                    fmt_pair(arp_mn_formatter(formatters),make_member_property(member_sample,prop)),
                    fmt_pair(arp_strings_formatter(formatters),string_conjunction_of),
                    fmt_pair(arp_operands_formatter(formatters),b)
                );
            }
        );
    }
};

//-------------------------------------------------------------

/**
 * @brief Apply presentation and order of validation report for arbitrary number of arguments.
 */
template <typename ...Args>
struct apply_reorder_present_t
{
};

/**
 * @brief Apply presentation and order of validation report for 5 arguments with member and sample member.
 */
template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct apply_reorder_present_t<T1,T2,T3,T4,T5> : public apply_reorder_present_5args_t<T1,T2,T3,T4,T5>
{
};

/**
 * @brief Apply presentation and order of validation report for 4 arguments with member.
 */
template <typename T1, typename T2, typename T3, typename T4>
struct apply_reorder_present_t<T1,T2,T3,T4> : public apply_reorder_present_4args_t<T1,T2,T3,T4>
{
};

/**
 * @brief Apply presentation and order of validation report for 3 arguments with property but without member.
 */
template <typename T1, typename T2, typename T3>
struct apply_reorder_present_t<T1,T2,T3> : public apply_reorder_present_3args_t<T1,T2,T3>
{
};

/**
 * @brief Apply presentation and order of validation report for 2 arguments without property and without member.
 */
template <typename T1, typename T2>
struct apply_reorder_present_t<T1,T2> : public apply_reorder_present_2args_t<T1,T2>
{
};

/**
 * @brief Apply presentation and order of validation report for 1 argument.
 */
template <typename T1>
struct apply_reorder_present_t<T1> : public apply_reorder_present_1arg_t<T1>
{
};

/**
 * @brief Apply presentation and order.
 */
template <typename ... Args>
constexpr apply_reorder_present_t<Args...> apply_reorder_present{};

//-------------------------------------------------------------

/**
 * @brief Helper for applying presentation and order of validation report.
 */
struct reorder_and_present_t
{
    template <typename DstT, typename FormatterTs, typename ...Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        apply_reorder_present<Args...>(dst,std::forward<FormatterTs>(formatters),std::forward<Args>(args)...);
    }
};

/**
 * @brief Apply presentation and order of validation report.
 */
constexpr reorder_and_present_t reorder_and_present{};

//-------------------------------------------------------------

}

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
