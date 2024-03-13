/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/apply_generated_paths.ipp
*
*  Defines "apply_generated_paths".
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_FILTER_MEMBER_IPP
#define HATN_VALIDATOR_FILTER_MEMBER_IPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/utils/hana_to_std_tuple.hpp>
#include <hatn/validator/filter_member.hpp>
#include <hatn/validator/member.hpp>
#include <hatn/validator/utils/wrap_object.hpp>
#include <hatn/validator/apply_generated_paths.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename MemberT, typename HandlerT>
status apply_generated_paths_impl::operator () (UsedPathSizeT&& used_path_size, PathT&& current_path, AdapterT&& adapter, MemberT&& member, HandlerT&& handler) const
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

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_FILTER_MEMBER_HPP
