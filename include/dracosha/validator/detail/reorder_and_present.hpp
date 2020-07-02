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
#include <dracosha/validator/cref.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/detail/if_bool.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Apply adjusting of order and presentation
 */
template <typename HandlerT, typename FormatterTs, typename ...Args>
constexpr auto apply_reorder_present_fn(HandlerT&& fn, FormatterTs&& formatters, Args&&... args) -> decltype(auto)
{
    // the remarkable composition below is about
    // to apply each formatter to its corresponding argument
    // and then forward the results back to handler

    return hana::unpack(hana::transform( // transform all {formatter,arg} pairs to results of formatter(arg)
                                            hana::zip( // pair each formatter with its argument
                                                std::forward<FormatterTs>(formatters),
                                                make_cref_tuple(std::forward<Args>(args)...)
                                            ),
                                            hana::fuse(apply_cref) // for each pair invoke a formatter with the argument from the same pair
                                        ),
                                        std::forward<HandlerT>(fn) // send all formatter(pair) results to handler
                            );
}

/**
 * @brief Adjust presentation and order of validation report for 3 arguments with property but without member
 */
template <typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_3args_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_present_fn(std::forward<HandlerT>(fn),
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
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const PropT&, const OpT&, const T2& b
                                ) const -> decltype(auto)
    {
        auto format_empty=[&](bool eq)
        {
            if (eq==b)
            {
                fn(
                    apply_cref(hana::at(formatters,hana::size_c<1>),string_empty)
                );
            }
            else
            {
                fn(
                    apply_cref(hana::at(formatters,hana::size_c<1>),string_not_empty)
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


/**
 * @brief Adjust presentation and order of validation report for 4 arguments with member
 */
template <typename MemberT, typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_4args_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_present_fn(std::forward<HandlerT>(fn),
                                std::forward<FormatterTs>(formatters),
                                member,prop,if_bool<T2,OpT>(std::forward<OpT>(op)),b
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
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const MemberT& member, const PropT&, const OpT&, const T2& b
                                ) const -> decltype(auto)
    {
        auto format_empty=[&](bool eq)
        {
            if (eq==b)
            {
                fn(
                    apply_cref(hana::at(formatters,hana::size_c<1>),member),
                    apply_cref(hana::at(formatters,hana::size_c<2>),string_empty)
                );
            }
            else
            {
                fn(
                    apply_cref(hana::at(formatters,hana::size_c<1>),member),
                    apply_cref(hana::at(formatters,hana::size_c<2>),string_not_empty)
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

/**
 * @brief Adjust presentation and order of validation report for 2 arguments without property and without member
 */
template <typename OpT, typename T2>
struct apply_reorder_present_2args_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_present_fn(std::forward<HandlerT>(fn),
                                std::forward<FormatterTs>(formatters),
                                if_bool<T2,OpT>(std::forward<OpT>(op)),b
                                );
    }
};
template <typename OpT, typename T2>
constexpr apply_reorder_present_2args_t<OpT,T2> apply_reorder_present_2args{};

/**
 * @brief Adjust presentation and order of validation report for arbitrary number of arguments
 */
template <typename ...Args>
struct apply_reorder_present_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_fn(std::forward<HandlerT>(fn),
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
    template <typename HandlerT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_4args<T1,T2,T3,T4>(
                                             std::forward<HandlerT>(fn),
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
    template <typename HandlerT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_3args<T1,T2,T3>(std::forward<HandlerT>(fn),
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
    template <typename HandlerT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present_2args<T1,T2>(std::forward<HandlerT>(fn),
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

template <typename ... Args>
constexpr apply_reorder_present_t<Args...> apply_reorder_present{};

/**
 * @brief Adjust presentation and order of validation report
 */
struct reorder_and_present_t
{
    template <typename HandlerT, typename FormatterTs, typename ...Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_present<Args...>(std::forward<HandlerT>(fn),std::forward<FormatterTs>(formatters),std::forward<Args>(args)...);
    }
};
constexpr reorder_and_present_t reorder_and_present{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
