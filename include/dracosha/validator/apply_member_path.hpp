/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/apply_member_path.hpp
*
*  Defines "apply_member_path".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_APPLY_MEMBER_PATH_HPP
#define DRACOSHA_VALIDATOR_APPLY_MEMBER_PATH_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementer of apply_member_path.
 */
struct apply_member_path_impl
{
    template <typename UsedPathSizeT, typename PathT, typename FnT, typename AdapterT, typename MemberT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& current_path, FnT&& fn, AdapterT&& adapter, MemberT&& member) const;
};
/**
 * @brief Apply validation handler to member path.
 * @param used_path_size Length of already used member's path prefix.
 * @param current_path Current path to apply handler to.
 * @param fn Validation handler.
 * @param adapter Validation adapter.
 * @param member Member.
 * @return Status of validation.
 *
 * Member path might be not the final actual path the validation is applied to.
 * If there are aggregations in path then actual paths would be generated and processed in apply_generated_paths().
 */
constexpr apply_member_path_impl apply_member_path{};


//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_MEMBER_PATH_HPP
