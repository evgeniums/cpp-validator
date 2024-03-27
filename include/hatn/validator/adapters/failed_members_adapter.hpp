/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/adapters/failed_members_adapter.hpp
*
*  Defines adapter that collects all failed members.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_FAILED_MEMBERS_ADAPTER_HPP
#define HATN_VALIDATOR_FAILED_MEMBERS_ADAPTER_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/adapters/reporting_adapter.hpp>
#include <hatn/validator/reporting/failed_members_reporter.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Create adapter that collects all failed members.
 * @param obj Object to validate.
 * @return Adapter.
 *
 * The list of failed member names can be accessed via adapter.traits().reporter().failed_members() after validation.
 *
 * @note Note 1. When validation with this adapter fails then validator.apply() returns either status::ignore or status::success, i.e. its result is useless.
 * In order to figure out actual validation result check if the list of failed members is empty. If it is empty then validation succeeded, if it is not empty then validation failed.
 *
 * @note Note 2. Use with care with validators that check member existence before checking value - if the member does not exist then
 * undefined behaviour is expected, e.g. an exception can be thrown or even the application crashes.
 *
 * @note Note 3. If validator is too complicated, e.g. it includes NOT aggregation or some other nested conditions, then failed member collecting
 * might work not properly - some members might be missing while some other members might be excessive.
 *
 */
template <typename ObjT>
auto make_failed_members_adapter(ObjT&& obj)
{
    return reporting_adapter<ObjT,failed_members_reporter,hana::true_>(std::forward<ObjT>(obj),failed_members_reporter{});
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_FAILED_MEMBERS_ADAPTER_HPP
