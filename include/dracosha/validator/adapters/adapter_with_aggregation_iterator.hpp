/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/adapter_with_aggregation_iterator.hpp
*
*  Helpers for daoters with aggregation iterators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WITH_AGGREGATION_ITERATOR_HPP
#define DRACOSHA_VALIDATOR_WITH_AGGREGATION_ITERATOR_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper to wrap adapter with aggregation iterator.
 *
 * Default implementation returns adapter "as is" ignoring iterator.
 */
template <typename AdapterT, typename IteratorT, typename Enable=hana::when<true>>
struct adapter_with_aggregation_iterator_t
{
    template <typename AdapterT1, typename IteratorT1>
    auto operator() (AdapterT1&& adapter, IteratorT1&&) const -> decltype(auto)
    {
        return hana::id(std::forward<AdapterT1>(adapter));
    }
};
template <typename AdapterT, typename IteratorT>
constexpr adapter_with_aggregation_iterator_t<AdapterT,IteratorT> adapter_with_aggregation_iterator_inst{};

/**
 * @brief Wrap adapter with aggregation iterator.
 * @param adapter Adapter to wrap.
 * @param it Iterator of container used in aggregation operation.
 * @return Wrapped adapter depending on adapter type. In default implementation the adapter itself is returned.
 */
template <typename AdapterT, typename IteratorT>
auto adapter_with_aggregation_iterator(AdapterT&& adapter, IteratorT&& it) -> decltype(auto)
{
    return adapter_with_aggregation_iterator_inst<std::decay_t<AdapterT>,std::decay_t<IteratorT>>
           (std::forward<AdapterT>(adapter),std::forward<IteratorT>(it));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WITH_AGGREGATION_ITERATOR_HPP
