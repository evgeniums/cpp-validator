/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/adapter.hpp
*
*  Defines validation adapter template.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_ADAPTER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct adapter_tag;

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

        /**
         * @brief Conctructor
         * @param args Arguments fo forwarding to traits
         */
        template <typename ...Args>
        adapter(Args&&... args)
            : _traits(*this,std::forward<Args>(args)...)
        {}

        /**
         * @brief Get traits
         * @return Reference to traits
         */
        TraitsT& traits()
        {
            return _traits;
        }

        /**
         * @brief Get constant traits
         * @return Const reference to traits
         */
        const TraitsT& traits() const
        {
            return _traits;
        }

        /**
         *  @brief Perform validation of embedded object at one level without member nesting
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         *  @return Validation status
         */
        template <typename T2, typename OpT>
        status validate_operator(OpT&& op, T2&& b) const
        {
            return _traits.validate_operator(*this,std::forward<OpT>(op),std::forward<T2>(b));
        }

        /**
         *  @brief Perform validation of embedded object's property at one level without member nesting
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT>
        status validate_property(PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate_property(*this,std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<T2>(b));
        }

        /**
         *  @brief Validate existance of a member
         *  @param a Object to validate
         *  @param member Member descriptor
         *  @param op Operator for validation
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
         *  @param from_check_member Boolean flag, when true then the method is called before other validation to check ig method exists
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename MemberT>
        status validate_exists(MemberT&& member, OpT&& op, T2&& b, bool from_check_member=false) const
        {
            return _traits.validate_exists(*this,std::forward<MemberT>(member),std::forward<OpT>(op),std::forward<T2>(b),from_check_member);
        }

        /**
         *  @brief Normal validation of a member
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        status validate(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate(*this,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),
                            std::forward<T2>(b));
        }

        /**
         *  @brief Validate using other member of the same embedded object as a reference argument for validation
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Descriptor of sample member of the same embedded object
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_other_member(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate_with_other_member(*this,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),
                                              std::forward<T2>(b));
        }

        /**
         *  @brief Validate using the same member of a Sample embedded object
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample embedded object whose member must be used as argument passed to validation operator
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_master_sample(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
        {
            return _traits.validate_with_master_sample(*this,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),
                                              std::forward<T2>(b));
        }

        /**
         * @brief Execute validators on embedded object and aggregate their results using logical AND
         * @param ops List of intermediate validators or validation operators
         * @return Logical AND of results of intermediate validators
         */
        template <typename OpsT>
        status validate_and(OpsT&& ops) const
        {
            return _traits.validate_and(*this,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on the member of embedded object and aggregate their results using logical AND
         * @param member Member to process with validators
         * @param ops List of intermediate validators or validation operators
         * @return Logical AND of results of intermediate validators
         */
        template <typename MemberT, typename OpsT>
        status validate_and(MemberT&& member, OpsT&& ops) const
        {
            return _traits.validate_and(*this,std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on embedded object and aggregate their results using logical OR
         * @param ops List of intermediate validators or validation operators
         * @return Logical OR of results of intermediate validators
         */
        template <typename OpsT>
        status validate_or(OpsT&& ops) const
        {
            return _traits.validate_or(*this,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on the member of embedded object and aggregate their results using logical OR
         * @param member Member to process with validators
         * @param ops List of intermediate validators or validation operators
         * @return Logical OR of results of intermediate validators
         */
        template <typename MemberT, typename OpsT>
        status validate_or(MemberT&& member, OpsT&& ops) const
        {
            return _traits.validate_or(*this,std::forward<decltype(member)>(member),std::forward<decltype(ops)>(ops));
        }

        /**
         * @brief Execute validator on embedded object and negate the result
         * @param op Intermediate validator or validation operator
         * @return Logical NOT of results of intermediate validator
         */
        template <typename OpT>
        status validate_not(OpT&& op) const
        {
            return _traits.validate_not(*this,std::forward<decltype(op)>(op));
        }

        /**
         * @brief Execute validator on the member of embedded object and negate the result
         * @param member Member to process with validator
         * @param op Intermediate validator or validation operator
         * @return Logical NOT of results of intermediate validator
         */
        template <typename MemberT, typename OpT>
        status validate_not(MemberT&& member, OpT&& op) const
        {
            return _traits.validate_not(*this,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if at least one element satisfies validation condition
         * @param op Validation operator
         * @return Validation status
         */
        template <typename OpT>
        status validate_any(OpT&& op) const
        {
            return _traits.validate_any(*this,std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if at least one element of the member satisfies validation condition
         * @param member Member to validate
         * @param op Validation operator
         * @return Validation status
         */
        template <typename MemberT, typename OpT>
        status validate_any(MemberT&& member, OpT&& op) const
        {
            return _traits.validate_any(*this,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if all elements satisfy validation condition
         * @param op Validation operator
         * @return Validation status
         */
        template <typename OpT>
        status validate_all(OpT&& op) const
        {
            return _traits.validate_all(*this,std::forward<decltype(op)>(op));
        }

        /**
         * @brief Check if all elements of the member satisfy validation condition
         * @param member Member to validate
         * @param op Validation operator
         * @return Validation status
         */
        template <typename MemberT, typename OpT>
        status validate_all(MemberT&& member, OpT&& op) const
        {
            return _traits.validate_all(*this,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
        }

        void begin_explicit_report() const
        {
            _traits.reporter().begin_explicit_report();
        }

        void end_explicit_report(const std::string& description) const
        {
            _traits.reporter().end_explicit_report(description);
        }

    private:

        mutable TraitsT _traits;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HPP
