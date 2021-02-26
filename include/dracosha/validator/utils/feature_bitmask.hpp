/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/feature_bitmask.hpp
*
* Defines feature bitmask.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FEATURE_BITMASK_HPP
#define DRACOSHA_VALIDATOR_FEATURE_BITMASK_HPP

#include <cstddef>
#include <initializer_list>
#include <bitset>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
template <typename FeatureEnumT, typename =hana::when<true>>
struct feature_bitmask_helper
{
    using type=uint32_t;
};
template <typename FeatureEnumT>
struct feature_bitmask_helper<
            FeatureEnumT,
            hana::when<(
                FeatureEnumT::END_ <= 8
            )>
        >
{
    using type=uint8_t;
};
template <typename FeatureEnumT>
struct feature_bitmask_helper<
            FeatureEnumT,
            hana::when<(
                FeatureEnumT::END_ > 8
                &&
                FeatureEnumT::END_ <= 16
            )>
        >
{
    using type=uint16_t;
};
template <typename FeatureEnumT>
struct feature_bitmask_helper<
            FeatureEnumT,
            hana::when<(
                FeatureEnumT::END_ > 32
                &&
                FeatureEnumT::END_ <= 64
            )>
        >
{
    using type=uint64_t;
};
template <typename FeatureEnumT>
struct feature_bitmask_helper<
            FeatureEnumT,
            hana::when<(
                FeatureEnumT::END_ > 64
            )>
        >
{
};
}

/**
 * @brief Template for enum'ed feature bitmasks.
 *
 * Enum FeatureEnumT must define a sequence of bitmask having sequential ids starting from 0.
 * The last item in FeatureEnumT must be named END_.
 *
 */
template <typename FeatureEnumT>
struct feature_bitmask_t
{
    using bitmask=typename detail::feature_bitmask_helper<FeatureEnumT>::type;
    using feature=FeatureEnumT;

    /**
     * @brief Convert feature to bitmask.
     * @param ft Feature for converting.
     * @return Bitmask where bit corresponding to the feature is set to 1.
     */
    template <typename T>
    constexpr static bitmask bit(const T& ft) noexcept
    {
        return bitmask(1)<<static_cast<size_t>(ft);
    }

    /**
     * @brief Convert list of features to bitmask.
     * @param bitmask Features for converting.
     * @return Bitmask where bits corresponding to the bitmask are set to 1.
     */
    constexpr static bitmask bits(const std::initializer_list<feature>& fts) noexcept
    {
        bitmask mask=0;
        for (auto&& ft:fts)
        {
            mask|=bit(ft);
        }
        return mask;
    }

    /**
     * @brief Convert list of features to bitmask.
     * @param bitmask Features for converting.
     * @return Bitmask where bits corresponding to the bitmask are set to 1.
     */
    template <typename ... Features>
    constexpr static bitmask bits(Features&&... fts) noexcept
    {
        return hana::fold(
            hana::make_tuple(std::forward<Features>(fts)...),
            bitmask(0),
            [](bitmask mask, const auto& ft)
            {
                mask|=bit(ft);
                return mask;
            }
        );
    }

    /**
     * @brief Get bitmask where all bitmask are set.
     * @return Bitmask with all bitmask enabled.
     */
    constexpr static bitmask all_bits() noexcept
    {
        bitmask mask=0;
        for (size_t i=0;i<static_cast<size_t>(feature::END_);i++)
        {
            mask|=(1<<i);
        }
        return mask;
    }

    /**
     * @brief Count number of enabled features.
     * @param bitmask Bitmask of features to count.
     * @return Number of enabled features.
     */
    constexpr static size_t count(const bitmask& mask) noexcept
    {
        std::bitset<static_cast<size_t>(FeatureEnumT::END_)> bitset(mask);
        return bitset.count();
    }

    /**
     * @brief Count number of matched features.
     * @param features1 First bitmask of features.
     * @param features2 Second bitmask of features.
     * @return Number of matched features.
     */
    constexpr static size_t count(const bitmask& features1, const bitmask& features2) noexcept
    {
        return count(features1&features2);
    }

    /**
     * @brief Check if a feature is set in bitmask.
     * @param mask Bitmask to query.
     * @param ft Feature to check for.
     * @return Result.
     */
    template <typename T>
    constexpr static bool is_set(const bitmask& mask, const T& ft) noexcept
    {
        return mask&bit(ft);
    }
};

/**
 * @brief Helper for operations with feature bitmasks.
 */
template <typename FeatureEnumT>
constexpr feature_bitmask_t<FeatureEnumT> feature_bitmask{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FEATURE_BITMASK_HPP
