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

struct apply_generated_paths_t
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename MemberT, typename HandlerT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& current_path, AdapterT&&, MemberT&& member, HandlerT&& handler) const;
};
constexpr apply_generated_paths_t apply_generated_paths{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_GENERATED_PATHS_HPP
