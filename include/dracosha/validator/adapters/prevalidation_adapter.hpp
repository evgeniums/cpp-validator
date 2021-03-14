/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/prevalidation_adapter.hpp
*
*  Defines adapter for prevalidation of a single member with reporting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/value_as_container.hpp>
#include <dracosha/validator/adapter.hpp>
#include <dracosha/validator/reporting/reporter.hpp>
#include <dracosha/validator/reporting/reporting_adapter_impl.hpp>
#include <dracosha/validator/prevalidation/prevalidation_adapter_impl.hpp>
#include <dracosha/validator/prevalidation/prevalidation_adapter_tag.hpp>
#include <dracosha/validator/prevalidation/strict_any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits of prevalidation adapter.
 */
template <typename MemberT, typename T, typename ReporterT, typename WrappedT>
class prevalidation_adapter_traits : public adapter_traits,
                                     public object_wrapper<WrappedT>,
                                     public reporting_adapter_impl<ReporterT,prevalidation_adapter_impl<MemberT>>,
                                     public with_check_member_exists<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>
{
    public:

        using hana_tag=prevalidation_adapter_tag;

        using expand_aggregation_members=std::integral_constant<bool,false>;
        using filter_if_not_exists=std::integral_constant<bool,false>;

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        prevalidation_adapter_traits(
                    MemberT&& member,
                    T&& val,
                    ReporterT&& reporter
                ) : object_wrapper<WrappedT>(std::forward<T>(val)),
                    reporting_adapter_impl<ReporterT,prevalidation_adapter_impl<MemberT>>(
                        std::forward<ReporterT>(reporter),
                        std::forward<MemberT>(member)
                    ),
                    with_check_member_exists<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>(*this)
        {
            this->set_check_member_exists_before_validation(true);
        }

        template <typename PredicateT, typename AdapterT, typename OpsT, typename MemberT1>
        static status validate_member_aggregation(const PredicateT& pred, AdapterT&& adapter, MemberT1&& member, OpsT&& ops)
        {
            return prevalidation_adapter_impl<MemberT>::validate_member_aggregation(
                            pred,
                            std::forward<AdapterT>(adapter),
                            std::forward<MemberT1>(member),
                            std::forward<OpsT>(ops)
                        );
        }
};

/**
 * @brief Prevalidation adapter to validate single member before update.
 */
template <typename MemberT, typename T, typename ReporterT, typename WrappedT=T>
class prevalidation_adapter : public adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>
{
    public:

        using reporter_type=ReporterT;

        using adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>::adapter;
};


template <typename MemberT, typename T, typename ReporterT>
auto make_prevalidation_adapter_impl(
                            MemberT&& member,
                            T&& val,
                            ReporterT&& reporter)
{
    using value_type=decltype(adjust_view_type(std::forward<T>(val)));
    return prevalidation_adapter<MemberT,value_type,ReporterT>(std::forward<MemberT>(member),adjust_view_type(std::forward<T>(val)),std::forward<ReporterT>(reporter));
}

/**
 * @brief Create prevalidation adapter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param reporter Reporter to use for report construction if validation fails.
 */
template <typename MemberT, typename T, typename ReporterT>
auto make_prevalidation_adapter(
                            MemberT&& member,
                            T&& val,
                            ReporterT&& reporter,
                            std::enable_if_t<
                                    hana::is_a<reporter_tag,ReporterT> && !hana::is_a<strict_any_tag,T>,
                                    void*>
                            =nullptr)
{
    using key_type=decltype(member.key());
    using value_type=unwrap_object_t<T>;

    return hana::eval_if(
        hana::and_(
           hana::is_a<range_tag,value_type>,
           !hana::is_a<element_aggregation_tag,unwrap_object_t<key_type>>
        ),
        [&](auto&&)
        {
            return make_prevalidation_adapter_impl(member[ALL],std::forward<T>(val),std::forward<ReporterT>(reporter));
        },
        [&](auto&&)
        {
            return make_prevalidation_adapter_impl(std::forward<MemberT>(member),std::forward<T>(val),std::forward<ReporterT>(reporter));
        }
    );
}

/**
 * @brief Create prevalidation adapter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param reporter Reporter to use for report construction if validation fails.
 */
template <typename MemberT, typename T, typename ReporterT>
auto make_prevalidation_adapter(
                            MemberT&& member,
                            T&& val,
                            ReporterT&& reporter,
                            std::enable_if_t<
                                    hana::is_a<reporter_tag,ReporterT> && hana::is_a<strict_any_tag,T>,
                                    void*>
                            =nullptr)
{
    auto adapter=make_prevalidation_adapter(std::forward<MemberT>(member),val.to_wrapper(),std::forward<ReporterT>(reporter));
    traits_of(adapter).next_adapter_impl().set_strict_any(true);
    return adapter;
}

/**
 * @brief Create prevalidation adapter with default reporter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param dst Destination object where to put validation report if validation fails.
 */
template <typename MemberT, typename T, typename DstT>
auto make_prevalidation_adapter(
                            MemberT&& member,
                            T&& val,
                            DstT& dst,
                            std::enable_if_t<
                                    !hana::is_a<reporter_tag,DstT>,
                                    void*>
                            =nullptr)
{
    return make_prevalidation_adapter(std::forward<MemberT>(member),std::forward<T>(val),make_reporter(dst));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
