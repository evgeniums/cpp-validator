/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/prevalidation_adapter_impl.hpp
*
*  Defines implementation of adapter for member prevalidation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_IMPL_HPP
#define DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/adapters/impl/default_adapter_impl.hpp>
#include <dracosha/validator/prevalidation/prevalidation_adapter_tag.hpp>
#include <dracosha/validator/utils/value_as_container.hpp>
#include <dracosha/validator/prevalidation/strict_any.hpp>
#include <dracosha/validator/range.hpp>
#include <dracosha/validator/filter_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementation of prevalidation adapter.
 *
 * Adapter first checks path of the member to validate and performs validation only
 * if the path matches the path of filtering member.
 */
template <typename CheckMemberT>
class prevalidation_adapter_impl : public strict_any_tag
{
    public:

        prevalidation_adapter_impl(CheckMemberT&& member)
            : _mmember(std::forward<CheckMemberT>(member)),
              _member_checked(false)
        {}

        template <typename AdapterT, typename T2, typename OpT>
        status validate_operator(AdapterT&& adpt, OpT&& op, T2&& b) const
        {
            return default_adapter_impl::validate_operator(std::forward<AdapterT>(adpt),
                                                             std::forward<OpT>(op),
                                                             std::forward<T2>(b));
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT>
        status validate_property(AdapterT&& adpt, PropT&& prop, OpT&& op, T2&& b, bool any=false) const
        {
            auto&& obj=unwrap_object(extract(traits_of(adpt).get()));
            auto&& val=extract(std::forward<T2>(b));
            return hana::eval_if(
                hana::is_a<range_tag,decltype(obj)>,
                [&](auto&& _)
                {
                    auto ok=!any;
                    for (auto&& it : _(obj).container)
                    {
                        ok=op(property(it,prop),val);
                        if (any)
                        {
                            if (ok)
                            {
                               break;
                            }
                        }
                        else
                        {
                            if (!ok)
                            {
                                break;
                            }
                        }
                    }
                    return ok;
                },
                [&](auto&&)
                {
                    return default_adapter_impl::validate_property(std::forward<AdapterT>(adpt),
                                                                     std::forward<PropT>(prop),
                                                                     std::forward<OpT>(op),
                                                                     val);
                }
            );
        }

        template <typename AdapterT, typename T2, typename OpT, typename MemberT>
        status validate_exists(AdapterT&& adpt, MemberT&& member, OpT&&, T2&& b, bool from_check_member=false, bool skip_check=false) const
        {
            std::ignore=from_check_member;
            std::ignore=skip_check;

            // check [exists] suffix only
            return hana::if_(
                std::is_same<std::decay_t<decltype(exists)>,std::decay_t<decltype(check_member().key())>>{},
                [](auto&& self, auto&& adpt, auto&& member, auto&& b)
                {
                    if (self->filter_member(member[exists]))
                    {
                        return status(status::code::ignore);
                    }
                    const auto& value=extract(traits_of(adpt).get());
                    return status(b==value);
                },
                [](auto&&...){return status::code::ignore;}
            )(
                this,
                std::forward<decltype(adpt)>(adpt),
                std::forward<decltype(member)>(member),
                std::forward<decltype(b)>(b)
            );
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT, typename SizePropT>
        status validate_size_property(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b, SizePropT&& size_p) const
        {
            auto self=this;
            return hana::eval_if(
                hana::or_(
                    std::is_same<std::decay_t<decltype(size)>,std::decay_t<PropT>>{},
                    std::is_same<std::decay_t<decltype(length)>,std::decay_t<PropT>>{},
                    std::is_same<std::decay_t<decltype(empty)>,std::decay_t<PropT>>{}
                ),
                [&](auto&& _)
                {
                    if (!((_(size_p)==prop) || (_(size_p)==size && _(prop)==length)))
                    {
                        return status(status::code::ignore);
                    }

                    if (_(size_p)==empty)
                    {
                        if (_(self)->filter_member(_(member)[empty]))
                        {
                            return status(status::code::ignore);
                        }
                    }
                    else
                    {
                        if (_(self)->filter_member(_(member)[size]))
                        {
                            return status(status::code::ignore);
                        }
                    }
                    const auto& value=extract(traits_of(_(adpt)).get());
                    return status(_(op)(value,_(b)));
                },
                [](auto&&)
                {
                    return status(status::code::ignore);
                });
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
        status validate(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            if (member.has_any() && !is_strict_any())
            {
                return status(status::code::ignore);
            }

            const auto& obj=extract(traits_of(adpt).get());
            return hana::if_(
                hana::and_(
                    hana::is_a<value_as_container_tag,decltype(obj)>,
                    hana::not_(std::is_same<std::decay_t<OpT>,std::decay_t<decltype(contains)>>{})
                ),
                [](auto&& ...)
                {
                    // only "contains" operator is suitable for value_as_container_tag
                    return status(status::code::ignore);
                },
                [](auto&& self, auto&& adpt, auto&& member, auto&& prop, auto&& op, auto&& b)
                {
                    // select execution path depending on the type of adapter's member key, if size/empty then check size
                    return hana::if_(
                        hana::or_(
                            std::is_same<std::decay_t<decltype(size)>,std::decay_t<decltype(self->check_member().key())>>{},
                            std::is_same<std::decay_t<decltype(empty)>,std::decay_t<decltype(self->check_member().key())>>{}
                        ),
                        [](auto&& self, auto&& adpt, auto&& member, auto&& prop, auto&& op, auto&& b)
                        {
                            // check size/empty member path suffix
                            return self->validate_size_property(
                                            std::forward<decltype(adpt)>(adpt),
                                            std::forward<decltype(member)>(member),
                                            std::forward<decltype(prop)>(prop),
                                            std::forward<decltype(op)>(op),
                                            std::forward<decltype(b)>(b),
                                            self->check_member().key()
                                        );
                        },
                        [](auto&& self, auto&& adpt, auto&& member, auto&& prop, auto&& op, auto&& b)
                        {
                            if (self->_member_checked)
                            {
                                return self->validate_property(
                                            std::forward<decltype(adpt)>(adpt),
                                            std::forward<decltype(prop)>(prop),
                                            std::forward<decltype(op)>(op),
                                            std::forward<decltype(b)>(b),
                                            member.key_is_any()
                                        );
                            }

                            // check member path as is
                            return hana::eval_if(
                                hana::and_(
                                    same_member_path_types(self->check_member(),member)
                                ),
                                [&self,&member,&adpt,&prop,&op,&b](auto&&)
                                {
                                    if (self->filter_member(member))
                                    {
                                        return status(status::code::ignore);
                                    }
                                    return self->validate_property(
                                                std::forward<decltype(adpt)>(adpt),
                                                std::forward<decltype(prop)>(prop),
                                                std::forward<decltype(op)>(op),
                                                std::forward<decltype(b)>(b),
                                                member.key_is_any()
                                            );
                                },
                                [](auto&&)
                                {
                                    return status(status::code::ignore);
                                }
                            );
                        }
                    )(
                        self,
                        std::forward<decltype(adpt)>(adpt),
                        std::forward<decltype(member)>(member),
                        std::forward<decltype(prop)>(prop),
                        std::forward<decltype(op)>(op),
                        std::forward<decltype(b)>(b)
                      );
                }
            )(
                this,
                std::forward<decltype(adpt)>(adpt),
                std::forward<decltype(member)>(member),
                std::forward<decltype(prop)>(prop),
                std::forward<decltype(op)>(op),
                std::forward<decltype(b)>(b)
            );
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_other_member(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            // not supported
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
            if (member.has_any() && !is_strict_any())
            {
                return status(status::code::ignore);
            }

            if (filter_member_with_size(member))
            {
                return status(status::code::ignore);
            }

            const auto& obj=extract(traits_of(adpt).get());
            const auto& sample=extract(b)();

            auto sample_might_have_path=is_member_path_valid(sample,check_member().path());
            auto sample_has_path=hana::if_(
                sample_might_have_path,
                [&sample](auto&& path)
                {
                    return exists(sample,path);
                },
                [](auto&&)
                {
                    return false;
                }
            )(check_member().path());
            if (!sample_has_path)
            {
                // if sample does not have member then ignore check
                return status(status::code::ignore);
            }

            return hana::if_(
                sample_might_have_path,
                [&obj,&prop,&op,&sample](auto&& path)
                {
                    return status(op(
                                property(obj,std::forward<PropT>(prop)),
                                property(get_member(sample,path),prop)
                            ));
                },
                [](auto&&)
                {
                    return status(status::code::ignore);
                }
            )(check_member().path());
        }

        template <typename AdapterT, typename OpsT>
        status validate_and(AdapterT&& adpt, OpsT&& ops) const
        {
            return default_adapter_impl::validate_and(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
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
                        return status(
                                    apply_member(
                                        adapter,
                                        std::forward<decltype(op)>(op),
                                        member
                                    )
                                 );
                      }
                  );
        }

        template <typename AdapterT, typename MemberT, typename OpsT>
        status validate_and(AdapterT&& adapter, MemberT&& member, OpsT&& ops) const
        {
            return default_adapter_impl::validate_and(std::forward<AdapterT>(adapter),std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename OpsT>
        status validate_or(AdapterT&& adpt, OpsT&& ops) const
        {
            return default_adapter_impl::validate_or(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename MemberT, typename OpsT>
        status validate_or(AdapterT&& adapter, MemberT&& member, OpsT&& ops) const
        {
            return default_adapter_impl::validate_or(std::forward<AdapterT>(adapter),std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename OpT>
        status validate_not(AdapterT&& adpt, OpT&& op) const
        {
            return default_adapter_impl::validate_not(std::forward<AdapterT>(adpt),std::forward<OpT>(op));
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_not(AdapterT&& adapter, MemberT&& member, OpT&& op) const
        {
            if (filter_member_with_size(member))
            {
                return status(status::code::ignore);
            }
            return status(!apply_member(adapter,std::forward<decltype(op)>(op),member));
        }

        void set_member_checked(bool enable) noexcept
        {
            _member_checked=enable;
        }
        bool is_member_checked() const noexcept
        {
            return _member_checked;
        }

        const auto& check_member() const
        {
            return _mmember.get();
        }

    private:

        template <typename MemberT>
        bool filter_member(const MemberT& member) const noexcept
        {
            return !check_member().equals(member);
        }

        template <typename MemberT>
        bool filter_member_with_size(const MemberT& member) const noexcept
        {
            return filter_member(member)
               &&
               filter_member(member[size])
               &&
               filter_member(member[empty]);
        }

        object_wrapper<CheckMemberT> _mmember;
        mutable bool _member_checked;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_IMPL_HPP
