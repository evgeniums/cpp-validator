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

/**
 * @brief Tag of all adapter traits that wrap other adapter traits.
 */
struct adapter_traits_wrapper_tag{};

/**
 * @brief Adapter traits used to wrap other adapter traits.
 * This traits implement "decorator" design pattern. Consequently, this traits can be used to add additional
 * behaviors to original adapters.
 */
template <typename TraitsT>
struct adapter_traits_wrapper : public adapter_traits_wrapper_tag,
                                public TraitsT::base_tag
{
    using expand_aggregation_members=typename TraitsT::expand_aggregation_members;
    using filter_if_not_exists=typename TraitsT::filter_if_not_exists;
    using base_tag=typename TraitsT::base_tag;

    /**
     * @brief Constructor.
     * @param traits Original adapter traits.
     */
    adapter_traits_wrapper(TraitsT& traits) : _traits(traits)
    {}

    /**
     * @brief Get original adapter traits.
     * @return Original adapter traits.
     */
    TraitsT& wrapped()
    {
        return _traits;
    }

    /**
     * @brief Get original adapter traits.
     * @return Original adapter traits.
     */
    const TraitsT& wrapped() const
    {
        return _traits;
    }

    TraitsT& _traits;
};

/**
 * @brief Implementer of traits_of().
 */
struct traits_of_impl
{
    template <typename AdapterT>
    auto operator () (AdapterT&& adapter) const -> decltype(auto)
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
};
/**
 * @brief Get traits of adapter.
 * @param adapter Adapter.
 * @return Adapter traits or original adapter traits in case this is a wrapping adapter.
 */
constexpr traits_of_impl traits_of{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_TRAITS_WRAPPER_HPP
