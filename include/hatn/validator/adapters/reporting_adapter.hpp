/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/reporting_adapter.hpp
*
*  Defines reporting adapter.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_REPORTING_ADAPTER_HPP
#define HATN_VALIDATOR_REPORTING_ADAPTER_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/utils/object_wrapper.hpp>
#include <hatn/validator/adapter.hpp>
#include <hatn/validator/reporting/reporter.hpp>
#include <hatn/validator/reporting/reporting_adapter_impl.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits of reporting adapter.
 */
template <typename T, typename ReporterT, typename CollectAllFailedMembers=hana::false_>
struct reporting_adapter_traits : public adapter_traits,
                                  public object_wrapper<T>,
                                  public with_check_member_exists<reporting_adapter_traits<T,ReporterT>>,
                                  public reporting_adapter_impl<ReporterT,default_adapter_impl,CollectAllFailedMembers>
{
    public:

        /**
         * @brief Constructor.
         * @param adpt Adapter.
         * @param obj Constant reference to object under validation.
         * @param reporter_args Parameters to forward to reporter's constructor.
         */
        reporting_adapter_traits(
                    T&& obj,
                    ReporterT&& reporter
                )
            : object_wrapper<T>(std::forward<T>(obj)),
              with_check_member_exists<reporting_adapter_traits<T,ReporterT>>(*this),
              reporting_adapter_impl<ReporterT,default_adapter_impl,CollectAllFailedMembers>(std::forward<ReporterT>(reporter))
        {}
};

/**
 * @brief Reporting adapter that constructs report if validation fails.
 */
template <typename T, typename ReporterT, typename CollectAllFailedMembers=hana::false_>
class reporting_adapter : public adapter<reporting_adapter_traits<T,ReporterT,CollectAllFailedMembers>>
{
    public:

        using reporter_type=ReporterT;

        using adapter<reporting_adapter_traits<T,ReporterT,CollectAllFailedMembers>>::adapter;
};

/**
 * @brief Create reporting adapter based on default adapter.
 * @param reporter Reporter.
 * @param obj Object to validate.
 * @return Reporting adapter.
 */
template <typename ObjT, typename ReporterT>
auto make_reporting_adapter(ObjT&& obj,
                            ReporterT&& reporter,
                            std::enable_if_t<
                                    hana::is_a<reporter_tag,ReporterT>,
                                    void*>
                            =nullptr)
{
    return reporting_adapter<ObjT,ReporterT>(std::forward<ObjT>(obj),std::forward<ReporterT>(reporter));
}

/**
 * @brief Create reporting adapter based on default adapter and using default reporter.
 * @param dst Destination object where to put report.
 * @param obj Object to validate.
 * @return Reporting adapter.
 */
template <typename ObjT, typename DstT>
auto make_reporting_adapter(ObjT&& obj,
                            DstT& dst,
                            std::enable_if_t<
                                    (!hana::is_a<reporter_tag,DstT> && !hana::is_a<adapter_tag,ObjT>),
                                    void*>
                            =nullptr)
{
    return make_reporting_adapter(std::forward<ObjT>(obj),make_reporter(dst));
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_REPORTING_ADAPTER_HPP
