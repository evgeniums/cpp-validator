/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/reorder_and_present.hpp
*
*  Defines helpers for adjusting strings order and presentation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
#define DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/detail/if_bool.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#include <dracosha/validator/detail/formatter_fmt.hpp>
#else
//! \todo use std string formatting
#endif

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

#ifdef DRACOSHA_VALIDATOR_FMT
constexpr detail::fmt_formatter_t backend_formatter{};
constexpr detail::fmt_join_append_t formatter_join_append{};
#else
//! \todo use std string formatting
#endif

//-------------------------------------------------------------

namespace detail
{

//-------------------------------------------------------------

/**
 * @brief Apply adjusting of order and presentation
 */
template <typename DstT, typename FormatterTs, typename ...Args>
constexpr auto apply_reorder_present_fn(DstT& dst, FormatterTs&& formatters, Args&&... args) -> decltype(auto)
{
    // the remarkable composition below is about
    // to apply each formatter to its corresponding argument
    // and then forward the results to formatter_join_append handler

    return hana::unpack(hana::transform( // transform all {formatter,arg} pairs to results of formatter(arg)
                                            hana::zip( // pair each formatter with its argument
                                                std::forward<FormatterTs>(formatters),
                                                make_cref_tuple(std::forward<Args>(args)...)
                                            ),
                                            hana::fuse(apply_ref) // for each pair invoke a formatter with the argument from the same pair
                                        ),
                        hana::partial(     // send all formatter(pair) results to join_append handler
                                formatter_join_append,
                                ref(dst)
                            )
                       );
}

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 1 argument, which must be an aggregation operator
 */
template <typename OpT, typename = hana::when<true>>
struct apply_reorder_present_1arg_t
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                    DstT& dst,
                                    FormatterTs&& formatters,
                                    const OpT& op
                                ) const -> decltype(auto)
    {
        // op:
        return backend_formatter.append(
            dst,
            apply_ref(hana::at(formatters,hana::size_c<0>),op),
            apply_ref(hana::at(formatters,hana::size_c<0>),string_conjunction_aggregate)
        );
    }
};
template <typename OpT>
constexpr apply_reorder_present_1arg_t<OpT> apply_reorder_present_1arg{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 2 arguments without property and without member
 */
template <typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_2args_t
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                    DstT& dst,
                                    FormatterTs&& formatters,
                                    const OpT& op,
                                    const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_present_fn(
                                dst,
                                std::forward<FormatterTs>(formatters),
                                if_bool<T2,OpT>(std::forward<OpT>(op)),b
                                );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 2 arguments with logical operator and a member
 */
template <typename MemberT,typename OpT>
struct apply_reorder_present_2args_t<MemberT,OpT,
                        hana::when<hana::is_a<aggregation_op_tag,OpT>>
                    >
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                    DstT& dst,
                                    FormatterTs&& formatters,
                                    const MemberT& member,
                                    const OpT& op
                                ) const -> decltype(auto)
    {
        // for member op:
        backend_formatter.join_append(
            dst,
            apply_ref(hana::at(formatters,hana::size_c<1>),string_conjunction_for),
            apply_ref(hana::at(formatters,hana::size_c<0>),member),
            apply_ref(hana::at(formatters,hana::size_c<1>),op)
        );
        return backend_formatter.append(
            dst,
            apply_ref(hana::at(formatters,hana::size_c<1>),string_conjunction_aggregate)
        );
    }
};

template <typename OpT, typename T2>
constexpr apply_reorder_present_2args_t<OpT,T2> apply_reorder_present_2args{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 3 arguments with property but without member
 */
template <typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_3args_t
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                DstT& dst, FormatterTs&& formatters,
                                const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_present_fn(dst,
                                std::forward<FormatterTs>(formatters),
                                prop,if_bool<T2,OpT>(std::forward<OpT>(op)),b
                                );
    }
};

/**
 * @brief Adjust presentation and order of validation report for property "empty"
 */
template <typename PropT, typename OpT, typename T2>
struct apply_reorder_present_3args_t<
                        PropT,OpT,T2,
                        hana::when<
                            std::is_same<std::decay_t<PropT>,type_p_empty>::value
                            &&
                            std::is_same<std::decay_t<T2>,bool>::value
                            &&
                            (std::is_same<std::decay_t<OpT>,eq_t>::value || std::is_same<std::decay_t<OpT>,ne_t>::value)
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                DstT& dst, FormatterTs&& formatters,
                                const PropT&, const OpT&, const T2& b
                                ) const -> decltype(auto)
    {
        auto format_empty=[&](bool eq)
        {
            if (eq==b)
            {
                // is empty
                backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),string_empty)
                );
            }
            else
            {
                // is not empty
                backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),string_not_empty)
                );
            }
        };

        hana::eval_if(
            std::is_same<std::decay_t<OpT>,eq_t>::value,
            [&](auto){format_empty(true);},
            [&](auto){format_empty(false);}
        );
    }
};
template <typename PropT, typename OpT, typename T2>
constexpr apply_reorder_present_3args_t<PropT,OpT,T2> apply_reorder_present_3args{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with member
 */
template <typename MemberT, typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_4args_t
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                DstT& dst, FormatterTs&& formatters,
                                const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op b
                return backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),if_bool<T2,OpT>(std::forward<OpT>(op))),
                    apply_ref(hana::at(formatters,hana::size_c<3>),b)
                );
            },
            [&](auto)
            {
                // prop of member op b
                return backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),prop),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),if_bool<T2,OpT>(std::forward<OpT>(op))),
                    apply_ref(hana::at(formatters,hana::size_c<3>),b)
                );
            }
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with a member is compared to other member
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                MemberT,PropT,OpT,T2,hana::when<hana::is_a<member_name_tag,T2>>
            >
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                DstT& dst, FormatterTs&& formatters,
                                const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op b
                return backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),if_bool<T2,OpT>(std::forward<OpT>(op))),
                    apply_ref(hana::at(formatters,hana::size_c<3>),b.get())
                );
            },
            [&](auto)
            {
                // prop of member op b
                return backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),prop),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),if_bool<T2,OpT>(std::forward<OpT>(op))),
                    apply_ref(hana::at(formatters,hana::size_c<1>),prop),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<3>),b.get())
                );
            }
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with a member is compared to the same member of sample object
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                MemberT,PropT,OpT,T2,hana::when<std::is_same<std::decay_t<T2>,string_master_sample_t>::value>
            >
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                DstT& dst, FormatterTs&& formatters,
                                const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op member of sample
                return backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),if_bool<T2,OpT>(std::forward<OpT>(op))),
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<3>),b)
                );
            },
            [&](auto)
            {
                // prop of member op prop of member of sample
                return backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),prop),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),if_bool<T2,OpT>(std::forward<OpT>(op))),
                    apply_ref(hana::at(formatters,hana::size_c<1>),prop),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<0>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_conjunction_of),
                    apply_ref(hana::at(formatters,hana::size_c<3>),b)
                );
            }
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for property "empty" of a member
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                        MemberT,PropT,OpT,T2,
                        hana::when<
                            std::is_same<std::decay_t<PropT>,type_p_empty>::value
                            &&
                            std::is_same<std::decay_t<T2>,bool>::value
                            &&
                            (std::is_same<std::decay_t<OpT>,eq_t>::value || std::is_same<std::decay_t<OpT>,ne_t>::value)
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (
                                DstT& dst, FormatterTs&& formatters,
                                const MemberT& member, const PropT&, const OpT&, const T2& b
                                ) const -> decltype(auto)
    {
        auto format_empty=[&](bool eq)
        {
            if (eq==b)
            {
                // member is empty
                backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_empty)
                );
            }
            else
            {
                // member is not empty
                backend_formatter.join_append(
                    dst,
                    apply_ref(hana::at(formatters,hana::size_c<1>),member),
                    apply_ref(hana::at(formatters,hana::size_c<2>),string_not_empty)
                );
            }
        };

        hana::eval_if(
            std::is_same<std::decay_t<OpT>,eq_t>::value,
            [&](auto){format_empty(true);},
            [&](auto){format_empty(false);}
        );
    }
};
template <typename MemberT, typename PropT, typename OpT, typename T2>
constexpr apply_reorder_present_4args_t<MemberT,PropT,OpT,T2> apply_reorder_present_4args{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for arbitrary number of arguments
 */
template <typename ...Args>
struct apply_reorder_present_t
{
    template <typename DstT, typename FormatterTs>
    constexpr auto operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_fn(
                                dst,
                                std::forward<FormatterTs>(formatters),
                                std::forward<Args>(args)...
                                );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with member
 */
template <typename T1, typename T2, typename T3, typename T4>
struct apply_reorder_present_t<T1,T2,T3,T4>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_4args<T1,T2,T3,T4>(
                                             dst,
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 3 arguments with property but without member
 */
template <typename T1, typename T2, typename T3>
struct apply_reorder_present_t<T1,T2,T3>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_3args<T1,T2,T3>(dst,
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 2 arguments without property and without member
 */
template <typename T1, typename T2>
struct apply_reorder_present_t<T1,T2>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_2args<T1,T2>(
                                             dst,
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 1 argument
 */
template <typename T1>
struct apply_reorder_present_t<T1>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_1arg<T1>(
                                             dst,
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

template <typename ... Args>
constexpr apply_reorder_present_t<Args...> apply_reorder_present{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report
 */
struct reorder_and_present_t
{
    template <typename DstT, typename FormatterTs, typename ...Args>
    constexpr auto operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present<Args...>(dst,std::forward<FormatterTs>(formatters),std::forward<Args>(args)...);
    }
};
constexpr reorder_and_present_t reorder_and_present{};

//-------------------------------------------------------------

}

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
