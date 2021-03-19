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

struct intermediate_adapter_tag{};

template <typename BaseTraitsT, typename IntermediateT, typename PathPrefixLengthT>
class intermediate_adapter_traits : public adapter_traits_wrapper<BaseTraitsT>,
                                    public intermediate_adapter_tag
{
    public:

        using path_prefix_length=PathPrefixLengthT;

        template <typename BaseTraitsT1>
        intermediate_adapter_traits(
                BaseTraitsT1&& traits,
                IntermediateT&& intermediate,
                PathPrefixLengthT path_prefix_length
            )
            : adapter_traits_wrapper<BaseTraitsT>(std::forward<BaseTraitsT1>(traits)),
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

        template <typename PathSizeT>
        constexpr static auto ignore_exists(PathSizeT)
        {
            return hana::less_equal(
                        PathSizeT{},
                        PathPrefixLengthT{}
                    );
        }

    private:

        object_wrapper<IntermediateT> _intermediate;
        PathPrefixLengthT _path_prefix_length;
};

//-------------------------------------------------------------

struct intermediate_ignore_check_exist1_impl
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
constexpr intermediate_ignore_check_exist1_impl intermediate_ignore_check_exist1{};

struct intermediate_ignore_check_exist2_impl
{
    template <typename AdapterT, typename MemberT>
    constexpr auto operator() (AdapterT&&, MemberT&&) const
    {
        return hana::false_c;
    }
};
constexpr intermediate_ignore_check_exist2_impl intermediate_ignore_check_exist2{};

struct intermediate_ignore_check_exist_impl
{
    template <typename AdapterT, typename MemberT>
    constexpr auto operator() (AdapterT&& adapter, MemberT&& member) const
    {
        return hana::if_(
            std::is_base_of<intermediate_adapter_tag,typename std::decay_t<AdapterT>::type>{},
            intermediate_ignore_check_exist1,
            intermediate_ignore_check_exist2
        )(adapter,member);
    }
};
constexpr intermediate_ignore_check_exist_impl intermediate_ignore_check_exist{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INTERMEDIATE_ADAPTER_TRAITS_HPP
