/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/details/reporting_adapter_impl.hpp
*
*  Defines implementation of reporting adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REPORTING_ADAPTER_IMPL_HPP
#define DRACOSHA_VALIDATOR_REPORTING_ADAPTER_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/default_adapter_impl.hpp>
#include <dracosha/validator/operators/any.hpp>
#include <dracosha/validator/operators/all.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Implementation of reporting adapter
 */
template <typename ReporterT, typename NextAdapterImplT>
class reporting_adapter_impl
{
    public:

        using reporter_type=ReporterT;

        template <typename ...Args>
        reporting_adapter_impl(
            ReporterT&& reporter,
            Args&&... args
        ) : _reporter(std::forward<ReporterT>(reporter)),
            _next_adapter_impl(std::forward<Args>(args)...)
        {}

        reporting_adapter_impl(
            ReporterT&& reporter
        ) : _reporter(std::forward<ReporterT>(reporter))
        {}

        template <typename AdapterT, typename T2, typename OpT>
        status validate_operator(AdapterT&& adpt, OpT&& op, T2&& b)
        {
            auto ok=_next_adapter_impl.validate_operator(adpt,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_operator(op,b);
            }
            return ok;
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT>
        status validate_property(AdapterT&& adpt, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=_next_adapter_impl.validate_property(adpt,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_property(prop,op,b);
            }
            return ok;
        }

        template <typename AdapterT, typename T2, typename MemberT>
        status validate_exists(AdapterT&& adpt, MemberT&& member, T2&& b, bool from_check_member)
        {
            auto ok=_next_adapter_impl.validate_exists(adpt,member,b,from_check_member);
            if (!from_check_member && (!ok || _reporter.current_not()))
            {
                _reporter.validate_exists(member,b);
            }
            return ok;
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
        status validate(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=_next_adapter_impl.validate(adpt,member,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate(member,prop,op,b);
            }
            return ok;
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_other_member(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=_next_adapter_impl.validate_with_other_member(adpt,member,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_with_other_member(member,prop,op,b);
            }
            return ok;
        }

        template <typename AdapterT, typename T2, typename OpT, typename PropT, typename MemberT>
        status validate_with_master_sample(AdapterT&& adpt, MemberT&& member, PropT&& prop, OpT&& op, T2&& b)
        {
            auto ok=_next_adapter_impl.validate_with_master_sample(adpt,member,prop,op,b);
            if (!ok || _reporter.current_not())
            {
                _reporter.validate_with_master_sample(member,prop,op,b);
            }
            return ok;
        }

        template <typename AdapterT, typename OpsT>
        status validate_and(AdapterT&& adpt, OpsT&& ops)
        {
            return aggregate(
                            string_and,
                            [this,&adpt,&ops](){return _next_adapter_impl.validate_and(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));}
                        );
        }

        template <typename AdapterT, typename MemberT, typename OpsT>
        status validate_and(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
        {
            return aggregate(
                            string_and,
                            [this,&adpt,&member,&ops](){return _next_adapter_impl.validate_and(std::forward<AdapterT>(adpt),member,std::forward<OpsT>(ops));},
                            member
                        );
        }

        template <typename AdapterT, typename OpsT>
        status validate_or(AdapterT&& adpt, OpsT&& ops)
        {
            return aggregate(
                            string_or,
                            [this,&adpt,&ops](){return _next_adapter_impl.validate_or(std::forward<AdapterT>(adpt),std::forward<OpsT>(ops));}
                        );
        }

        template <typename AdapterT, typename MemberT, typename OpsT>
        status validate_or(AdapterT&& adpt, MemberT&& member, OpsT&& ops)
        {
            return aggregate(
                            string_or,
                            [this,&adpt,&member,&ops](){return _next_adapter_impl.validate_or(std::forward<AdapterT>(adpt),member,std::forward<OpsT>(ops));},
                            member
                        );
        }

        template <typename AdapterT, typename OpT>
        status validate_not(AdapterT&& adpt, OpT&& op)
        {
            return aggregate(
                            string_not,
                            [this,&adpt,&op](){return _next_adapter_impl.validate_not(std::forward<AdapterT>(adpt),std::forward<OpT>(op));}
                        );
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_not(AdapterT&& adpt, MemberT&& member, OpT&& op)
        {
            return aggregate(
                            string_not,
                            [this,&adpt,&member,&op](){return _next_adapter_impl.validate_not(std::forward<AdapterT>(adpt),member,std::forward<OpT>(op));},
                            member
                        );
        }

        template <typename AdapterT, typename OpT>
        status validate_any(AdapterT&& adpt, OpT&& op)
        {
            return aggregate(
                            string_any,
                            [this,&adpt,&op](){return _next_adapter_impl.validate_any(std::forward<AdapterT>(adpt),std::forward<OpT>(op));}
                        );
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_any(AdapterT&& adpt, MemberT&& member, OpT&& op)
        {
            return aggregate(
                            string_any,
                            [this,&adpt,&member,&op](){return _next_adapter_impl.validate_any(std::forward<AdapterT>(adpt),member,std::forward<OpT>(op));},
                            member
                        );
        }

        template <typename AdapterT, typename OpT>
        status validate_all(AdapterT&& adpt, OpT&& op)
        {
            return aggregate(
                            string_all,
                            [this,&adpt,&op](){return _next_adapter_impl.validate_all(std::forward<AdapterT>(adpt),std::forward<OpT>(op));}
                        );
        }

        template <typename AdapterT, typename MemberT, typename OpT>
        status validate_all(AdapterT&& adpt, MemberT&& member, OpT&& op)
        {
            return aggregate(
                            string_all,
                            [this,&adpt,&member,&op](){return _next_adapter_impl.validate_all(std::forward<AdapterT>(adpt),member,std::forward<OpT>(op));},
                            member
                        );
        }

    private:

        template <typename AgrregationT,typename HandlerT, typename ...Args>
        status aggregate(AgrregationT&& aggregation,HandlerT&& handler,Args&&... args)
        {
            _reporter.aggregate_open(std::forward<AgrregationT>(aggregation),std::forward<Args>(args)...);
            auto st=handler();
            _reporter.aggregate_close(st);
            return st;
        }

        ReporterT _reporter;
        NextAdapterImplT _next_adapter_impl;
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTING_ADAPTER_IMPL_HPP
