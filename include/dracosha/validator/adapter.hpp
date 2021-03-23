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
#include <dracosha/validator/embedded_object.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/adapters/make_intermediate_adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct adapter_tag;

/**
 * @brief Base struct for validation adapter traits.
 */
struct adapter_traits
{
    /**
     *  @brief Logical integral constant saying whether the aggregation members should be expanded when member paths are generated.
     *
     *  Default is YES.
     */
    using expand_aggregation_members=std::integral_constant<bool,true>;

    /**
     *  @brief Logical integral constant saying whether not existing members should be filtered when member paths are generated.
     *
     *  Default is YES.
     */
    using filter_if_not_exists=std::integral_constant<bool,true>;

    /**
     * @brief Default implementation of validation of member aggregation.
     * @param pred Logical predicate of the aggregation.
     * @param adapter Validation adapter.
     * @param member Current member under validation.
     * @param ops Validation conditions.
     * @return Status of validation operation.
     */
    template <typename PredicateT, typename AdapterT, typename OpsT, typename MemberT>
    static status validate_member_aggregation(const PredicateT& pred, AdapterT&& adapter, MemberT&& member, OpsT&& ops)
    {
        auto tmp_adapter=make_intermediate_adapter(adapter,member.path());
        return while_each(
                  ops,
                  pred,
                  status(status::code::ignore),
                  [&member,&tmp_adapter](auto&& op)
                  {
                    return status(
                                apply_member(
                                    tmp_adapter,
                                    std::forward<decltype(op)>(op),
                                    member
                                )
                             );
                  }
              );
    }
};

/**
 * @brief Template of validation adapter.
 *
 * Actual validation must be implemented in adapter tratis.
 *
 */
template <typename TraitsT>
class adapter : public check_member_exists_traits_proxy<adapter<TraitsT>>
{
    public:

        using hana_tag=adapter_tag;
        using type=TraitsT;

        using expand_aggregation_members=typename TraitsT::expand_aggregation_members;

        /**
         * @brief Conctructor with single argument.
         * @param v Argument for forwarding to traits.
         *
         * GCC can not select copy constructor if there is also matching template constructor,
         * thus the constructor is spplit into multiple constructors.
         */
        template <typename T1>
        adapter(T1&& v, std::enable_if_t<!hana::is_a<adapter_tag,T1>,void*> =nullptr)
            : check_member_exists_traits_proxy<adapter<TraitsT>>(*this),
              _traits(std::forward<T1>(v))
        {
        }

        /**
         * @brief Conctructor with multiple arguments.
         * @param v1 First argument for forwarding to traits.
         * @param v2 Second argument for forwarding to traits.
         * @param args The rest arguments for forwarding to traits.
        */
        template <typename T1, typename T2, typename ...Args>
        adapter(T1&& v1, T2&& v2, Args&&... args)
            : check_member_exists_traits_proxy<adapter<TraitsT>>(*this),
              _traits(std::forward<T1>(v1),std::forward<T2>(v2),std::forward<Args>(args)...)
        {
        }

        /**
         * @brief Move constructor.
         * @param other Other adapter.
         */
        adapter(TraitsT&& other)
            : check_member_exists_traits_proxy<adapter<TraitsT>>(*this),
              _traits(std::move(other))
        {}

        /**
         * @brief Copy constructor.
         * @param other Other adapter.
         */
        adapter(const TraitsT& other)
            : check_member_exists_traits_proxy<adapter<TraitsT>>(*this),
              _traits(other)
        {}

        ~adapter() = default;
        adapter(adapter&& other) = default;
        adapter(const adapter& other) = default;
        adapter& operator = (adapter&& other) = default;
        adapter& operator = (const adapter& other) = default;

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

        /**
         * @brief Derive new adapter from this moving traits to new adapter.
         * @param creator Creater of traits of new adapter.
         * @return New adapter.
         *
         * This adapter becomes invalidated.
         */
        template <typename T>
        auto derive(T&& creator) &&
        {
            auto&& traits=creator(std::move(_traits));
            return adapter<std::decay_t<decltype(traits)>>{std::move(traits)};
        }

        /**
         * @brief Create new adapter copying traits to new adapter.
         * @param creator Creater of traits of new adapter.
         * @return New adapter.
         */
        template <typename T>
        auto clone(T&& creator) const
        {
            auto traits=creator(_traits);
            return adapter<std::decay_t<decltype(traits)>>{std::move(traits)};
        }

        /**
         * @brief Create new adapter copying traits to new adapter.
         * @param creator Creater of traits of new adapter.
         * @return New adapter.
         */
        template <typename T>
        auto clone(T&& creator)
        {
            auto traits=creator(_traits);
            return adapter<std::decay_t<decltype(traits)>>{std::move(traits)};
        }

        template <typename AdapterT, typename T2, typename PathT>
        /**
         * @brief Check if member paths exists in object.
         * @param adpt Validation adapter.
         * @param path Path.
         * @param b Boolean true/false to compare existence with.
         * @return Result of is_exists==b.
         */
        static bool check_path_exists(const AdapterT& adpt, PathT&& path, T2&& b)
        {
            return traits_of(adpt).check_path_exists(path,b);
        }

        /**
         * @brief Call hint processing before validation.
         * @param hint Hint.
         */
        template <typename AdapterT, typename HintT>
        static status hint_before(AdapterT& adpt, HintT&& hint)
        {
            return detail::hint_before<decltype(traits_of(adpt)),HintT>(traits_of(adpt),std::forward<HintT>(hint));
        }

        /**
         * @brief Call hint processing after validation.
         * @param hint Hint.
         */
        template <typename AdapterT, typename HintT>
        static status hint_after(AdapterT& adpt, status validation_status, HintT&& hint)
        {
            return detail::hint_after<decltype(traits_of(adpt)),HintT>(traits_of(adpt),validation_status,std::forward<HintT>(hint));
        }

    private:

        TraitsT _traits;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HPP
