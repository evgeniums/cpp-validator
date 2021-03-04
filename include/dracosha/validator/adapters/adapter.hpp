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
class adapter
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
            : _traits(*this,std::forward<Args>(args)...)
        {}

        /**
         * @brief Get traits.
         * @return Reference to traits.
         */
        TraitsT& traits()
        {
            return _traits;
        }

        /**
         * @brief Get constant traits.
         * @return Const reference to traits.
         */
        const TraitsT& traits() const
        {
            return _traits;
        }

        /**
         *  @brief Perform validation of plain value.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         *  @return Validation status.
         */
        template <typename T2, typename OpT>
        status validate_operator(OpT&& op, T2&& b) const
        {
            return _traits.validate_operator(*this,std::forward<OpT>(op),std::forward<T2>(b));
        }

        /**
         *  @brief Perform validation of property.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         *  @return Validation status.
         */
        template <typename T2, typename OpT, typename PropT>
        status validate_property(PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate_property(*this,std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<T2>(b));
        }

        /**
         *  @brief Validate existance of a member.
         *  @param a Object to validate.
         *  @param member Member descriptor.
         *  @param op Operator for validation.
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist.
         *  @param from_check_member Boolean flag, when true then the method is called before other validation to check ig method exists.
         *  @return Validation status.
         */
        template <typename T2, typename OpT, typename MemberT>
        status validate_exists(MemberT&& member, OpT&& op, T2&& b, bool from_check_member=false) const
        {
            return _traits.validate_exists(*this,std::forward<MemberT>(member),std::forward<OpT>(op),std::forward<T2>(b),from_check_member);
        }

        /**
         * @brief Check if path exists in object.
         * @param path Path to check.
         * @param b Boolean flag to compare result with.
         * @return Operation result.
         */
        template <typename T2, typename PathT>
        bool check_path_exists(PathT&& path, T2&& b) const
        {
            const auto& obj=extract(traits().get());
            return hana::if_(check_member_path(obj,path),
                [&obj,&b](auto&& path)
                {
                    return exists(obj,std::forward<decltype(path)>(path))==b;
                },
                [&b](auto&&)
                {
                    return b==false;
                }
            )(path);
        }

        /**
         *  @brief Normal validation of a member.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample argument for validation.
         *  @return Validation status.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        status validate(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate(*this,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),
                            std::forward<T2>(b));
        }

        /**
         *  @brief Validate using other member of the same object as a operand for validation.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Descriptor of operand member of the same object.
         *  @return Validation status.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_other_member(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate_with_other_member(*this,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),
                                              std::forward<T2>(b));
        }

        /**
         *  @brief Validate using the same member of sample object.
         *  @param member Member descriptor.
         *  @param prop Property to validate.
         *  @param op Operator for validation.
         *  @param b Sample object whose member must be used as argument passed to validation operator.
         *  @return Validation status.
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_master_sample(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate_with_master_sample(*this,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),
                                              std::forward<T2>(b));
        }

        /**
         * @brief Execute validators and aggregate their results using logical AND.
         * @param ops List of intermediate validators or validation operators.
         * @return Logical AND of results of intermediate validators.
         */
        template <typename OpsT>
        status validate_and(OpsT&& ops) const
        {
            return _traits.validate_and(*this,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on the member of object and aggregate their results using logical AND.
         * @param member Member to apply validators to.
         * @param ops List of intermediate validators or validation operators.
         * @return Logical AND of results of intermediate validators.
         */
        template <typename MemberT, typename OpsT>
        status validate_and(MemberT&& member, OpsT&& ops) const
        {
            return _traits.validate_and(*this,std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators and aggregate their results using logical OR.
         * @param ops List of intermediate validators or validation operators.
         * @return Logical OR of results of intermediate validators.
         */
        template <typename OpsT>
        status validate_or(OpsT&& ops) const
        {
            return _traits.validate_or(*this,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on the member of object and aggregate their results using logical OR.
         * @param member Member to apply validators to.
         * @param ops List of intermediate validators or validation operators.
         * @return Logical OR of results of intermediate validators.
         */
        template <typename MemberT, typename OpsT>
        status validate_or(MemberT&& member, OpsT&& ops) const
        {
            return _traits.validate_or(*this,std::forward<decltype(member)>(member),std::forward<decltype(ops)>(ops));
        }

        /**
         * @brief Execute validator object and negate the result.
         * @param op Intermediate validator or validation operator.
         * @return Logical NOT of results of intermediate validator.
         */
        template <typename OpT>
        status validate_not(OpT&& op) const
        {
            return _traits.validate_not(*this,std::forward<decltype(op)>(op));
        }

        /**
         * @brief Execute validator on the member of object and negate the result.
         * @param member Member to process validator to.
         * @param op Intermediate validator or validation operator.
         * @return Logical NOT of results of intermediate validator.
         */
        template <typename MemberT, typename OpT>
        status validate_not(MemberT&& member, OpT&& op) const
        {
            return _traits.validate_not(*this,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if at least one element satisfies validation condition.
         * @param op Validation operator.
         * @return Validation status.
         */
        template <typename OpT>
        status validate_any(OpT&& op) const
        {
            return _traits.validate_any(*this,std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if at least one element of the member satisfies validation condition.
         * @param member Member to validate.
         * @param op Validation operator.
         * @return Validation status.
         */
        template <typename MemberT, typename OpT>
        status validate_any(MemberT&& member, OpT&& op) const
        {
            return _traits.validate_any(*this,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if all elements satisfy validation condition.
         * @param op Validation operator.
         * @return Validation status.
         */
        template <typename OpT>
        status validate_all(OpT&& op) const
        {
            return _traits.validate_all(*this,std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if all elements of the member satisfy validation condition.
         * @param member Member to validate.
         * @param op Validation operator.
         * @return Validation status.
         */
        template <typename MemberT, typename OpT>
        status validate_all(MemberT&& member, OpT&& op) const
        {
            return _traits.validate_all(*this,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
        }

        /**
         * @brief Call hint processing before validation.
         * @param hint Hint.
         */
        template <typename HintT>
        status hint_before(HintT&& hint) const
        {
            return detail::hint_before<TraitsT,HintT>(_traits,std::forward<HintT>(hint));
        }

        /**
         * @brief Call hint processing after validation.
         * @param hint Hint.
         */
        template <typename HintT>
        status hint_after(status validation_status, HintT&& hint) const
        {
            return detail::hint_after<TraitsT,HintT>(_traits,validation_status,std::forward<HintT>(hint));
        }

    private:

        mutable TraitsT _traits;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HPP
