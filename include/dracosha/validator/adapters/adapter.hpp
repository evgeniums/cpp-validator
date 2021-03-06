/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/adapter.hpp
*
*  Defines template of validation adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/detail/hint_helper.hpp>
#include <dracosha/validator/check_member_exists_traits_proxy.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct adapter_tag;

struct adapter_traits
{
    using expand_aggregation_members=std::integral_constant<bool,true>;
};

/**
 * @brief Template of validation adapter.
 *
 * Actual validation must be implemented in adapter tratis.
 *
 */
template <typename TraitsT>
class adapter : public check_member_exists_traits_proxy<TraitsT>
{
    public:

        using hana_tag=adapter_tag;
        using type=TraitsT;

        using expand_aggregation_members=typename TraitsT::expand_aggregation_members;

        /**
         * @brief Conctructor.
         * @param args Arguments for forwarding to traits.
         */
        template <typename ...Args>
        adapter(Args&&... args)
            : _traits(std::forward<Args>(args)...)
        {
            this->set_traits(&_traits);
        }

        //! @todo Move traits to private section, implement wrapping methods.

        /**
         * @brief Get traits.
         * @return Reference to traits.
         */
        TraitsT& traits() noexcept
        {
            return _traits;
        }

        /**
         * @brief Get constant traits.
         * @return Const reference to traits.
         */
        const TraitsT& traits() const noexcept
        {
            return _traits;
        }

        template <typename T>
        auto derive(T&& creator)
        {
            return creator(std::move(_traits));
        }

        template <typename AdapterT, typename T2, typename PathT>
        static bool check_path_exists(const AdapterT& adpt, PathT&& path, T2&& b)
        {
            return adpt.traits().check_path_exists(path,b);
        }

        /**
         * @brief Call hint processing before validation.
         * @param hint Hint.
         */
        template <typename AdapterT, typename HintT>
        static status hint_before(AdapterT& adpt, HintT&& hint)
        {
            return detail::hint_before<decltype(adpt.traits()),HintT>(adpt.traits(),std::forward<HintT>(hint));
        }

        /**
         * @brief Call hint processing after validation.
         * @param hint Hint.
         */
        template <typename AdapterT, typename HintT>
        static status hint_after(AdapterT& adpt, status validation_status, HintT&& hint)
        {
            return detail::hint_after<decltype(adpt.traits()),HintT>(adpt.traits(),validation_status,std::forward<HintT>(hint));
        }

    private:

        TraitsT _traits;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HPP
