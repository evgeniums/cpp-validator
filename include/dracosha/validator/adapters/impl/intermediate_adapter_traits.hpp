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
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>
#include <dracosha/validator/adapters/adapter_traits_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Tag of intermediate adapters.
 */
struct intermediate_adapter_tag{};

/**
 * @brief Traits of intermediate adapters.
 * @param BaseTraitsT Base traits this intermediate adapter will wrap.
 * @param IntermediateT Type of intermediate member's value.
 * @param PathPrefixLengthT Length of member's path already used to extract the intermidiate value.
 *
 * Intermediate adapters are used to hold pre-extracted intermediate values (references) of nested members
 * in order to extract full member path only once per aggregation.
 */
template <typename BaseTraitsT, typename IntermediateT, typename PathPrefixLengthT>
class intermediate_adapter_traits : public adapter_traits_wrapper<BaseTraitsT>,
                                    public intermediate_adapter_tag
{
    public:

        using path_prefix_length=std::decay_t<PathPrefixLengthT>;

        /**
         * @brief Constructor.
         * @param traits Base traits this intermediate adapter will wrap.
         * @param intermediate Intermediate member's value.
         * @param path_prefix_size Length of member's path already used to extract the intermidiate value.
         */
        template <typename BaseTraitsT1>
        intermediate_adapter_traits(
                BaseTraitsT1&& traits,
                IntermediateT&& intermediate,
                PathPrefixLengthT path_prefix_size
            )
            : adapter_traits_wrapper<BaseTraitsT>(std::forward<BaseTraitsT1>(traits)),
              _intermediate(std::forward<IntermediateT>(intermediate)),
              _path_prefix_length(path_prefix_size)
        {}

        /**
         *  @brief Get intermediate member's value.
         */
        auto value() const -> decltype(auto)
        {
            return _intermediate.get();
        }

        /**
         *  @brief Get intermediate member's value.
         */
        auto value() -> decltype(auto)
        {
            return _intermediate.get();
        }

        /**
         *  @brief Get remaining member's path skipping already used part of path of path_prefix_length.
         *  @param path Full member's path.
         *  @return Remaining path excluding already used prefix.
         */
        template <typename OriginalPathT>
        auto path(const OriginalPathT& path) const
        {
            return hana::drop_front(path,_path_prefix_length);
        }

        /**
         * @brief Check if full path is already used and the exists() checking must be ignored.
         * @param PathSizeT Integral constant holding full path size.
         * @return Logical integral constant.
         */
        template <typename PathSizeT>
        constexpr static auto ignore_exists(PathSizeT)
        {
            return hana::less_equal(
                        PathSizeT{},
                        path_prefix_length{}
                    );
        }

    private:

        object_wrapper<IntermediateT> _intermediate;
        PathPrefixLengthT _path_prefix_length;
};

//-------------------------------------------------------------

namespace detail
{
/**
 * @brief Implementer of intermediate_ignore_check_exist.
 */
struct intermediate_ignore_check_exist_impl
{
    template <typename AdapterT, typename MemberT>
    constexpr auto operator() (AdapterT&&, MemberT&&) const
    {
        using traits=typename std::decay_t<AdapterT>::type;
        return traits::ignore_exists(
                        hana::size_c<std::decay_t<MemberT>::path_depth()>
                    );
    }
};
/**
 * @brief Helper to check if full member's path is already used by intermediate adapter and the exists() checking must be ignored.
 * @param AdapterT Intermediate adapter.
 * @param MemberT Member.
 * @return Logical integral constant.
 */
constexpr intermediate_ignore_check_exist_impl intermediate_ignore_check_exist{};

/**
 * @brief Implementer of intermediate_ignore_check_exist_false.
 */
struct intermediate_ignore_check_exist_false_impl
{
    template <typename AdapterT, typename MemberT>
    constexpr auto operator() (AdapterT&&, MemberT&&) const
    {
        return hana::false_c;
    }
};
/**
 * @brief Helper returning logical false integral constant to be used with intermediate_ignore_check_exist.
 */
constexpr intermediate_ignore_check_exist_false_impl intermediate_ignore_check_exist_false{};

}

/**
 * @brief Implementer intermediate_ignore_check_exist.
 */
struct intermediate_ignore_check_exist_impl
{
    template <typename AdapterT, typename MemberT>
    constexpr auto operator() (AdapterT&& adapter, MemberT&& member) const
    {
        return hana::if_(
            std::is_base_of<intermediate_adapter_tag,typename std::decay_t<AdapterT>::type>{},
            detail::intermediate_ignore_check_exist,
            detail::intermediate_ignore_check_exist_false
        )(adapter,member);
    }
};
/**
 * @brief Check if full member's path is already used by intermediate adapter and the exists() checking must be ignored.
 * @param adapter Intermediate adapter.
 * @param member Member.
 * @return Logical integral constant.
 */
constexpr intermediate_ignore_check_exist_impl intermediate_ignore_check_exist{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INTERMEDIATE_ADAPTER_TRAITS_HPP
