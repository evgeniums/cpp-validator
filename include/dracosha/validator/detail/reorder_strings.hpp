/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/reorder_strings.hpp
*
*  Defines helpers for strings reordering.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REORDER_STRINGS_HPP
#define DRACOSHA_VALIDATOR_REORDER_STRINGS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/cref.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/properties/empty.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename HandlerT, typename FormatterTs, typename ...Args>
constexpr auto apply_reorder_fn(HandlerT&& fn, FormatterTs&& formatters, Args&&... args) -> decltype(auto)
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

template <typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_3args_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const PropT& prop, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_fn(std::forward<HandlerT>(fn),
                                std::forward<FormatterTs>(formatters),
                                prop,op,b
                                );
    }
};

template <typename HandlerT,typename FormattersT>
constexpr void format_empty(HandlerT&& fn, FormattersT&& formatters, bool eq, bool b)
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
}

template <typename PropT, typename OpT, typename T2>
struct apply_reorder_3args_t<
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
        hana::eval_if(
            std::is_same<std::decay_t<OpT>,eq_t>::value,
            [&](auto)
            {
                format_empty(std::forward<HandlerT>(fn),std::forward<FormatterTs>(formatters),true,b);
            },
            [&](auto)
            {
                format_empty(std::forward<HandlerT>(fn),std::forward<FormatterTs>(formatters),false,b);
            }
        );
    }
};
template <typename PropT, typename OpT, typename T2>
constexpr apply_reorder_3args_t<PropT,OpT,T2> apply_reorder_3args{};

template <typename PropT, typename MemberT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_4args_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (
                                HandlerT&& fn, FormatterTs&& formatters,
                                const PropT& prop, const MemberT& member, const OpT& op, const T2& b
                                ) const -> decltype(auto)
    {
        return apply_reorder_fn(std::forward<HandlerT>(fn),
                                std::forward<FormatterTs>(formatters),
                                prop,member,op,b
                                );
    }
};
template <typename PropT, typename MemberT, typename OpT, typename T2>
constexpr apply_reorder_4args_t<PropT,MemberT,OpT,T2> apply_reorder_4args{};

template <typename ...Args>
struct apply_reorder_t
{
    template <typename HandlerT, typename FormatterTs>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_fn(std::forward<HandlerT>(fn),
                                std::forward<FormatterTs>(formatters),
                                std::forward<Args>(args)...
                                );
    }
};

template <typename T1, typename T2, typename T3, typename T4>
struct apply_reorder_t<T1,T2,T3,T4>
{
    template <typename HandlerT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_4args<T1,T2,T3,T4>(
                                             std::forward<HandlerT>(fn),
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

template <typename T1, typename T2, typename T3>
struct apply_reorder_t<T1,T2,T3>
{
    template <typename HandlerT, typename FormatterTs, typename ... Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder_3args<T1,T2,T3>(std::forward<HandlerT>(fn),
                                             std::forward<FormatterTs>(formatters),
                                             std::forward<Args>(args)...
                                             );
    }
};

template <typename ... Args>
constexpr apply_reorder_t<Args...> apply_reorder{};

struct reorder_strings_t
{
    template <typename HandlerT, typename FormatterTs, typename ...Args>
    constexpr auto operator () (HandlerT&& fn, FormatterTs&& formatters, Args&&... args) const -> decltype(auto)
    {
        return apply_reorder<Args...>(std::forward<HandlerT>(fn),std::forward<FormatterTs>(formatters),std::forward<Args>(args)...);
    }
};
constexpr reorder_strings_t reorder_strings{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REORDER_STRINGS_HPP
