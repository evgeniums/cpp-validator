/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/apply_generated_paths.hpp
*
*  Defines "apply_generated_paths".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_APPLY_GENERATED_PATHS_HPP
#define DRACOSHA_VALIDATOR_APPLY_GENERATED_PATHS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementer of apply_generated_paths.
 */
struct apply_generated_paths_impl
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename MemberT, typename HandlerT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& current_path, AdapterT&&, MemberT&& member, HandlerT&& handler) const;
};
/**
 * @brief Apply validation handler to generated member path.
 * @param used_path_size Length of already used member's path prefix.
 * @param current_path Current path to apply handler to.
 * @param adapter Validation adapter.
 * @param member Member.
 * @param handler Validation handler.
 * @return Status of validation.
 */
constexpr apply_generated_paths_impl apply_generated_paths{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_GENERATED_PATHS_HPP
