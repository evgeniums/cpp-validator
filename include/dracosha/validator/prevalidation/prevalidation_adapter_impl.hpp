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

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementation of prevalidation adapter.
 *
 * Adapter first checks path of the member to validate and performs validation only
 * if the path matches the path of filtering member.
 *
 * @note Currently nested ALL/ANY aggregation operators are not supported.
 */
template <typename CheckMemberT>
class prevalidation_adapter_impl : public strict_any_tag
{
    public:

        prevalidation_adapter_impl(CheckMemberT&& member)
            : _member(std::forward<CheckMemberT>(member)),
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
        status validate_property(AdapterT&& adpt, PropT&& prop, OpT&& op, T2&& b) const
        {
            return default_adapter_impl::validate_property(std::forward<AdapterT>(adpt),
                                                             std::forward<PropT>(prop),
                                                             std::forward<OpT>(op),
                                                             std::forward<T2>(b));
        }

        template <typename AdapterT, typename T2, typename OpT, typename MemberT>
        status validate_exists(AdapterT&& adpt, MemberT&& member, OpT&&, T2&& b, bool from_check_member) const
        {
            std::ignore=from_check_member;

            // check [exists] suffix only
            return hana::if_(
                std::is_same<std::decay_t<decltype(exists)>,std::decay_t<decltype(_member.key())>>{},
                [](auto&& self, auto&& adpt, auto&& member, auto&& b)
                {
                    if (self->filter_member(member[exists]))
                    {
                        return status(status::code::ignore);
                    }
                    const auto& value=extract(adpt.traits().get());
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
                    const auto& value=extract(_(adpt).traits().get());
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
            const auto& obj=extract(adpt.traits().get());

            return hana::if_(
                hana::and_(
                    hana::is_a<value_as_container_tag,decltype(obj)>,
                    hana::not_(std::is_same<std::decay_t<OpT>,std::decay_t<decltype(contains)>>{})
                ),
                [](auto&& ...)
                {
                    // only "contains" operator is suitable for check_contains_stub
                    return status(status::code::ignore);
                },
                [&obj](auto&& self, auto&& adpt, auto&& member, auto&& prop, auto&& op, auto&& b)
                {
                    // select execution path depending on the type of adapter's member key, if size/empty then check size
                    return hana::if_(
                        hana::or_(
                            std::is_same<std::decay_t<decltype(size)>,std::decay_t<decltype(self->_member.key())>>{},
                            std::is_same<std::decay_t<decltype(empty)>,std::decay_t<decltype(self->_member.key())>>{}
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
                                            self->_member.key()
                                        );
                        },
                        [&obj](auto&& self, auto&& adpt, auto&& member, auto&& prop, auto&& op, auto&& b)
                        {
                            if (self->_member_checked)
                            {
                                return self->validate_property(
                                            std::forward<decltype(adpt)>(adpt),
                                            std::forward<decltype(prop)>(prop),
                                            std::forward<decltype(op)>(op),
                                            std::forward<decltype(b)>(b)
                                        );
                            }

                            // check member path as is
                            return hana::eval_if(
                                        (
                                            (check_member_path_types(self->_member,member))
                                            &&
                                            has_property_fn(obj,prop)
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
                                                std::forward<decltype(b)>(b)
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
            if (filter_member_with_size(member))
            {
                return status(status::code::ignore);
            }
            const auto& obj=extract(adpt.traits().get());
            return status(op(
                        property(obj,std::forward<PropT>(prop)),
                        property(get_member(extract(b)(),member.path()),prop)
                    ));
        }

        template <typename AdapterT, typename OpsT>
        status validate_and(AdapterT&& adpt, OpsT&& ops) const
        {
            return default_adapter_impl::validate_and(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename MemberT, typename OpsT>
        status validate_and(AdapterT&& adpt, MemberT&& member, OpsT&& ops) const
        {
            return default_adapter_impl::validate_member_and(std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename OpsT>
        status validate_or(AdapterT&& adpt, OpsT&& ops) const
        {
            return default_adapter_impl::validate_or(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename MemberT, typename OpsT>
        status validate_or(AdapterT&& adpt, MemberT&& member, OpsT&& ops) const
        {
            return default_adapter_impl::validate_member_or(std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        template <typename AdapterT, typename OpT>
        status validate_not(AdapterT&& adpt, OpT&& op) const
        {
            return default_adapter_impl::validate_not(std::forward<AdapterT>(adpt),std::forward<OpT>(op));
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_not(AdapterT&& adpt, MemberT&& member, OpT&& op) const
        {
            if (filter_member_with_size(member))
            {
                return status(status::code::ignore);
            }
            return status(!apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member)));
        }

        template <typename AdapterT, typename OpT>
        status validate_any(AdapterT&& adpt, OpT&& op) const
        {
            if (!is_strict_any())
            {
                return status(status::code::ignore);
            }

            return default_adapter_impl::validate_any(std::forward<AdapterT>(adpt),std::forward<OpT>(op));
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_any_impl(AdapterT&& adpt, MemberT&& member, OpT&& op) const
        {
            auto self=this;
            return hana::eval_if(
                hana::or_(
                            check_member_path_types(_member,member),
                            check_member_path_types(_member,member[size]),
                            check_member_path_types(_member,member[empty])
                         ),
                [&self,&member,&adpt,&op](auto&&)
                {
                    if (self->filter_member_with_size(member))
                    {
                        return status(status::code::ignore);
                    }
                    const auto& obj=extract(adpt.traits().get());
                    return aggregate_impl<decltype(obj)>::any(obj,std::forward<AdapterT>(adpt),member,std::forward<OpT>(op));
                },
                [](auto&&)
                {
                    return status(status::code::ignore);
                }
            );
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_any(AdapterT&& adpt, MemberT&& member, OpT&& op) const
        {
            if (!is_strict_any())
            {
                return status(status::code::ignore);
            }

            return hana::if_(
                hana::greater_equal(hana::size_c<hana_tuple_size<decltype(_member.path())>::value>,hana::size_c<2>),
                [](auto&& self, AdapterT&& adpt, MemberT&& member, OpT&& op)
                {
                    if (hana::equal(size,self->_member.key()) || hana::equal(empty,self->_member.key()))
                    {
                        auto&& adjusted_member_path=hana::append(member.path(),hana::back(self->_member.parent_path()));
                        auto adjusted_member=make_member(std::move(adjusted_member_path));
                        return self->validate_any_impl(std::forward<AdapterT>(adpt),adjusted_member,std::forward<OpT>(op));
                    }
                    auto&& adjusted_member_path=hana::append(member.path(),hana::back(self->_member.path()));
                    auto adjusted_member=make_member(std::move(adjusted_member_path));
                    return self->validate_any_impl(std::forward<AdapterT>(adpt),adjusted_member,std::forward<OpT>(op));
                },
                [](auto&& self, AdapterT&& adpt, MemberT&& member, OpT&& op)
                {
                    return self->validate_any_impl(std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpT>(op));
                }
            )(this,std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpT>(op));
        }

        template <typename AdapterT, typename OpT>
        status validate_all(AdapterT&& adpt, OpT&& op) const
        {
            return default_adapter_impl::validate_all(std::forward<AdapterT>(adpt),std::forward<OpT>(op));
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_all_impl(AdapterT&& adpt, MemberT&& member, OpT&& op) const
        {
            auto self=this;
            return hana::eval_if(
                hana::or_(
                            check_member_path_types(_member,member),
                            check_member_path_types(_member,member[size]),
                            check_member_path_types(_member,member[empty])
                         ),
                [&self,&member,&adpt,&op](auto&&)
                {
                    if (self->filter_member_with_size(member))
                    {
                        return status(status::code::ignore);
                    }
                    const auto& obj=extract(adpt.traits().get());
                    return aggregate_impl<decltype(obj)>::all(obj,std::forward<AdapterT>(adpt),member,std::forward<OpT>(op));
                },
                [](auto&&)
                {
                    return status(status::code::ignore);
                }
            );
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_all(AdapterT&& adpt, MemberT&& member, OpT&& op) const
        {
            return hana::if_(
                hana::greater_equal(hana::size_c<hana_tuple_size<decltype(_member.path())>::value>,hana::size_c<2>),
                [](auto&& self, AdapterT&& adpt, MemberT&& member, OpT&& op)
                {
                    if (hana::equal(size,self->_member.key()) || hana::equal(empty,self->_member.key()))
                    {
                        auto&& adjusted_member_path=hana::append(member.path(),hana::back(self->_member.parent_path()));
                        auto adjusted_member=make_member(std::move(adjusted_member_path));
                        return self->validate_all_impl(std::forward<AdapterT>(adpt),adjusted_member,std::forward<OpT>(op));
                    }
                    auto&& adjusted_member_path=hana::append(member.path(),hana::back(self->_member.path()));
                    auto adjusted_member=make_member(std::move(adjusted_member_path));
                    return self->validate_all_impl(std::forward<AdapterT>(adpt),adjusted_member,std::forward<OpT>(op));
                },
                [](auto&& self, AdapterT&& adpt, MemberT&& member, OpT&& op)
                {
                    return self->validate_all_impl(std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpT>(op));
                }
            )(this,std::forward<AdapterT>(adpt),std::forward<MemberT>(member),std::forward<OpT>(op));
        }

        const auto& member() const
        {
            return _member;
        }

        void set_member_checked(bool enable) noexcept
        {
            _member_checked=enable;
        }
        bool is_member_checked() const noexcept
        {
            return _member_checked;
        }

    private:

        template <typename MemberT>
        bool filter_member(const MemberT& member) const noexcept
        {
            return !_member.isEqual(member);
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

        CheckMemberT _member;
        mutable bool _member_checked;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_IMPL_HPP
