/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/impl/default_adapter_impl.hpp
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
#include <dracosha/validator/utils/conditional_fold.hpp>

#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
/**
 * @brief Implementation of default adapter.
 */
struct default_adapter_impl
{
    /**
     *  @brief Call validation operator on object extracted from adapter.
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
     *  @brief Call validation operator on property of the object extracted from adapter.
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
     * @brief Check if member exists.
     */
    template <typename AdapterT, typename T2, typename OpT, typename MemberT>
    static status validate_exists(AdapterT&& adpt, MemberT&& member, OpT&&, T2&& b, bool from_check_member=false)
    {
        std::ignore=from_check_member;
        return adpt.check_path_exists(adpt,member.path(),b);
    }

    /**
     * @brief Validate a member.
     *
     * If configured, the existence of the member can be checked before validation.
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path()),
            [&obj,&prop,&op,&b](auto&& path)
            {
                auto&& val=get_member(obj,std::forward<decltype(path)>(path));
                auto&& pr=property(val,std::forward<PropT>(prop));
                return op(
                            pr,
                            extract(std::forward<T2>(b))
                        );
            },
            [](auto&&)
            {
                return status(status::code::ignore);
            }
        )(member.path());
    }

    /**
     * @brief Validate a member using other member as sample.
     *
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
        return hana::if_(check_member_path(obj,member.path()),
            [&obj,&prop,&op](auto&& path, auto&& b_path)
            {
                return status(op(
                            property(get_member(obj,path),prop),
                            property(get_member(obj,b_path),prop)
                        ));
            },
            [](auto&&, auto&&)
            {
                return status(status::code::ignore);
            }
        )(member.path(),b.path());
    }

    /**
     * @brief Validate a member using member with the same path of sample object.
     *
     * If configured, the existance of the member can be checked before validation.
     * If sample does not contain member then check is ignored.
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate_with_master_sample(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        const auto& sample=extract(b)();

        auto&& obj_might_have_path=check_member_path(sample,member.path());
        auto&& sample_might_have_path=check_member_path(sample,member.path());

        auto sample_has_path=hana::if_(sample_might_have_path,
            [&sample](auto&& path)
            {
                return exists(sample,path);
            },
            [](auto&&)
            {
                return false;
            }
        )(member.path());
        if (!sample_has_path)
        {
            // if sample does not have member then ignore check
            return status(status::code::ignore);
        }

        return hana::if_(
            hana::and_(
                obj_might_have_path,
                sample_might_have_path
            ),
            [&obj,&prop,&op,&sample](auto&& path)
            {
                return status(op(
                            property(get_member(obj,path),prop),
                            property(get_member(sample,path),prop)
                        ));
            },
            [](auto&&)
            {
                return status(status::code::ignore);
            }
        )(member.path());
    }

    template <typename PredicateT, typename AdapterT, typename OpsT>
    static status validate_aggregation(const PredicateT& pred, AdapterT&& adpt, OpsT&& ops)
    {
        return while_each(
                              ops,
                              pred,
                              status(status::code::ignore),
                              [&adpt](auto&& op)
                              {
                                return status(apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
                              }
                          );
    }

    template <typename PredicateT, typename AdapterT, typename OpsT, typename MemberT>
    static status validate_member_aggregation(const PredicateT& pred, AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        return while_each(
                              ops,
                              pred,
                              status(status::code::ignore),
                              [&member,&adpt](auto&& op)
                              {
                                return status(apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
                              }
                          );
    }

    /**
     * @brief Validate AND aggregation.
     */
    template <typename AdapterT, typename OpsT>
    static status validate_and(AdapterT&& adpt, OpsT&& ops)
    {
        return validate_aggregation(predicate_and,std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_member_and(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        return validate_member_aggregation(predicate_and,std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpsT>(ops));
    }

    /**
     * @brief Validate a member using AND aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_and(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path()),
            [&adpt,&member,&ops](auto&&)
            {
                return validate_member_and(
                            std::forward<decltype(adpt)>(adpt),
                            std::forward<decltype(member)>(member),
                            std::forward<decltype(ops)>(ops)
                          );
            },
            [](auto&&)
            {
                return status(status::code::ignore);
            }
        )(member.path());
    }

    /**
     * @brief Validate OR aggregation.
     */
    template <typename AdapterT, typename OpsT>
    static status validate_or(AdapterT&& adpt, OpsT&& ops)
    {
        return validate_aggregation(status_predicate_or,std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_member_or(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        return validate_member_aggregation(status_predicate_or,std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpsT>(ops));
    }

    /**
     * @brief Validate a member using OR aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_or(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
    {
        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path()),
            [&adpt,&member,&ops](auto&&)
            {
                return validate_member_or(
                            std::forward<decltype(adpt)>(adpt),
                            std::forward<decltype(member)>(member),
                            std::forward<decltype(ops)>(ops)
                          );
            },
            [](auto&&)
            {
                return status(status::code::ignore);
            }
        )(member.path());
    }

    /**
     * @brief Validate NOT aggregation.
     */
    template <typename AdapterT, typename OpT>
    static status validate_not(AdapterT&& adpt, OpT&& op)
    {
        return status(!apply(std::forward<AdapterT>(adpt),std::forward<decltype(op)>(op)));
    }

    /**
     * @brief Validate a member NOT aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpT>
    static status validate_not(AdapterT&& adpt, MemberT&& member, OpT&& op)
    {
        if (!adpt.traits().check_member_exists(member))
        {
            return adpt.traits().not_found_status();
        }

        const auto& obj=extract(adpt.traits().get());
        return hana::if_(check_member_path(obj,member.path()),
            [&adpt,&member,&op](auto&&)
            {
                return status(!apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
            },
            [&adpt,&member](auto&&)
            {
                if (adpt.traits().unknown_member_mode()==if_member_not_found::abort)
                {
                    return status(adpt.traits().validate_exists(adpt,member,exists,true));
                }
                return status(status::code::ignore);
            }
        )(member.path());
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_HPP
