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
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/reporting/reporter.hpp>
#include <dracosha/validator/adapters/impl/reporting_adapter_impl.hpp>
#include <dracosha/validator/prevalidation/prevalidation_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits of prevalidation adapter.
 */
template <typename MemberT, typename T, typename ReporterT>
class prevalidation_adapter_traits : public object_wrapper<T>,
                                     public reporting_adapter_impl<ReporterT,prevalidation_adapter_impl<MemberT>>
{
    public:

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        prevalidation_adapter_traits(
                    adapter<prevalidation_adapter_traits<MemberT,T,ReporterT>>&,
                    MemberT&& member,
                    T&& val,
                    ReporterT&& reporter
                ) : object_wrapper<T>(std::forward<T>(val)),
                    reporting_adapter_impl<ReporterT,prevalidation_adapter_impl<MemberT>>(
                        std::forward<ReporterT>(reporter),
                        std::forward<MemberT>(member)
                    )
        {}
};

/**
 * @brief Prevalidation adapter to validate single member before update.
 */
template <typename MemberT, typename T, typename ReporterT>
class prevalidation_adapter : public adapter<prevalidation_adapter_traits<MemberT,T,ReporterT>>
{
    public:

        using reporter_type=ReporterT;

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        prevalidation_adapter(
                MemberT&& member,
                T&& val,
                ReporterT&& reporter
            ) : adapter<prevalidation_adapter_traits<MemberT,T,ReporterT>>(std::forward<MemberT>(member),std::forward<T>(val),std::forward<ReporterT>(reporter))
        {}
};

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
                                    hana::is_a<reporter_tag,ReporterT>,
                                    void*>
                            =nullptr)
{
    return prevalidation_adapter<MemberT,T,ReporterT>(std::forward<MemberT>(member),adjust_view_type(std::forward<T>(val)),std::forward<ReporterT>(reporter));
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
    return make_prevalidation_adapter(std::forward<MemberT>(member),adjust_view_type(std::forward<T>(val)),make_reporter(dst));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
