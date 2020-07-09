/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/reporting_adapter.hpp
*
*  Defines reporting adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/adapters/chained_adapter.hpp>
#include <dracosha/validator/reporting/reporter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  @brief Reporting adapter
 *
 *  Adaper first tries to perform validation and if validation fails the adapter then calls reporter to construct a report describing the failure.
 */
template <typename ReporterT, typename AdapterT>
class reporting_adapter : public chained_adapter<AdapterT>
{
    public:

        using reporter_type=ReporterT;

        /**
         * @brief Constructor
         * @param reporter Reporter to construct reports
         * @param next_adapter Next adapter in chain
         */
        reporting_adapter(
            ReporterT&& reporter,
            AdapterT&& next_adapter
        ) : chained_adapter<AdapterT>(std::forward<AdapterT>(next_adapter)),
            _reporter(std::forward<ReporterT>(reporter))
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
            auto ok=this->next_adapter().validate_operator(op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_operator(op,b);
            }
            return ok;
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
            auto ok=this->next_adapter().validate_property(prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_property(prop,op,b);
            }
            return ok;
        }

        /**
         *  @brief Validate existance of a member
         *  @param a Object to validate
         *  @param member Member descriptor
         *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
         *  @return Validation status
         */
        template <typename T2, typename MemberT>
        bool validate_exists(MemberT&& member, T2&& b)
        {
            auto ok=this->next_adapter().validate_exists(member,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_exists(member,b);
            }
            return ok;
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
        bool validate(MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=this->next_adapter().validate(member,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate(member,prop,op,b);
            }
            return ok;
        }

        /**
         *  @brief Validate using other member of the same object as a reference argument for validation
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Descriptor of sample member of the same object
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        bool validate_with_other_member(MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=this->next_adapter().validate(member,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_with_other_member(member,prop,op,b);
            }
            return ok;
        }

        /**
         *  @brief Validate using the same member of a Sample object
         *  @param member Member descriptor
         *  @param prop Property to validate
         *  @param op Operator for validation
         *  @param b Sample object whose member to use as argument passed to validation operator
         *  @return Validation status
         */
        template <typename T2, typename OpT, typename PropT, typename MemberT>
        bool validate_with_master_sample(MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=this->next_adapter().validate(member,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_with_master_sample(member,prop,op,b);
            }
            return ok;
        }

        template <typename ObjT, typename OpsT>
        bool validate_and(ObjT&& obj, OpsT&& ops,
                                 std::enable_if_t<!hana::is_a<member_tag,ObjT>,void*> =nullptr)
        {
            return aggregate(
                            string_and,
                            [this,&obj,&ops](){return this->next_adapter().validate_and(std::forward<ObjT>(obj),std::forward<OpsT>(ops));}
                        );
        }

        /**
         * @brief Execute validators on object and aggregate their results using logical AND
         * @param ops List of intermediate validators or validation operators
         * @return Logical AND of results of intermediate validators
         */
        template <typename OpsT>
        bool validate_and(OpsT&& ops)
        {
            return validate_and(*this,std::forward<OpsT>(ops));
        }

        template <typename ObjT, typename MemberT, typename OpsT>
        bool validate_and(ObjT&& obj, MemberT&& member, OpsT&& ops)
        {
            return aggregate(
                            string_and,
                            [this,&obj,&member,&ops](){return this->next_adapter().validate_and(std::forward<ObjT>(obj),member,std::forward<OpsT>(ops));},
                            member
                        );
        }

        /**
         * @brief Execute validators on object's member and aggregate their results using logical AND
         * @param member Member to process with validators
         * @param ops List of intermediate validators or validation operators
         * @return Logical AND of results of intermediate validators
         */
        template <typename MemberT, typename OpsT>
        bool validate_and(MemberT&& member,OpsT&& ops,
                          std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr)
        {
            return validate_and(*this,std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        template <typename ObjT, typename OpsT>
        bool validate_or(ObjT&& obj, OpsT&& ops,
                                 std::enable_if_t<!hana::is_a<member_tag,ObjT>,void*> =nullptr)
        {
            return aggregate(
                            string_or,
                            [this,&obj,&ops](){return this->next_adapter().validate_or(std::forward<ObjT>(obj),std::forward<OpsT>(ops));}
                        );
        }

        /**
         * @brief Execute validators on object and aggregate their results using logical OR
         * @param ops List of intermediate validators or validation operators
         * @return Logical OR of results of intermediate validators
         */
        template <typename OpsT>
        bool validate_or(OpsT&& ops)
        {
            return validate_or(*this,std::forward<OpsT>(ops));
        }

        template <typename ObjT,typename MemberT, typename OpsT>
        bool validate_or(ObjT&& obj, MemberT&& member, OpsT&& ops)
        {
            return aggregate(
                            string_or,
                            [this,&obj,&member,&ops](){return this->next_adapter().validate_or(std::forward<ObjT>(obj),member,std::forward<OpsT>(ops));},
                            member
                        );
        }

        /**
         * @brief Execute validators on object's member and aggregate their results using logical OR
         * @param member Member to process with validators
         * @param ops List of intermediate validators or validation operators
         * @return Logical OR of results of intermediate validators
         */
        template <typename MemberT, typename OpsT>
        bool validate_or(MemberT&& member, OpsT&& ops,
                         std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr)
        {
            return validate_or(*this,std::forward<MemberT>(member),std::forward<OpsT>(ops));
        }

        template <typename ObjT, typename OpT>
        bool validate_not(ObjT&& obj, OpT&& op,
                                 std::enable_if_t<!hana::is_a<member_tag,ObjT>,void*> =nullptr)
        {
            return aggregate(
                            string_not,
                            [this,&obj,&op](){return this->next_adapter().validate_not(std::forward<ObjT>(obj),std::forward<OpT>(op));}
                        );
        }

        /**
         * @brief Execute validator on object and negate the result
         * @param op Intermediate validator or validation operator
         * @return Logical NOT of results of intermediate validator
         */
        template <typename OpT>
        bool validate_not(OpT&& op)
        {
            return validate_not(*this,std::forward<OpT>(op));
        }

        template <typename ObjT, typename MemberT, typename OpT>
        bool validate_not(ObjT&& obj, MemberT&& member, OpT&& op)
        {
            return aggregate(
                            string_not,
                            [this,&obj,&member,&op](){return this->next_adapter().validate_not(std::forward<ObjT>(obj),member,std::forward<OpT>(op));},
                            member
                        );
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
        template <typename MemberT, typename OpT>
        bool validate_not(MemberT&& member, OpT&& op,
                          std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr)
        {
            return validate_not(*this,std::forward<MemberT>(member),std::forward<OpT>(op));
        }

    private:

        template <typename AgrregationT,typename HandlerT, typename ...Args>
        bool aggregate(AgrregationT&& aggregation,HandlerT&& handler,Args&&... args)
        {
            _reporter.aggregate_open(std::forward<AgrregationT>(aggregation),std::forward<Args>(args)...);
            auto ok=handler();
            _reporter.aggregate_close(ok);
            return ok;
        }

        ReporterT _reporter;
};

/**
 * @brief Create reporting adapter
 * @param reporter Reporter
 * @param next_adapter Next adapter in chain
 * @return Reporting adapter
 */
template <typename ReporterT, typename AdapterT>
auto make_reporting_adapter(ReporterT&& reporter, AdapterT&& next_adapter,
                            std::enable_if_t<
                                    (hana::is_a<reporter_tag,ReporterT> && hana::is_a<adapter_tag,AdapterT>),
                                    void*>
                            =nullptr)
{
    return reporting_adapter<ReporterT,AdapterT>{
                                std::forward<ReporterT>(reporter),
                                std::forward<AdapterT>(next_adapter)
                            };
}

/**
 * @brief Create reporting adapter using default adapter
 * @param reporter Reporter
 * @param obj Object to validate
 * @return Reporting adapter
 */
template <typename ReporterT, typename ObjT>
auto make_reporting_adapter(ReporterT&& reporter,
                            const ObjT& obj,
                            std::enable_if_t<
                                    (hana::is_a<reporter_tag,ReporterT> && !hana::is_a<adapter_tag,ObjT>),
                                    void*>
                            =nullptr)
{
    return make_reporting_adapter(std::forward<ReporterT>(reporter),make_adapter(obj));
}

/**
 * @brief Create reporting adapter using default reporter
 * @param dst Destination object where to put report
 * @param next_adapter Next adapter in chain
 * @return Reporting adapter
 */
template <typename DstT, typename AdapterT>
auto make_reporting_adapter(DstT& dst,
                            AdapterT&& next_adapter,
                            std::enable_if_t<
                                    (!hana::is_a<reporter_tag,DstT> && hana::is_a<adapter_tag,AdapterT>),
                                    void*>
                            =nullptr)
{
    return make_reporting_adapter(make_reporter(dst),std::forward<AdapterT>(next_adapter));
}

/**
 * @brief Create reporting adapter using default reporter and default adapter
 * @param dst Destination object where to put report
 * @param obj Object to validate
 * @return Reporting adapter
 */
template <typename DstT, typename ObjT>
auto make_reporting_adapter(DstT& dst,
                            const ObjT& obj,
                            std::enable_if_t<
                                    (!hana::is_a<reporter_tag,DstT> && !hana::is_a<adapter_tag,ObjT>),
                                    void*>
                            =nullptr)
{
    return make_reporting_adapter(make_reporter(dst),make_adapter(obj));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP
