/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/aggregation.ipp
*
*  Defines aggregation used for logical operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATION_IPP
#define DRACOSHA_VALIDATOR_AGGREGATION_IPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/aggregation.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename PredicateT, typename EmptyFnT, typename StringT, typename PathT, typename AdapterT, typename HandlerT>
status element_aggregation::invoke(PredicateT&& pred, EmptyFnT&& empt, StringT&& str, PathT&& path, AdapterT&& adapter, HandlerT&& handler)
{
    const auto& obj=extract(adapter.traits().get());
    const auto parent_path=hana::drop_back(path);

    auto&& parent=make_member(parent_path);
    if (!adapter.traits().check_member_exists(parent))
    {
        return adapter.traits().not_found_status();
    }

    const auto& parent_element=get_member(obj,parent_path);
    return hana::eval_if(
        is_container_t<std::decay_t<decltype(parent_element)>>{},
        [&](auto&& _)
        {
            bool empty=true;
            for (auto it=_(parent_element).begin();it!=_(parent_element).end();++it)
            {
                status ret=_(handler)(hana::append(parent_path,wrap_it(it,str)));
                if (!pred(ret))
                {
                    return ret;
                }
                empty=false;
            }
            return empt(empty);
        },
        [&](auto&& _)
        {
            return _(handler)(std::forward<decltype(path)>(path));
        }
    );
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_IPP
