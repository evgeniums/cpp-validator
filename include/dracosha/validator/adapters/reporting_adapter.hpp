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
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/reporting/reporter.hpp>
#include <dracosha/validator/detail/reporting_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits of reporting adapter
 */
template <typename T,typename ReporterT>
struct reporting_adapter_traits : public with_check_member_exists<adapter<reporting_adapter_traits<T,ReporterT>>>,
                                  public reference_wrapper_t<const T>,
                                  public detail::reporting_adapter_impl<ReporterT,detail::default_adapter_impl>
{
    public:

        /**
         * @brief Constructor
         * @param adpt Adapter
         * @param obj Constant reference to object under validation
         * @param reporter_args Parameters to forward to reporter's constructor
         */
        template <typename ... ReporterArgs>
        reporting_adapter_traits(
                    adapter<reporting_adapter_traits<T,ReporterT>>& adpt,
                    const T& obj,
                    ReporterArgs&&... reporter_args
                )
            : with_check_member_exists<adapter<reporting_adapter_traits<T,ReporterT>>>(adpt),
              reference_wrapper_t<const T>(obj),
              detail::reporting_adapter_impl<ReporterT,detail::default_adapter_impl>(std::forward<ReporterArgs>(reporter_args)...)
        {}
};

/**
 * @brief Reporting adapter that constructs report if validation by default adapter fails.
 */
template <typename T, typename ReporterT>
class reporting_adapter : public adapter<reporting_adapter_traits<T,ReporterT>>,
                          public check_member_exists_traits_proxy<reporting_adapter<T,ReporterT>>
{
    public:

        using reporter_type=ReporterT;

        /**
         * @brief Constructor
         * @param obj Constant reference to object under validation
         * @param reporter_args Parameters to forward to reporter's constructor
         */
        template <typename ... ReporterArgs>
        reporting_adapter(
                const T& obj,
                ReporterArgs&&... reporter_args
            )
            : adapter<reporting_adapter_traits<T,ReporterT>>(obj,std::forward<ReporterArgs>(reporter_args)...),
              check_member_exists_traits_proxy<reporting_adapter<T,ReporterT>>(*this)
        {}
};

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
                                    hana::is_a<reporter_tag,ReporterT>,
                                    void*>
                            =nullptr)
{
    return reporting_adapter<ObjT,ReporterT>(obj,std::forward<ReporterT>(reporter));
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
    return make_reporting_adapter(make_reporter(dst),obj);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REPORTING_ADAPTER_HPP
