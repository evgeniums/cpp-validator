/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/adapter_traits_wrapper.hpp
*
*  Defines adapter_traits_wrapper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADAPTER_TRAITS_WRAPPER_HPP
#define DRACOSHA_VALIDATOR_ADAPTER_TRAITS_WRAPPER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct adapter_traits_wrapper_tag{};

template <typename TraitsT>
struct adapter_traits_wrapper : public adapter_traits_wrapper_tag,
                                public TraitsT::base_tag
{
    using expand_aggregation_members=typename TraitsT::expand_aggregation_members;
    using filter_if_not_exists=typename TraitsT::filter_if_not_exists;
    using base_tag=typename TraitsT::base_tag;

    adapter_traits_wrapper(TraitsT& traits) : _traits(traits)
    {}

    TraitsT& wrapped()
    {
        return _traits;
    }

    const TraitsT& wrapped() const
    {
        return _traits;
    }

    TraitsT& _traits;
};

template <typename AdapterT>
auto traits_of(AdapterT&& adapter) -> decltype(auto)
{
    return hana::if_(
        std::is_base_of<adapter_traits_wrapper_tag,typename std::decay_t<AdapterT>::type>{},
        [](auto&& adapter) -> decltype(auto)
        {
            return adapter.traits().wrapped();
        },
        [](auto&& adapter) -> decltype(auto)
        {
            return adapter.traits();
        }
    )(std::forward<AdapterT>(adapter));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_TRAITS_WRAPPER_HPP
