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
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/generate_paths.hpp>
#include <dracosha/validator/apply_generated_paths.hpp>
#include <dracosha/validator/apply_member_path.hpp>
#include <dracosha/validator/invoke_member_if_exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

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
        return apply_member_path(hana::size_c<0>,hana::tuple<>{},handler,adapter,member);
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
