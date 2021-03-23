/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/filer_member.hpp
*
*  Defines "filter_member".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP
#define DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/filter_path.hpp>
#include <dracosha/validator/embedded_object.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/generate_paths.hpp>
#include <dracosha/validator/apply_generated_paths.hpp>
#include <dracosha/validator/apply_member_path.hpp>
#include <dracosha/validator/invoke_member_if_exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default invoker of member filtering.
 *
 * This implementation processes member in 3 steps:
 *  1. check if member's path must be filtered;
 *  2. if not filtered then check if member exists or might exist;
 *  3. if esists or might exist then invoke validation handler.
 */
template <typename AdapterT, typename MemberT, typename Enable=hana::when<true>>
struct filter_member_invoker
{
    template <typename FnT, typename AdapterT1, typename MemberT1>
    static status invoke(FnT&& fn, AdapterT1&& adapter, MemberT1&& member)
    {
        if (filter_path(adapter,member.path()))
        {
            return status::code::ignore;
        }        
        return invoke_member_if_exists(fn,adapter,member);
    }
};

/**
 * @brief Invoker of member filtering in case of aggregated members and adapters with expand_aggregation_members set to integral constant true.
 *
 * This implementation forwards member processing to paths generating.
 * For each generated path it checks if that path must be filtered and if not then invoke
 * validation handler if member exists or might exist.
 */
template <typename AdapterT, typename MemberT>
struct filter_member_invoker<AdapterT,MemberT,
            hana::when<
                MemberT::is_aggregated::value
                &&
                AdapterT::expand_aggregation_members::value
            >
        >
{
    template <typename FnT, typename AdapterT1, typename MemberT1>
    static auto invoke(FnT&& fn, AdapterT1&& adapter, MemberT1&& member)
    {
        auto handler=[&fn](auto&& adapter, auto&& member)
        {
            return invoke_member_if_exists(fn,adapter,member);
        };
        return apply_member_path(hana::size_c<0>,hana::tuple<>{},handler,adapter,member);
    }
};

//-------------------------------------------------------------

/**
 * @brief Implementer of filter_member().
 */
struct filter_member_impl
{
    template <typename FnT, typename AdapterT, typename MemberT>
    auto operator () (FnT&& fn, AdapterT&& adapter, MemberT&& member) const
    {
        return filter_member_invoker<std::decay_t<AdapterT>,std::decay_t<MemberT>>
                    ::invoke(std::forward<FnT>(fn),std::forward<AdapterT>(adapter),std::forward<MemberT>(member));
    }
};
/**
 * @brief Filter a member before validation.
 * @param fn Validation handler.
 * @param adapter Validation adapter.
 * @param member Member.
 * @return If member is filtered status::code::ignore, otherwise result of validation handler.
 */
constexpr filter_member_impl filter_member{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP
