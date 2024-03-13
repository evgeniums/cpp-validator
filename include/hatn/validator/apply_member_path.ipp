/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/apply_member_path.ipp
*
*  Defines "apply_member_path".
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_APPLY_MEMBER_PATH_IPP
#define HATN_VALIDATOR_APPLY_MEMBER_PATH_IPP

#include <hatn/validator/config.hpp>

#include <hatn/validator/apply_member_path.hpp>
#include <hatn/validator/utils/hana_to_std_tuple.hpp>
#include <hatn/validator/filter_member.hpp>
#include <hatn/validator/member.hpp>
#include <hatn/validator/utils/wrap_object.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

template <typename UsedPathSizeT, typename PathT, typename FnT, typename AdapterT, typename MemberT>
status apply_member_path_impl::operator ()
    (UsedPathSizeT&& used_path_size, PathT&& current_path, FnT&& fn, AdapterT&& adapter, MemberT&& member) const
{
    return apply_generated_paths(
                std::forward<UsedPathSizeT>(used_path_size),
                std::forward<PathT>(current_path),
                std::forward<AdapterT>(adapter),
                std::forward<MemberT>(member),
                [&fn,&member](auto&& adapter, auto&& generated_path, auto&& used_path_size)
                {
                    if (filter_path(adapter,generated_path))
                    {
                        return status(status::code::ignore);
                    }

                    return hana::if_(
                        hana::greater_equal(used_path_size,hana::size(member.path())),
                        [](auto&&, auto&& generated_path, auto&& fn, auto&& adapter, auto&& member)
                        {
                            return status(fn(std::forward<decltype(adapter)>(adapter),inherit_member(std::forward<decltype(generated_path)>(generated_path),std::forward<decltype(member)>(member))));
                        },
                        [](auto&& used_path_size, auto&& generated_path, auto&& fn, auto&& adapter, auto&& member)
                        {
                            return apply_member_path(
                                                std::forward<decltype(used_path_size)>(used_path_size),
                                                std::forward<decltype(generated_path)>(generated_path),
                                                std::forward<decltype(fn)>(fn),
                                                std::forward<decltype(adapter)>(adapter),
                                                std::forward<decltype(member)>(member)
                                              );
                        }
                    )(
                        std::forward<decltype(used_path_size)>(used_path_size),
                        std::forward<decltype(generated_path)>(generated_path),
                        std::forward<decltype(fn)>(fn),
                        std::forward<decltype(adapter)>(adapter),
                        std::forward<decltype(member)>(member)
                      );
                }
            );
}

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_APPLY_MEMBER_PATH_IPP
