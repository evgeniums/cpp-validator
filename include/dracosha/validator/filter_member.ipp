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
#include <dracosha/validator/utils/make_object_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

template <typename KeyT, typename Enable>
template <typename PathT, typename AdapterT, typename HandlerT>
status generate_paths_t<KeyT,Enable>::operator() (PathT&& path, AdapterT&&, HandlerT&& handler) const
{
    return handler(std::forward<PathT>(path));
}

template <typename PathT, typename AdapterT, typename MemberT, typename HandlerT>
status apply_generated_paths_t::operator () (PathT&& current_path, AdapterT&& adapter, MemberT&& member, HandlerT&& handler) const
{
    return hana::eval_if(
        hana::greater_equal(hana::size(current_path),hana::size(member.path())),
        [&](auto&& _)
        {
            return generate_paths<std::decay_t<decltype(hana::back(_(current_path)))>>(_(current_path),_(adapter),_(handler));
        },
        [&](auto&& _)
        {
            auto&& key=make_object_wrapper_ref(hana::at(_(member).path(),hana::size(_(current_path))));
            return generate_paths<std::decay_t<decltype(key)>>(
                        hana::append(_(current_path),std::move(key)),
                        _(adapter),
                        _(handler)
                      );
        }
    );
}

template <typename PathT, typename FnT, typename AdapterT, typename MemberT>
status apply_member_path_t::operator ()
    (PathT&& current_path, FnT&& fn, AdapterT&& adapter, MemberT&& member) const
{
    return apply_generated_paths(
                std::forward<PathT>(current_path),
                std::forward<AdapterT>(adapter),
                std::forward<MemberT>(member),
                [&fn,&adapter,&member](auto&& generated_path)
                {
                    if (filter_path(adapter,generated_path))
                    {
                        return status(status::code::ignore);
                    }

                    return hana::if_(
                        hana::greater_equal(hana::size(generated_path),hana::size(member.path())),
                        [](auto&& generated_path, auto&& fn, auto&& adapter, auto&& member)
                        {
                            return status(fn(adapter,inherit_member(std::forward<decltype(generated_path)>(generated_path),std::forward<decltype(member)>(member))));
                        },
                        [](auto&& generated_path, auto&& fn, auto&& adapter, auto&& member)
                        {
                            return apply_member_path(
                                                std::forward<decltype(generated_path)>(generated_path),
                                                std::forward<decltype(fn)>(fn),
                                                std::forward<decltype(adapter)>(adapter),
                                                std::forward<decltype(member)>(member)
                                              );
                        }
                    )(
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
