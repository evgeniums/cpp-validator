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
#include <dracosha/validator/aggregation/wrap_it.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/embedded_object.hpp>

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
    static status validate_operator(AdapterT&& adapter, OpT&& op, T2&& b)
    {
        return op(
                    embedded_object(adapter),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Call validation operator on property of the object extracted from adapter.
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT>
    static status validate_property(AdapterT&& adapter, PropT&& prop, OpT&& op, T2&& b)
    {
        const auto& obj=embedded_object(adapter);
        return op(
                    property(obj,std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     * @brief Check if member exists.
     */
    template <typename AdapterT, typename T2, typename OpT, typename MemberT>
    static status validate_exists(AdapterT&& adapter, MemberT&& member, OpT&&, T2&& b, bool from_check_member=false)
    {
        std::ignore=from_check_member;
        return adapter.check_path_exists(adapter,member.path(),b);
    }

    /**
     * @brief Validate a member.
     *
     * If configured, the existence of the member can be checked before validation.
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate(AdapterT&& adapter, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!embedded_object_has_member(adapter,member))
        {
            return adapter.traits().not_found_status();
        }

        const auto& original_obj=original_embedded_object(std::forward<AdapterT>(adapter));
        return hana::if_(
            is_member_path_valid(original_obj,member.path()),
            [&prop,&op,&b](const auto& adapter, const auto& member)
            {
                return op(
                            property(embedded_object_member(adapter,member),std::forward<PropT>(prop)),
                            extract(std::forward<T2>(b))
                        );
            },
            [](const auto&,const auto&)
            {
                return status(status::code::ignore);
            }
        )(adapter,member);
    }

    /**
     * @brief Validate a member using other member as sample.
     *
     * If configured, the existance of the member can be checked before validation.
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate_with_other_member(AdapterT&& adapter, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!embedded_object_has_member(adapter,member) || !original_embedded_object_has_member(adapter,b))
        {
            return adapter.traits().not_found_status();
        }

        const auto& original_obj=original_embedded_object(adapter);
        return hana::if_(
            hana::and_(
                is_member_path_valid(original_obj,member.path()),
                is_member_path_valid(original_obj,b.path())
            ),
            [&prop,&op](const auto& adapter, const auto& member, const auto& b)
            {
                return status(
                        op(
                            property(embedded_object_member(adapter,member),prop),
                            property(original_embedded_object_member(adapter,b),prop)
                        )
                    );
            },
            [](const auto&, const auto&, const auto&)
            {
                return status(status::code::ignore);
            }
        )(adapter,member,b);
    }

    /**
     * @brief Validate a member using member with the same path of sample object.
     *
     * If configured, the existance of the member can be checked before validation.
     * If sample does not contain member then check is ignored.
     */
    template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
    static status validate_with_master_sample(AdapterT&& adapter, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
    {
        if (!embedded_object_has_member(adapter,member))
        {
            return adapter.traits().not_found_status();
        }

        const auto& sample=extract(b)();
        auto sample_might_have_path=is_member_path_valid(sample,member.path());
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

        const auto& original_obj=original_embedded_object(adapter);
        auto obj_might_have_path=is_member_path_valid(original_obj,member.path());

        return hana::if_(
            hana::and_(
                obj_might_have_path,
                sample_might_have_path
            ),
            [&prop,&op,&sample](const auto& adapter, const auto& member)
            {
                return status(
                        op(
                            property(embedded_object_member(adapter,member),prop),
                            property(get_member(sample,member.path()),prop)
                        )
                    );
            },
            [](const auto&,const auto&)
            {
                return status(status::code::ignore);
            }
        )(adapter,member);
    }

    template <typename PredicateT, typename AdapterT, typename OpsT>
    static status validate_aggregation(const PredicateT& pred, AdapterT&& adapter, OpsT&& ops)
    {
        return while_each(
                              ops,
                              pred,
                              status(status::code::ignore),
                              [&adapter](auto&& op)
                              {
                                return status(apply(std::forward<AdapterT>(adapter),std::forward<decltype(op)>(op)));
                              }
                          );
    }

    template <typename PredicateT, typename AdapterT, typename OpsT, typename MemberT>
    static status validate_member_aggregation(const PredicateT& pred, AdapterT&& adapter, MemberT&& member, OpsT&& ops)
    {
        return while_each(
                              ops,
                              pred,
                              status(status::code::ignore),
                              [&member,&adapter](auto&& op)
                              {
                                return status(apply_member(std::forward<decltype(adapter)>(adapter),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
                              }
                          );
    }

    /**
     * @brief Validate AND aggregation.
     */
    template <typename AdapterT, typename OpsT>
    static status validate_and(AdapterT&& adapter, OpsT&& ops)
    {
        return validate_aggregation(predicate_and,std::forward<AdapterT>(adapter),std::forward<OpsT>(ops));
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_member_and(AdapterT&& adapter, MemberT&& member, OpsT&& ops)
    {
        return validate_member_aggregation(predicate_and,std::forward<AdapterT>(adapter),std::forward<MemberT>(member),std::forward<OpsT>(ops));
    }

    /**
     * @brief Validate a member using AND aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_and(AdapterT&& adapter, MemberT&& member, OpsT&& ops)
    {
        if (!embedded_object_has_member(adapter,member))
        {
            return adapter.traits().not_found_status();
        }

        const auto& original_obj=original_embedded_object(adapter);
        return hana::if_(
            is_member_path_valid(original_obj,member.path()),
            [&adapter,&member,&ops](auto&&)
            {
                return validate_member_and(
                            std::forward<decltype(adapter)>(adapter),
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
    static status validate_or(AdapterT&& adapter, OpsT&& ops)
    {
        return validate_aggregation(status_predicate_or,std::forward<AdapterT>(adapter),std::forward<OpsT>(ops));
    }

    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_member_or(AdapterT&& adapter, MemberT&& member, OpsT&& ops)
    {
        return validate_member_aggregation(status_predicate_or,std::forward<AdapterT>(adapter),std::forward<MemberT>(member),std::forward<OpsT>(ops));
    }

    /**
     * @brief Validate a member using OR aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpsT>
    static status validate_or(AdapterT&& adapter, MemberT&& member, OpsT&& ops)
    {
        const auto& original_obj=original_embedded_object(adapter);
        return hana::if_(
            is_member_path_valid(original_obj,member.path()),
            [&adapter,&member,&ops](auto&&)
            {
                return validate_member_or(
                            std::forward<decltype(adapter)>(adapter),
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
    static status validate_not(AdapterT&& adapter, OpT&& op)
    {
        return status(!apply(std::forward<AdapterT>(adapter),std::forward<decltype(op)>(op)));
    }

    /**
     * @brief Validate a member NOT aggregation.
     */
    template <typename AdapterT, typename MemberT, typename OpT>
    static status validate_not(AdapterT&& adapter, MemberT&& member, OpT&& op)
    {
        if (!embedded_object_has_member(adapter,member))
        {
            return adapter.traits().not_found_status();
        }

        const auto& original_obj=original_embedded_object(adapter);
        return hana::if_(
            is_member_path_valid(original_obj,member.path()),
            [&adapter,&member,&op](auto&&)
            {
                return status(!apply_member(std::forward<decltype(adapter)>(adapter),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
            },
            [&adapter,&member](auto&&)
            {
                if (adapter.traits().unknown_member_mode()==if_member_not_found::abort)
                {
                    return status(adapter.traits().validate_exists(adapter,member,exists,true));
                }
                return status(status::code::ignore);
            }
        )(member.path());
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_HPP
