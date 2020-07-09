/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/single_member_adapter.hpp
*
*  Defines adapter for single member validation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/adapters/chained_adapter.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  @brief Adapter for single member validation
 *
 */
template <typename MemberT, typename AdapterT>
class single_member_adapter : public chained_adapter<AdapterT>
{
    public:

        using member_type=MemberT;

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param next_adapter Next adapter in chain
         */
        single_member_adapter(
            MemberT&& member,
            AdapterT&& next_adapter
        ) : chained_adapter<AdapterT>(std::forward<AdapterT>(next_adapter)),
            _member(std::forward<MemberT>(member))
        {}

        /**
         *  @brief Perform validation of object at one level without member nesting
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         *  @return Validation status
         */
        template <typename T2, typename OpT>
        bool validate_operator(OpT&& op, T2&& b)
        {
            return this->next_adapter().validate_operator(op,b);
        }

        /**
         *  @brief Perform validation of object's property at one level without member nesting
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT>
        bool validate_property(PropT&& prop, OpT&& op, T2&& b)
        {
            return this->next_adapter().validate_property(prop,op,b);
        }

        /**
         *  @brief Validate existance of a member
         *  @param a Object to validate
         *  @param member Member descriptor
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
         *  @return Validation status
         */
        template <typename T2, typename MemberT1>
        bool validate_exists(MemberT1&& member, T2&& b)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate_exists(member,b);
        }

        /**
         *  @brief Normal validation of a member
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample argument for validation
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT1>
        bool validate(MemberT1&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate(member,prop,op,b);
        }

        /**
         *  @brief Validate using other member of the same object as a reference argument for validation
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Descriptor of sample member of the same object
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT1>
        bool validate_with_other_member(MemberT1&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate(member,prop,op,b);
        }

        /**
         *  @brief Validate using the same member of a Sample object
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample object whose member to use as argument passed to validation operator
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT1>
        bool validate_with_master_sample(MemberT1&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate(member,prop,op,b);
        }

        /**
         * @brief Execute validators on object and aggregate their results using logical AND
         * @param ops List of intermediate validators or validation operators
         * @return Logical AND of results of intermediate validators
         */
        template <typename OpsT>
        bool validate_and(OpsT&& ops)
        {
            return this->next_adapter().validate_and(*this,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on object's member and aggregate their results using logical AND
         * @param member Member to process with validators
         * @param ops List of intermediate validators or validation operators
         * @return Logical AND of results of intermediate validators
         */
        template <typename OpsT, typename MemberT1>
        bool validate_and(MemberT1&& member, OpsT&& ops)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate_and(*this,member,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on object and aggregate their results using logical OR
         * @param ops List of intermediate validators or validation operators
         * @return Logical OR of results of intermediate validators
         */
        template <typename OpsT>
        bool validate_or(OpsT&& ops)
        {
            return this->next_adapter().validate_or(*this,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validators on object's member and aggregate their results using logical OR
         * @param member Member to process with validators
         * @param ops List of intermediate validators or validation operators
         * @return Logical OR of results of intermediate validators
         */
        template <typename OpsT, typename MemberT1>
        bool validate_or(MemberT1&& member, OpsT&& ops)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate_or(*this,member,std::forward<OpsT>(ops));
        }

        /**
         * @brief Execute validator on object and negate the result
         * @param op Intermediate validator or validation operator
         * @return Logical NOT of results of intermediate validator
         */
        template <typename OpT>
        bool validate_not(OpT&& op)
        {
            return this->next_adapter().validate_not(*this,std::forward<OpT>(op));
        }

        /**
         * @brief Execute validator on object's member and negate the result
         * @param member Member to process with validator
         * @param op Intermediate validator or validation operator
         * @return Logical NOT of results of intermediate validator
         *
         * @note The validator will report all matched conditions for nested AND operator
         *       but only the first matched condition for nested OR operator
         */
        template <typename OpT, typename MemberT1>
        bool validate_not(MemberT1&& member, OpT&& op)
        {
            if (!_member.isEqual(member))
            {
                return true;
            }
            return this->next_adapter().validate_not(*this,member,std::forward<OpT>(op));
        }

    private:

        MemberT _member;
};

/**
 * @brief Make adapter for single member validation
 * @param member Member to validate
 * @param adapter Actual adapter for validation
 * @return Adapter for validation of signle member
 */
template <typename MemberT, typename AdapterT>
auto make_single_member_adapter(MemberT&& member, AdapterT&& next_adapter,
                                std::enable_if_t<
                                        hana::is_a<adapter_tag,AdapterT>,
                                        void*>
                                =nullptr
                            )
{
    return single_member_adapter<MemberT,AdapterT>{
                                std::forward<MemberT>(member),
                                std::forward<AdapterT>(next_adapter)
                            };
}

/**
 * @brief Make adapter for single member validation
 * @param member Member to validate
 * @param obj Object under validation
 * @return Adapter for validation of signle member
 */
template <typename MemberT, typename ObjT>
auto make_single_member_adapter(MemberT&& member, ObjT&& obj,
                                std::enable_if_t<
                                        !hana::is_a<adapter_tag,ObjT>,
                                        void*>
                                =nullptr
                            )
{
    return make_single_member_adapter(
                std::forward<MemberT>(member),
                make_adapter(std::forward<ObjT>(obj))
        );
}

/**
 * @brief Make reporting adapter for single member validation
 * @param dst Destination object or reporter
 * @param member Member to validate
 * @param obj Object under validation
 * @return Adapter for validation of signle member
 */
template <typename DstT, typename MemberT, typename ObjT>
auto make_single_member_reporting_adapter(DstT&& dst, MemberT&& member, ObjT&& obj)
{
    return make_single_member_adapter(
                std::forward<MemberT>(member),
                make_reporting_adapter(std::forward<DstT>(dst),std::forward<ObjT>(obj))
        );
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_HPP
