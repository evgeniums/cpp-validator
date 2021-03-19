/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/filer_member.ipp
*
*  Defines "filter_member".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FILTER_MEMBER_IPP
#define DRACOSHA_VALIDATOR_FILTER_MEMBER_IPP

#include <dracosha/validator/config.hpp>

#include <dracosha/validator/utils/hana_to_std_tuple.hpp>
#include <dracosha/validator/filter_member.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/utils/wrap_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

template <typename KeyT, typename Enable>
template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
status generate_paths_t<KeyT,Enable>::operator() (UsedPathSizeT&&, PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
{
    return handler(std::forward<AdapterT>(adapter),std::forward<PathT>(path),hana::size(path));
}

template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename MemberT, typename HandlerT>
status apply_generated_paths_t::operator () (UsedPathSizeT&& used_path_size, PathT&& current_path, AdapterT&& adapter, MemberT&& member, HandlerT&& handler) const
{
    return hana::eval_if(
        hana::greater_equal(used_path_size,hana::size(member.path())),
        [&](auto&& _)
        {
            return generate_paths<std::decay_t<decltype(hana::back(_(current_path)))>>(_(used_path_size),_(current_path),_(adapter),_(handler));
        },
        [&](auto&& _)
        {
            auto&& key=wrap_object_ref(hana::at(_(member).path(),_(used_path_size)));
            return generate_paths<std::decay_t<decltype(key)>>(
                        hana::plus(_(used_path_size),hana::size_c<1>),
                        hana::append(_(current_path),std::move(key)),
                        _(adapter),
                        _(handler)
                      );
        }
    );
}

template <typename UsedPathSizeT, typename PathT, typename FnT, typename AdapterT, typename MemberT>
status apply_member_path_t::operator ()
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

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FILTER_MEMBER_HPP
