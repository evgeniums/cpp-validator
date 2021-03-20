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

struct apply_member_path_t
{
    template <typename UsedPathSizeT, typename PathT, typename FnT, typename AdapterT, typename MemberT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& current_path, FnT&& fn, AdapterT&& adapter, MemberT&& member) const;
};
constexpr apply_member_path_t apply_member_path{};


//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_APPLY_MEMBER_PATH_HPP
