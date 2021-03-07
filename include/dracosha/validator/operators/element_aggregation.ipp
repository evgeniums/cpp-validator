/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/element_aggregation.ipp
*
*  Defines helpers used for element aggregations.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_IPP
#define DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_IPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/element_aggregation.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/detail/aggregate_all.hpp>
#include <dracosha/validator/detail/aggregate_any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename PredicateT, typename EmptyFnT, typename StringT, typename PathT, typename AdapterT, typename HandlerT>
status element_aggregation::invoke(PredicateT&& pred, EmptyFnT&& empt, StringT&& str, PathT&& path, AdapterT&& adapter, HandlerT&& handler)
{
    const auto& obj=extract(adapter.traits().get());
    const auto parent_path=hana::drop_back(path);

    if (!adapter.traits().check_path_exists(parent_path))
    {
        return adapter.traits().not_found_status();
    }

    return hana::eval_if(check_member_path(obj,parent_path),
        [&](auto&& _v)
        {
            const auto& parent_element=get_member(_v(obj),_v(parent_path));
            return hana::eval_if(
                is_container_t<std::decay_t<decltype(parent_element)>>{},
                [&](auto&& _)
                {
                    const auto& el_aggregation=hana::back(path);

                    aggregate_report<AdapterT>::open(adapter,str,parent_path);
                    bool empty=true;
                    for (auto it=_(parent_element).begin();it!=_(parent_element).end();++it)
                    {
                        status ret=_(handler)(hana::append(parent_path,wrap_it(it,str,el_aggregation.modifier)));
                        if (!pred(ret))
                        {
                            aggregate_report<AdapterT>::close(adapter,ret);
                            return ret;
                        }
                        empty=false;
                    }
                    auto ret=empt(empty);
                    aggregate_report<AdapterT>::close(adapter,ret);
                    return ret;
                },
                [&](auto&& _)
                {
                    return _(handler)(std::forward<decltype(path)>(path));
                }
            );
        },
        [](auto&&)
        {
            return status{status::code::ignore};
        }
    );
}

template <typename ModifierT>
template <typename OpT>
constexpr auto all_t<ModifierT>::operator() (OpT&& op) const
{
    return hana::if_(
        hana::is_a<element_aggregation_modifier_tag,OpT>,
        [](auto&& op)
        {
            return all_t<std::decay_t<decltype(op)>>{};
        },
        [](auto&& op)
        {
            return make_validator(
                        hana::reverse_partial(
                            detail::aggregate_all,
                            std::forward<decltype(op)>(op)
                        )
                   );
        }
    )(std::forward<OpT>(op));
}

template <typename ModifierT>
template <typename OpT>
constexpr auto any_t<ModifierT>::operator() (OpT&& op) const
{
    return hana::if_(
        hana::is_a<element_aggregation_modifier_tag,OpT>,
        [](auto&& op)
        {
            return any_t<std::decay_t<decltype(op)>>{};
        },
        [](auto&& op)
        {
            return make_validator(
                        hana::reverse_partial(
                            detail::aggregate_any,
                            std::forward<decltype(op)>(op)
                        )
                   );
        }
    )(std::forward<OpT>(op));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_IPP
