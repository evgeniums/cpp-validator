/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/aggregation/tree.hpp
*
* Defines helpers for validatiion of trees.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_TREE_HPP
#define DRACOSHA_VALIDATOR_TREE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_ignore.hpp>
#include <dracosha/validator/filter_member.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/aggregation/element_aggregation.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct tree_tag;

template <typename AggregationT, typename PropertyT, typename MaxArgT>
struct tree_t : public adjust_storable_ignore
{
    using hana_tag=tree_tag;

    AggregationT aggregation;
    PropertyT property;
    MaxArgT max_arg;

    template <typename AggregationT1, typename PropertyT1, typename MaxArgT1>
    tree_t(AggregationT1&& aggr, PropertyT1&& prop, MaxArgT1&& mx_arg)
        : aggregation(std::forward<AggregationT1>(aggr)),
          property(std::forward<PropertyT1>(prop)),
          max_arg(std::forward<MaxArgT1>(mx_arg))
    {}
};

struct tree_impl
{
    template <typename AggregationT, typename PropertyT, typename MaxArgT>
    auto operator() (AggregationT&& aggr, PropertyT&& prop, MaxArgT&& max_arg) const
    {
        return tree_t<std::decay_t<AggregationT>,std::decay_t<PropertyT>,std::decay_t<MaxArgT>>
        {
            std::forward<AggregationT>(aggr),
            std::forward<PropertyT>(prop),
            std::forward<MaxArgT>(max_arg)
        };
    }
};
constexpr tree_impl tree{};

template <typename TreeKeyT, typename AdapterT, typename PredT, typename HandlerT, typename UsedPathSizeT,
          typename PathT, typename NodeT, typename VargT>
status each_tree_node(const TreeKeyT& tree_key, AdapterT& tmp_adapter, const PredT& pred,
                      const HandlerT& handler, const UsedPathSizeT& used_path_size,
                      const PathT& path, NodeT&& node, const VargT& aggregation_varg
                      )
{
    for (auto it=aggregation_varg.begin(node);
         aggregation_varg.while_cond(node,it);
         aggregation_varg.next(node,it)
        )
    {
        // handle content of current child node itself
        auto&& child_node=get_member(node,hana::make_tuple(tree_key.property,varg(it)));
        auto next_adapter=clone_intermediate_adapter(tmp_adapter,child_node);
        status ret=handler(next_adapter,path,used_path_size);
        if (!pred(ret))
        {
            return ret;
        }

        // recursive call for further iteration of children of current child node
        ret=each_tree_node(
                        tree_key,
                        next_adapter,
                        pred,
                        handler,
                        used_path_size,
                        path,
                        child_node,
                        aggregation_varg
                    );
        if (!pred(ret))
        {
            return ret;
        }

        // loop to handle next sibling child node
    }

    return status::code::ignore;
}

template <typename KeyT>
struct generate_paths_t<KeyT,hana::when<hana::is_a<tree_tag,KeyT>>>
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        auto upper_path=hana::drop_back(path);
        auto&& tree_key=hana::back(path);
        auto pred=hana::partial(tree_key.aggregation.predicate(),adapter);

        auto reporting_path=hana::append(upper_path,varg(wrap_it(0,tree_key.aggregation.string(),tree_modifier)));

        // handle top node
        auto tmp_adapter=make_intermediate_adapter(adapter,upper_path,used_path_size);
        auto&& begin_node=embedded_object_member(tmp_adapter,upper_path);
        auto next_adapter=clone_intermediate_adapter(tmp_adapter,begin_node,hana::size(reporting_path));
        status ret=handler(next_adapter,reporting_path,used_path_size);
        if (!pred(ret))
        {
            return ret;
        }

        // iterate over children nodes
        aggregate_report<AdapterT>::open(next_adapter,tree_key.aggregation.string(),upper_path);
        auto aggregation_varg=varg(tree_key.aggregation,tree_key.max_arg);
        auto result=each_tree_node(
                        tree_key,
                        next_adapter,
                        pred,
                        handler,
                        used_path_size,
                        reporting_path,
                        begin_node,
                        aggregation_varg.get()
                    );
        if (result==status::code::ignore)
        {
            result=ret;
        }
        aggregate_report<AdapterT>::close(adapter,result);
        return result;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_TREE_HPP
