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
#include <dracosha/validator/embedded_object.hpp>
#include <dracosha/validator/base_validator.hpp>
#include <dracosha/validator/adapters/make_intermediate_adapter.hpp>
#include <dracosha/validator/utils/heterogeneous_size.hpp>
#include <dracosha/validator/utils/foreach_if.hpp>
#include <dracosha/validator/aggregation/wrap_heterogeneous_index.hpp>
#include <dracosha/validator/compact_variadic_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename PredicateT, typename EmptyFnT, typename AggregationT,
          typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
status element_aggregation::invoke(PredicateT&& pred, EmptyFnT&& empt, AggregationT&& aggr,
                     UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler)
{
    // invoke only if parent path is ok
    const auto parent_path=hana::drop_back(path);
    if (!embedded_object_has_path(adapter,parent_path))
    {
        return traits_of(adapter).not_found_status();
    }

    return hana::eval_if(
        is_embedded_object_path_valid(adapter,parent_path),
        [&](auto&& _)
        {
            auto&& parent_element=embedded_object_member(_(adapter),_(parent_path));
            using parent_type=std::decay_t<decltype(parent_element)>;
            return hana::eval_if(
                is_container_t<parent_type>{},
                [&](auto&& _)
                {
                    // agrregation can be invoked on container types
                    const auto& el_aggregation=hana::back(path);

                    auto tmp_adapter=make_intermediate_adapter(_(adapter),_(parent_path));

                    aggregate_report<AdapterT>::open(_(adapter),_(aggr),_(parent_path));
                    bool empty=true;
                    for (auto it=_(parent_element).begin();it!=_(parent_element).end();++it)
                    {
                        status ret=_(handler)(tmp_adapter,hana::append(_(parent_path),wrap_it(it,_(aggr),el_aggregation.modifier)),_(used_path_size));
                        if (!pred(ret))
                        {
                            aggregate_report<AdapterT>::close(_(adapter),ret);
                            return ret;
                        }
                        empty=false;
                    }
                    auto ret=empt(empty);
                    aggregate_report<AdapterT>::close(_(adapter),ret);
                    return ret;
                },
                [&](auto&&)
                {
                    return hana::eval_if(
                        is_heterogeneous_container(parent_element),
                        [&](auto&& _)
                        {
                            // agrregation can be invoked on heterogeneous container types
                            auto tmp_adapter=make_intermediate_adapter(_(adapter),_(parent_path));
                            aggregate_report<AdapterT>::open(_(adapter),_(aggr),_(parent_path));
                            auto ret=foreach_if(
                                            parent_element,
                                            pred,
                                            [&](auto&&, auto&& index)
                                            {
                                                return _(handler)(tmp_adapter,hana::append(_(parent_path),wrap_heterogeneous_index(index,_(aggr))),_(used_path_size));
                                            }
                                        );
                            aggregate_report<AdapterT>::close(_(adapter),ret);
                            return ret;
                        },
                        [&](auto&& _)
                        {
                            // skip aggregation if not a container type
                            return _(handler)(_(adapter),std::forward<decltype(path)>(path),hana::plus(_(used_path_size),hana::size_c<1>));
                        }
                    );
                }
            );
        },
        [](auto&&)
        {
            return status{status::code::ignore};
        }
    );
}

//-------------------------------------------------------------

template <typename PredicateT, typename EmptyFnT, typename AggregationT,
          typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
status element_aggregation::invoke_variadic(PredicateT&& pred, EmptyFnT&& empt, AggregationT&& aggr,
                     UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler)
{
    auto compacted_path=compact_variadic_property(path);
    auto parent_compacted_path=hana::drop_back(compacted_path);
    if (!embedded_object_has_path(adapter,parent_compacted_path))
    {
        return traits_of(adapter).not_found_status();
    }

    return hana::eval_if(
        is_embedded_object_path_valid(adapter,parent_compacted_path),
        [&](auto&& _)
        {
            auto upper_path=hana::drop_back(_(path));
            auto&& parent=embedded_object_member(_(adapter),_(parent_compacted_path));
            const auto& aggregation_varg=unwrap_object(hana::back(_(path)));

            auto tmp_adapter=make_intermediate_adapter(_(adapter),_(parent_compacted_path));

            aggregate_report<AdapterT>::open(_(adapter),_(aggr),_(parent_compacted_path));
            bool empty=true;
            for (auto it=aggregation_varg.begin(parent);
                 aggregation_varg.while_cond(parent,it);
                 aggregation_varg.next(parent,it)
                )
            {
                status ret=_(handler)(tmp_adapter,hana::append(upper_path,varg(wrap_index(it,_(aggr)))),_(used_path_size));
                if (!pred(ret))
                {
                    aggregate_report<AdapterT>::close(_(adapter),ret);
                    return ret;
                }
                empty=false;
            }
            auto result=empt(empty);
            aggregate_report<AdapterT>::close(_(adapter),result);
            return result;
        },
        [](auto&&)
        {
            return status{status::code::ignore};
        }
    );
}

//-------------------------------------------------------------

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
                        make_aggregation_validator(
                            detail::aggregate_all,
                            std::forward<decltype(op)>(op)
                        )
                   );
        }
    )(std::forward<OpT>(op));
}

//-------------------------------------------------------------

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
                        make_aggregation_validator(
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
