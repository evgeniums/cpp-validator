/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/filer_member.hpp
*
*  Defines "filter_member".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP
#define DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/filter_path.hpp>
#include <dracosha/validator/embedded_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename KeyT, typename Enable=hana::when<true>>
struct generate_paths_t
{
    template <typename PathT, typename AdapterT, typename HandlerT>
    status operator () (PathT&& path, AdapterT&&, HandlerT&& handler) const;
};
template <typename KeyT>
constexpr generate_paths_t<KeyT> generate_paths{};

//-------------------------------------------------------------

struct apply_generated_paths_t
{
    template <typename PathT, typename AdapterT, typename MemberT, typename HandlerT>
    status operator () (PathT&& current_path, AdapterT&&, MemberT&& member, HandlerT&& handler) const;

    template <typename PathT, typename AdapterT, typename HandlerT>
    static status gen(PathT&& path, AdapterT&&, HandlerT&& handler);
};
constexpr apply_generated_paths_t apply_generated_paths{};

//-------------------------------------------------------------

struct apply_member_path_t
{
    template <typename PathT, typename FnT, typename AdapterT, typename MemberT>
    status operator () (PathT&& current_path, FnT&& fn, AdapterT&& adapter, MemberT&& member) const;
};
constexpr apply_member_path_t apply_member_path{};

//-------------------------------------------------------------

struct invoke_member_if_exists_impl
{
    template <typename FnT, typename AdapterT, typename MemberT>
    status operator () (FnT&& fn, AdapterT&& adapter, MemberT&& member) const
    {
        auto invoke=[](auto&& fn, auto&& adapter, auto&& member)
        {
            return fn(std::forward<decltype(adapter)>(adapter),std::forward<decltype(member)>(member));
        };

        using type=typename std::decay_t<AdapterT>::type;
        return hana::eval_if(
            hana::or_(
                typename type::filter_if_not_exists{}
            ),
            [&](auto&& _)
            {
                const auto& original_obj=original_embedded_object(_(adapter));
                return hana::eval_if(
                    is_member_path_valid(original_obj,_(member).path()),
                    [&](auto&& _)
                    {
                        if (!embedded_object_has_member(_(adapter),_(member)))
                        {
                            return _(adapter).traits().not_found_status();
                        }
                        return status(_(invoke(_(fn),_(adapter),_(member))));
                    },
                    [&](auto&&)
                    {
                        return status(status::code::ignore);
                    }
                );
            },
            [&](auto&& _)
            {
                return status(_(invoke(_(fn),_(adapter),_(member))));
            }
        );
    }
};
constexpr invoke_member_if_exists_impl invoke_member_if_exists{};

//-------------------------------------------------------------

template <typename AdapterT, typename MemberT, typename Enable=hana::when<true>>
struct filter_member_invoker
{
    template <typename FnT, typename AdapterT1, typename MemberT1>
    static status invoke(FnT&& fn, AdapterT1&& adapter, MemberT1&& member)
    {
        if (filter_path(adapter,member.path()))
        {
            return status::code::ignore;
        }        
        return invoke_member_if_exists(fn,adapter,member);
    }
};

template <typename AdapterT, typename MemberT>
struct filter_member_invoker<AdapterT,MemberT,
            hana::when<
                MemberT::is_aggregated::value
                &&
                AdapterT::expand_aggregation_members::value
            >
        >
{
    template <typename FnT, typename AdapterT1, typename MemberT1>
    static auto invoke(FnT&& fn, AdapterT1&& adapter, MemberT1&& member)
    {
        auto handler=[&fn](auto&& adapter, auto&& member)
        {
            return invoke_member_if_exists(fn,adapter,member);
        };
        return apply_member_path(hana::tuple<>{},handler,adapter,member);
    }
};

//-------------------------------------------------------------

struct filter_member_t
{
    template <typename FnT, typename AdapterT, typename MemberT>
    auto operator () (FnT&& fn, AdapterT&& adapter, MemberT&& member) const
    {
        return filter_member_invoker<std::decay_t<AdapterT>,std::decay_t<MemberT>>
                    ::invoke(std::forward<FnT>(fn),std::forward<AdapterT>(adapter),std::forward<MemberT>(member));
    }
};
constexpr filter_member_t filter_member{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP
