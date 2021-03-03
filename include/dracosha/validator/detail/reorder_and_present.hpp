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

#include <iostream>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/operators/op_report_without_operand.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_operand.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/report_aggregation.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename DstT, typename FormatterTs, typename ... Args>
void format_join(DstT& dst, FormatterTs&& formatters, Args&&... args)
{
    auto pairs=hana::zip(
        std::forward<FormatterTs>(formatters),
        make_cref_tuple(std::forward<Args>(args)...)
    );
    auto parts=hana::fold(
        pairs,
        hana::make_tuple(),
        [](auto&& prev, auto&& current)
        {
            return hana::append(std::forward<decltype(prev)>(prev),apply_ref(hana::front(current),hana::back(current),last_grammar_categories(prev,0)));
        }
    );
    backend_formatter.append_join(
       dst,
       " ",
       parts
    );
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
        format_join(dst,
            std::forward<FormatterTs>(formatters),
            op,
            b
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
        format_join(dst,
            hana::make_tuple(
                hana::at(formatters,hana::size_c<0>)
            ),
            op.str(b)
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
        format_join(dst,
            hana::make_tuple(
                hana::at(formatters,hana::size_c<0>)
            ),
            op.str(value,b)
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<1>),
                        hana::at(formatters,hana::size_c<2>)
                    ),
                    op,
                    b
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join(dst,
                    std::forward<FormatterTs>(formatters),
                    prop,
                    op,
                    b
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<1>)
                    ),
                    op.str(b)
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<1>)
                    ),
                    prop,
                    op.str(b)
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    prop,
                    op.str(prop,b)
                );
            },
            [&dst,&formatters,&op,&prop,&b](auto&&)
            {
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    op.str(prop,b)
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<1>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    op,
                    b.get()
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<1>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    prop,
                    op,
                    b.get()
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    member,
                    op,
                    b
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    make_member_property(member,prop),
                    op,
                    b
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    member,
                    op,
                    b.get()
                );
            },
            [&](auto)
            {
                // prop of member op prop of member_operand(b)
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    make_member_property(member,prop),
                    op,
                    make_member_property(b.get(),prop)
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
                // member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>)
                    ),
                    member,
                    op.str(b)
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>)
                    ),
                    make_member_property(member,prop),
                    op.str(b)
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<1>)
                    ),
                    member,
                    op.str(prop,b)
                );
            },
            [&dst,&member,&formatters,&op,&prop,&b](auto&&)
            {
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<1>)
                    ),
                    make_member_property(member,prop),
                    op.str(prop,b)
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
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    member,
                    op,
                    member_sample,
                    string_conjunction_of,
                    b
                );
            },
            [&](auto)
            {
                // prop of member op prop of member of sample
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    make_member_property(member,prop),
                    op,
                    make_member_property(member_sample,prop),
                    string_conjunction_of,
                    b
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
    template <typename DstT, typename FormatterTs>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        format_join(dst,
            std::forward<FormatterTs>(formatters),
            std::forward<Args>(args)...
        );
    }
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
