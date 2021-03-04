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
#include <dracosha/validator/adapters/impl/reporting_adapter_impl.hpp>
#include <dracosha/validator/detail/aggregate_all.hpp>
#include <dracosha/validator/detail/aggregate_any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename AdapterT, typename=hana::when<true>>
struct aggregate_report
{
    template <typename AdapterT1, typename AggregationT, typename MemberT>
    static void open(AdapterT1&&, AggregationT&&, MemberT&&)
    {}

    template <typename AdapterT1>
    static void close(AdapterT1&&, status)
    {}
};

template <typename AdapterT>
struct aggregate_report<AdapterT,
        hana::when<
            std::is_base_of<reporting_adapter_tag,typename std::decay_t<AdapterT>::type>::value
        >>
{
    template <typename AdapterT1, typename AggregationT, typename PathT>
    static void open(AdapterT1&& adapter, AggregationT&& str, PathT&& path)
    {
        auto& reporter=adapter.traits().reporter();
        using type=std::decay_t<decltype(reporter)>;
        auto& mutable_reporter=const_cast<type&>(reporter);
        hana::eval_if(
            hana::equal(hana::size(path),hana::size_c<0>),
            [&](auto&&)
            {
                mutable_reporter.aggregate_open(str);
            },
            [&](auto&& _)
            {
                mutable_reporter.aggregate_open(str,make_member(_(path)));
            }
        );
    }

    template <typename AdapterT1>
    static void close(AdapterT1&& adapter, status ret)
    {
        auto& reporter=adapter.traits().reporter();
        using type=std::decay_t<decltype(reporter)>;
        auto& mutable_reporter=const_cast<type&>(reporter);
        mutable_reporter.aggregate_close(ret);
    }
};

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
                    aggregate_report<AdapterT>::open(adapter,str,parent_path);
                    bool empty=true;
                    for (auto it=_(parent_element).begin();it!=_(parent_element).end();++it)
                    {
                        status ret=_(handler)(hana::append(parent_path,wrap_it(it,str)));
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

template <typename OpT>
constexpr auto all_t::operator() (OpT&& op) const
{
    return make_validator(
                hana::reverse_partial(
                    detail::aggregate_all,
                    std::forward<OpT>(op)
                )
           );
}

template <typename OpT>
constexpr auto any_t::operator() (OpT&& op) const
{
    return make_validator(
                hana::reverse_partial(
                    detail::aggregate_any,
                    std::forward<OpT>(op)
                )
           );
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_IPP
