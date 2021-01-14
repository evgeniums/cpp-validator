/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/single_member_adapter.hpp
*
*  Defines adapter for single member validation with reporting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/reporting/reporter.hpp>
#include <dracosha/validator/adapters/impl/reporting_adapter_impl.hpp>
#include <dracosha/validator/adapters/impl/single_member_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits of reporting adapter that validates only single member.
 */
template <typename MemberT, typename T, typename ReporterT>
class single_member_adapter_traits : public object_wrapper<T>,
                                     public reporting_adapter_impl<ReporterT,single_member_adapter_impl<MemberT>>
{
    public:

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        single_member_adapter_traits(
                    adapter<single_member_adapter_traits<MemberT,T,ReporterT>>&,
                    MemberT&& member,
                    T&& val,
                    ReporterT&& reporter
                ) : object_wrapper<T>(std::forward<T>(val)),
                    reporting_adapter_impl<ReporterT,single_member_adapter_impl<MemberT>>(
                        std::forward<ReporterT>(reporter),
                        std::forward<MemberT>(member)
                    )
        {}
};

/**
 * @brief Reporting adapter that validates only single member.
 */
template <typename MemberT, typename T, typename ReporterT>
class single_member_adapter : public adapter<single_member_adapter_traits<MemberT,T,ReporterT>>
{
    public:

        using reporter_type=ReporterT;

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        single_member_adapter(
                MemberT&& member,
                T&& val,
                ReporterT&& reporter
            ) : adapter<single_member_adapter_traits<MemberT,T,ReporterT>>(std::forward<MemberT>(member),std::forward<T>(val),std::forward<ReporterT>(reporter))
        {}
};

/**
 * @brief Create reporting adapter that validates only single member.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param reporter Reporter to use for report construction if validation fails.
 */
template <typename MemberT, typename T, typename ReporterT>
auto make_single_member_adapter(
                            MemberT&& member,
                            T&& val,
                            ReporterT&& reporter,
                            std::enable_if_t<
                                    hana::is_a<reporter_tag,ReporterT>,
                                    void*>
                            =nullptr)
{
    return single_member_adapter<MemberT,T,ReporterT>(std::forward<MemberT>(member),adjust_view_type(std::forward<T>(val)),std::forward<ReporterT>(reporter));
}

/**
 * @brief Create reporting adapter that validates only single member and uses default reporter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param dst Destination object where to put validation report if validation fails.
 */
template <typename MemberT, typename T, typename DstT>
auto make_single_member_adapter(
                            MemberT&& member,
                            T&& val,
                            DstT& dst,
                            std::enable_if_t<
                                    !hana::is_a<reporter_tag,DstT>,
                                    void*>
                            =nullptr)
{
    return make_single_member_adapter(std::forward<MemberT>(member),adjust_view_type(std::forward<T>(val)),make_reporter(dst));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SINGLE_MEMBER_ADAPTER_HPP
