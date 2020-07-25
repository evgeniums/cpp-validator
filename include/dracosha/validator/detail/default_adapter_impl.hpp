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
#include <dracosha/validator/utils/is_container.hpp>
#include <dracosha/validator/utils/wrap_it.hpp>
#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Helper for ANY/ALL aggregation of non-container types
 */
template <typename T, typename=hana::when<true>>
struct aggregate_impl
{
    template <typename ContainerT, typename AdapterT, typename MemberT, typename OpT>
    static status all(ContainerT&&, AdapterT&& adpt, MemberT&& member, OpT&& op)
    {
        return apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
    }

    template <typename ContainerT, typename AdapterT, typename MemberT, typename OpT>
    static status any(ContainerT&&, AdapterT&& adpt, MemberT&& member, OpT&& op)
    {
        return apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
    }

    template <typename ContainerT, typename AdapterT, typename OpT>
    static status all(ContainerT&&, AdapterT&& adpt, OpT&& op)
    {
        return apply(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op));
    }

    template <typename ContainerT, typename AdapterT, typename OpT>
    static status any(ContainerT&&, AdapterT&& adpt, OpT&& op)
    {
        return apply(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op));
    }
};

/**
 * @brief Helper for ANY/ALL aggregation of container types
 */
template <typename T>
struct aggregate_impl<T,
        hana::when<is_container_t<T>::value>>
{
    template <typename ContainerT, typename AdapterT, typename MemberT, typename OpT>
    static status all(ContainerT&& container, AdapterT&& adpt, MemberT&& member, OpT&& op);

    template <typename ContainerT, typename AdapterT, typename OpT>
    static status all(ContainerT&& container, AdapterT&& adpt, OpT&& op);

    template <typename ContainerT, typename AdapterT, typename MemberT, typename OpT>
    static status any(ContainerT&& container, AdapterT&& adpt, MemberT&& member, OpT&& op);

    template <typename ContainerT, typename AdapterT, typename OpT>
    static status any(ContainerT&& container, AdapterT&& adpt, OpT&& op);
};

//-------------------------------------------------------------
/**
 * @brief Implementation of default adapter
 */
struct default_adapter_impl
{
    /**
     *  Call validation operator on object extracted from adapter
     */
    template <typename AdapterT, typename T2, typename OpT>
    static status validate_operator(AdapterT&& adpt, OpT&& op, T2&& b)
    {
        const auto& obj=extract(adpt.traits().get());
        return op(
                    obj,
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  Call validation operator on property of the object extracted from adapter
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT>
    static status validate_property(AdapterT&& adpt, PropT&& prop, OpT&& op, T2&& b)
    {
        const auto& obj=extract(adpt.traits().get());
        return op(
                    property(obj,std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     * Check if member exists.
     */
    template <typename AdapterT, typename T2, typename OpT, typename MemberT>
    static status validate_exists(AdapterT&& adpt, MemberT&& member, OpT&&, T2&& b, bool from_check_member)
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

    /**
     * Validate a member.
     * If configured, the existance of the member can be checked before validation.
     */
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

    /**
     * Validate a member using other member as sample.
     * If configured, the existance of the member can be checked before validation.
     */
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

    /**
     * Validate a member using member with the same path of sample object.
     * If configured, the existance of the member can be checked before validation.
     */
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
                            property(get_member(extract(b)(),path),prop)
                        ));
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    /**
     * Validate AND aggregation.
     */
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

    /**
     * Validate a member using AND aggregation.
     */
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

    /**
     * Validate OR aggregation.
     */
    template <typename AdapterT, typename OpsT>
    static status validate_or(AdapterT&& adpt, OpsT&& ops)
    {
        auto ok=hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&adpt](status prevResult, auto&& op)
                    {
                        if (prevResult.value()==status::code::success)
                        {
                            return prevResult;
                        }
                        return status(apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
                    }
                );
        return ok;
    }

    /**
     * Validate a member using OR aggregation.
     */
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
                                if (prevResult.value()==status::code::success)
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

    /**
     * Validate ANY aggregation.
     */
    template <typename AdapterT, typename OpT>
    static status validate_any(AdapterT&& adpt, OpT&& op)
    {
        const auto& obj=extract(adpt.traits().get());
        return aggregate_impl<decltype(obj)>::any(obj,std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op));
    }

    /**
     * Validate a member using ANY aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpT>
    static status validate_any(AdapterT&& adpt, MemberT&& member, OpT&& op)
    {
        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&obj,&adpt,&member,&op](auto&&)
            {
                const auto& container=get_member(obj,member.path);
                return aggregate_impl<decltype(container)>::any(container,std::forward<decltype(adpt)>(adpt),member,std::forward<decltype(op)>(op));
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    /**
     * Validate a member ALL aggregation.
     */
    template <typename AdapterT, typename OpT>
    static status validate_all(AdapterT&& adpt, OpT&& op)
    {
        const auto& obj=extract(adpt.traits().get());
        return aggregate_impl<decltype(obj)>::all(obj,std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op));
    }

    /**
     * Validate a member using ALL aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpT>
    static status validate_all(AdapterT&& adpt, MemberT&& member, OpT&& op)
    {
        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path),
            [&obj,&adpt,&member,&op](auto&&)
            {
                const auto& container=get_member(obj,member.path);
                return aggregate_impl<decltype(container)>::all(container,std::forward<decltype(adpt)>(adpt),member,std::forward<decltype(op)>(op));
            },
            [](auto&&)
            {
                return status();
            }
        )(member.path);
    }

    /**
     * Validate NOT aggregation.
     */
    template <typename AdapterT, typename OpT>
    static status validate_not(AdapterT&& adpt, OpT&& op)
    {
        return status(!apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
    }

    /**
     * Validate a member NOT aggregation.
     */
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
                    return status(adpt.validate_exists(member,exists,true));
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
