/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/default_adapter_impl.hpp
*
*  Defines implementation of default adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_HPP
#define DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

struct default_adapter_impl
{
    template <typename AdapterT, typename T2, typename OpT>
    static status validate_operator(AdapterT&& adpt, OpT&& op, T2&& b)
    {
        const auto& obj=extract(adpt.traits().get());
        return op(
                    obj,
                    extract(std::forward<T2>(b))
                );
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT>
    static status validate_property(AdapterT&& adpt, PropT&& prop, OpT&& op, T2&& b)
    {
        const auto& obj=extract(adpt.traits().get());
        return op(
                    property(obj,std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    template <typename AdapterT, typename T2, typename MemberT>
    static status validate_exists(AdapterT&& adpt, MemberT&& member, T2&& b, bool from_check_member)
    {
        std::ignore=from_check_member;
        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&obj,&b](auto&& path)
            {
                return exists(obj,std::forward<decltype(path)>(path))==b;
            },
            [&b](auto&&)
            {
                return b==false;
            }
        )(member.path);
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&obj,&prop,&op,&b](auto&& path)
            {
                return op(
                            property(get_member(obj,std::forward<decltype(path)>(path)),std::forward<PropT>(prop)),
                            extract(std::forward<T2>(b))
                        );
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate_with_other_member(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!adpt.traits().check_member_exists(member) || !adpt.traits().check_member_exists(b))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&obj,&prop,&op](auto&& path, auto&& b_path)
            {
                return status(op(
                            property(get_member(obj,path),prop),
                            property(get_member(obj,b_path),prop)
                        ));
            },
            [](auto&&, auto&&)
            {
                return status();
            }
        )(member.path,b.path);
    }

    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate_with_master_sample(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&obj,&prop,&op,&b](auto&& path)
            {
                return status(op(
                            property(get_member(obj,path),prop),
                            property(get_member(b(),path),prop)
                        ));
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    template <typename AdapterT, typename OpsT>
    static status validate_and(AdapterT&& adpt, OpsT&& ops)
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&adpt](status prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return prevResult;
                        }
                        return status(apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
                    }
                );
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_and(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&adpt,&member,&ops](auto&&)
            {
                return hana::fold(std::forward<decltype(ops)>(ops),true,
                            [&member,&adpt](status prevResult, auto&& op)
                            {
                                if (!prevResult)
                                {
                                    return prevResult;
                                }
                                return status(apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
                            }
                        );
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    template <typename AdapterT, typename OpsT>
    static status validate_or(AdapterT&& adpt, OpsT&& ops)
    {
        auto ok=hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&adpt](status prevResult, auto&& op)
                    {
                        if (prevResult.value()==status::code::ok)
                        {
                            return prevResult;
                        }
                        return status(apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
                    }
                );
        return ok;
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_or(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&adpt,&member,&ops](auto&&)
            {
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
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    template <typename AdapterT, typename OpT>
    static status validate_not(AdapterT&& adpt, OpT&& op)
    {
        return status(!apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
    }

    template <typename AdapterT, typename MemberT, typename OpT>
    static status validate_not(AdapterT&& adpt, MemberT&& member, OpT&& op)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&adpt,&member,&op](auto&&)
            {
                return status(!apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
            },
            [&adpt,&member](auto&&)
            {
                if (adpt.traits().unknown_member_mode()==if_member_not_found::abort)
                {
                    return status(adpt.validate_exists(member,true));
                }
                return status();
            }
        )(member.path);
    }
};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_HPP
