/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/single_member_adapter_impl.hpp
*
*  Defines implementation of adapter for single member validation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_IMPL_HPP
#define DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/detail/default_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename CheckMemberT>
struct single_member_adapter_impl
{
    CheckMemberT _member;

    single_member_adapter_impl(CheckMemberT&& member):_member(std::forward<CheckMemberT>(member))
    {}

    template <typename AdapterT, typename T2, typename OpT>
    status validate_operator(AdapterT&& adpt, OpT&& op, T2&& b) const
    {
        return default_adapter_impl::validate_operator(std::forward<AdapterT>(adpt),
                                                         std::forward<OpT>(op),
                                                         std::forward<T2>(b));
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT>
    status validate_property(AdapterT&& adpt, PropT&& prop, OpT&& op, T2&& b) const
    {
        return default_adapter_impl::validate_property(std::forward<AdapterT>(adpt),
                                                         std::forward<PropT>(prop),
                                                         std::forward<OpT>(op),
                                                         std::forward<T2>(b));
    }

    template <typename AdapterT, typename T2, typename MemberT>
    status validate_exists(AdapterT&& adpt, MemberT&& member, T2&& b, bool from_check_member) const
    {
        std::ignore=adpt;
        std::ignore=from_check_member;
        if (!_member.isEqual(member))
        {
            return status::code::ignore;
        }
        return b;
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    status validate(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        const auto& obj=extract(adpt.traits().get());
        return hana::eval_if(
                    (
                        check_member_path_types(_member,member)
                        &&
                        has_property_fn(obj,prop)
                     ),
            [this,&member,&adpt,&prop,&op,&b](auto&&)
            {
                if (!_member.isEqual(member))
                {
                    return status(status::code::ignore);
                }
                return validate_property(std::forward<AdapterT>(adpt),std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<T2>(b));
            },
            [](auto&&)
            {
                return status(status::code::ignore);
            }
        );
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    status validate_with_other_member(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        std::ignore=adpt;
        std::ignore=member;
        std::ignore=prop;
        std::ignore=op;
        std::ignore=b;
        return status::code::ignore;
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    status validate_with_master_sample(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        if (!_member.isEqual(member))
        {
            return status::code::ignore;
        }
        return default_adapter_impl::validate_with_master_sample(std::forward<AdapterT>(adpt),std::forward<MemberT>(member),
                                                         std::forward<PropT>(prop),
                                                         std::forward<OpT>(op),
                                                         std::forward<T2>(b));
    }

    template <typename AdapterT, typename OpsT>
    status validate_and(AdapterT&& adpt, OpsT&& ops) const
    {
        return default_adapter_impl::validate_and(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    status validate_and(AdapterT&& adpt, MemberT&& member, OpsT&& ops) const
    {
        if (!_member.isEqual(member))
        {
            return status::code::ignore;
        }
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&member,&adpt](status prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return prevResult;
                        }
                        return status(
                            apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member))
                        );
                    }
                );
    }

    template <typename AdapterT, typename OpsT>
    status validate_or(AdapterT&& adpt, OpsT&& ops) const
    {
        return default_adapter_impl::validate_or(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    status validate_or(AdapterT&& adpt, MemberT&& member, OpsT&& ops) const
    {
        if (!_member.isEqual(member))
        {
            return status::code::ignore;
        }
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&adpt,&member](status prevResult, auto&& op)
                    {
                        if (prevResult.value()==status::code::ok)
                        {
                            return prevResult;
                        }
                        return status(apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
                    }
                );
    }

    template <typename AdapterT, typename OpT>
    status validate_not(AdapterT&& adpt, OpT&& op) const
    {
        return default_adapter_impl::validate_not(std::forward<AdapterT>(adpt),std::forward<OpT>(op));
    }

    template <typename AdapterT, typename MemberT, typename OpT>
    status validate_not(AdapterT&& adpt, MemberT&& member, OpT&& op) const
    {
        if (!_member.isEqual(member))
        {
            return status::code::ignore;
        }
        return status(!apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
    }
};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_IMPL_HPP
