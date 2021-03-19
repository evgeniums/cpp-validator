/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/make_intermediate_adapter.hpp
*
*  Defines "make_intermediate_adapter".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MAKE_INTERMEDIATE_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_MAKE_INTERMEDIATE_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/adapters/impl/intermediate_adapter_traits.hpp>
#include <dracosha/validator/embedded_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct make_intermediate_adapter_impl
{
    template <typename AdapterT, typename PathT, typename PathPrefixSizeT=std::decay_t<decltype(hana::size(std::declval<PathT>()))>>
    auto operator () (AdapterT&& adapter, PathT&& path, PathPrefixSizeT path_prefix_length=PathPrefixSizeT{}) const
    {
        auto create=[&](auto&& current_traits)
        {
            auto&& traits=hana::if_(
                std::is_base_of<adapter_traits_wrapper_tag,std::decay_t<decltype(current_traits)>>{},
                [](auto&& current_traits) -> decltype(auto)
                {
                    return current_traits.wrapped();
                },
                [](auto&& current_traits) -> decltype(auto)
                {
                    return hana::id(std::forward<decltype(current_traits)>(current_traits));
                }
            )(std::forward<decltype(current_traits)>(current_traits));

            auto&& obj=embedded_object_member(adapter,path);
            return intermediate_adapter_traits<
                        std::decay_t<decltype(traits)>,
                        decltype(obj),
                        decltype(path_prefix_length)
                    >{
                        traits,
                        obj,
                        path_prefix_length
                     };
        };
        return adapter.clone(create);
    }
};
constexpr make_intermediate_adapter_impl make_intermediate_adapter{};

//-------------------------------------------------------------

struct clone_intermediate_adapter_impl
{
    template <typename AdapterT, typename NewValueT, typename PathPrefixSizeT>
    auto operator () (AdapterT&& adapter, NewValueT&& new_value, PathPrefixSizeT path_prefix_size) const
    {
        auto create=[&](auto&& current_traits)
        {
            auto&& traits=current_traits.wrapped();
            return intermediate_adapter_traits<
                        std::decay_t<decltype(traits)>,
                        NewValueT,
                        PathPrefixSizeT
                    >{
                        traits,
                        std::forward<NewValueT>(new_value),
                        path_prefix_size
                     };
        };
        return adapter.clone(create);
    }

    template <typename AdapterT, typename NewValueT>
    auto operator () (AdapterT&& adapter, NewValueT&& new_value) const
    {
        using path_prefix_length=typename std::decay_t<AdapterT>::type::path_prefix_length;
        return (*this)(std::forward<AdapterT>(adapter),std::forward<NewValueT>(new_value),path_prefix_length{});
    }
};
constexpr clone_intermediate_adapter_impl clone_intermediate_adapter{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MAKE_INTERMEDIATE_ADAPTER_HPP
