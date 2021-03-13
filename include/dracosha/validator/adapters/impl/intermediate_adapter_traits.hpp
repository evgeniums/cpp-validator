/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/impl/intermediate_adapter_traits.hpp
*
*  Defines "intermediate_adapter_traits".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_INTERMEDIATE_ADAPTER_TRAITS_HPP
#define DRACOSHA_VALIDATOR_INTERMEDIATE_ADAPTER_TRAITS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct intermediate_adapter_tag{};

template <typename T, typename Enable=hana::when<true>>
struct base_intermediate_adapter_tag : public intermediate_adapter_tag
{
};
template <typename T>
struct base_intermediate_adapter_tag<T,
            hana::when<!std::is_base_of<intermediate_adapter_tag,T>::value>
        >
{
};

template <typename BaseTraitsT, typename IntermediateT, typename PathPrefixLengthT>
class intermediate_adapter_traits : public BaseTraitsT,
                                    public base_intermediate_adapter_tag<std::decay_t<BaseTraitsT>>
{
    public:

        template <typename BaseTraitsT1>
        intermediate_adapter_traits(
                BaseTraitsT1&& traits,
                IntermediateT&& intermediate,
                PathPrefixLengthT path_prefix_length
            )
            : BaseTraitsT(std::forward<BaseTraitsT1>(traits)),
              _intermediate(std::forward<IntermediateT>(intermediate)),
              _path_prefix_length(path_prefix_length)
        {}

        auto value() const -> decltype(auto)
        {
            return _intermediate.get();
        }
        auto value() -> decltype(auto)
        {
            return _intermediate.get();
        }

        template <typename OriginalPathT>
        auto path(const OriginalPathT& path) const
        {
            return hana::drop_front(path,_path_prefix_length);
        }

    private:

        object_wrapper<IntermediateT> _intermediate;
        PathPrefixLengthT _path_prefix_length;
};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INTERMEDIATE_ADAPTER_TRAITS_HPP
