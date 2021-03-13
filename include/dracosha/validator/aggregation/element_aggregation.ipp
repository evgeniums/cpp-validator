/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/aggregation/element_aggregation.ipp
*
*  Defines helpers used for element aggregations.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_IPP
#define DRACOSHA_VALIDATOR_ELEMENT_AGGREGATION_IPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/aggregation/element_aggregation.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/detail/aggregate_all.hpp>
#include <dracosha/validator/detail/aggregate_any.hpp>
#include <dracosha/validator/aggregation/wrap_it.hpp>
#include <dracosha/validator/aggregation/wrap_index.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename PredicateT, typename EmptyFnT, typename AggregationT,
          typename PathT, typename AdapterT, typename HandlerT>
status element_aggregation::invoke(PredicateT&& pred, EmptyFnT&& empt, AggregationT&& aggr,
                     PathT&& path, AdapterT&& adapter, HandlerT&& handler)
{
    const auto& obj=extract(adapter.traits().get());
    const auto parent_path=hana::drop_back(path);

    // invoke only if parent path is ok
    if (!adapter.traits().check_path_exists(parent_path))
    {
        return adapter.traits().not_found_status();
    }

    return hana::eval_if(is_member_path_valid(obj,parent_path),
        [&](auto&& _v)
        {
            const auto& parent_element=get_member(_v(obj),_v(parent_path));
            using parent_type=std::decay_t<decltype(parent_element)>;
            return hana::eval_if(
                is_container_t<parent_type>{},
                [&](auto&& _)
                {
                    // agrregation can be invoked only on container types
                    const auto& el_aggregation=hana::back(path);

                    aggregate_report<AdapterT>::open(adapter,aggr,parent_path);
                    bool empty=true;
                    for (auto it=_(parent_element).begin();it!=_(parent_element).end();++it)
                    {
                        status ret=_(handler)(hana::append(parent_path,wrap_it(it,aggr,el_aggregation.modifier)));
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
                    // skip aggregation if not a container type
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

template <typename PredicateT, typename EmptyFnT, typename AggregationT,
          typename PathT, typename AdapterT, typename HandlerT>
status element_aggregation::invoke_variadic(PredicateT&& pred, EmptyFnT&& empt, AggregationT&& aggr,
                     PathT&& path, AdapterT&& adapter, HandlerT&& handler)
{
    auto compacted_path=compact_variadic_property(path);
    auto parent_compacted_path=hana::drop_back(compacted_path);

    if (!adapter.traits().check_path_exists(parent_compacted_path))
    {
        return adapter.traits().not_found_status();
    }

    const auto& obj=extract(adapter.traits().get());
    return hana::eval_if(is_member_path_valid(obj,parent_compacted_path),
        [&](auto&& _)
        {
            auto upper_path=hana::drop_back(path);
            const auto& parent=get_member(_(obj),_(parent_compacted_path));
            const auto& aggregation_varg=unwrap_object(hana::back(_(path)));

            aggregate_report<AdapterT>::open(adapter,aggr,_(parent_compacted_path));
            bool empty=true;
            for (auto it=aggregation_varg.begin(parent);
                 aggregation_varg.while_cond(parent,it);
                 aggregation_varg.next(parent,it)
                )
            {
                status ret=_(handler)(hana::append(upper_path,varg(wrap_index(it,aggr))));
                if (!pred(ret))
                {
                    aggregate_report<AdapterT>::close(adapter,ret);
                    return ret;
                }
                empty=false;
            }
            auto result=empt(empty);
            aggregate_report<AdapterT>::close(adapter,result);
            return result;
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
