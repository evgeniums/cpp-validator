/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/wrap_heterogeneous_index.hpp
*
*  Defines wrapper of heterogeneous index.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WRAP_HETEROGENEOUS_INDEX_HPP
#define DRACOSHA_VALIDATOR_WRAP_HETEROGENEOUS_INDEX_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/heterogeneous_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Wrapper of index of element of heterogeneous container for element aggregations.
 */
template <size_t Index, typename AggregationT>
struct wrap_heterogeneous_index_t :  public heterogeneous_property_t<Index>
{
    /**
     * @brief Name of index.
     */
    constexpr static auto name()
    {
        return AggregationT::description;
    }

    template <typename FormatterT>
    constexpr static const char* flag_str(bool, const FormatterT&, bool =false)
    {
        return nullptr;
    }

    constexpr static bool has_flag_str()
    {
        return false;
    }
};

/**
 * @brief Wrap index of heterogeneous container for element aggregation.
 * @param Ti Itegral constant index.
 * @param Ta Aggregation.
 * @return Wrapped heterogeneous index.
 */
template <typename Ti, typename Ta>
constexpr auto wrap_heterogeneous_index(Ti&&, Ta&&)
{
    using IndexT=std::decay_t<Ti>;
    return wrap_heterogeneous_index_t<
            IndexT::value,
            std::decay_t<Ta>
       >{};
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WRAP_HETEROGENEOUS_INDEX_HPP
