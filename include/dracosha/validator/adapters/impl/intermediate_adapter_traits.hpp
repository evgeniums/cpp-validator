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

template <typename BaseTraitsT, typename IntermediateT, typename IndexT>
class intermediate_adapter_traits : public BaseTraitsT,
                                    public intermediate_adapter_tag
{
    public:

        template <typename BaseTraitsT1>
        intermediate_adapter_traits(
                BaseTraitsT1&& traits,
                IntermediateT&& intermediate,
                IndexT index
            )
            : BaseTraitsT(std::forward<BaseTraitsT1>(traits)),
              _intermediate(std::forward<IntermediateT>(intermediate)),
              _index(index)
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
            return hana::drop_front(path,_index);
        }

    private:

        object_wrapper<IntermediateT> _intermediate;
        IndexT _index;
};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INTERMEDIATE_ADAPTER_TRAITS_HPP
