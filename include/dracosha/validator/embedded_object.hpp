/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/embedded_object.hpp
*
*  Defines helper to extract embedded object from adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EMBEDDED_OBJECT_HPP
#define DRACOSHA_VALIDATOR_EMBEDDED_OBJECT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct embedded_object_impl
{
    template <typename AdapterT>
    auto operator() (const AdapterT& adapter) const -> decltype(auto)
    {
        return extract(adapter.traits().get());
    }
};
constexpr embedded_object_impl embedded_object;

struct original_embedded_object_impl
{
    template <typename AdapterT>
    auto operator() (const AdapterT& adapter) const -> decltype(auto)
    {
        return extract(unwrap_object(adapter.traits()));
    }
};
constexpr original_embedded_object_impl original_embedded_object;

struct embedded_object_has_member_impl
{
    template <typename AdapterT, typename MemberT>
    auto operator() (const AdapterT& adapter, const MemberT& member) const -> decltype(auto)
    {
        return adapter.traits().check_member_exists(member);
    }
};
constexpr embedded_object_has_member_impl embedded_object_has_member;

struct original_embedded_object_has_member_impl
{
    template <typename AdapterT, typename MemberT>
    auto operator() (const AdapterT& adapter, const MemberT& member) const -> decltype(auto)
    {
        return adapter.traits().check_member_exists(member);
    }
};
constexpr original_embedded_object_has_member_impl original_embedded_object_has_member;

struct embedded_object_member_impl
{
    template <typename AdapterT, typename MemberT>
    auto operator() (const AdapterT& adapter, const MemberT& member) const -> decltype(auto)
    {
        return get_member(embedded_object(adapter),member.path());
    }
};
constexpr embedded_object_member_impl embedded_object_member;

struct original_embedded_object_member_impl
{
    template <typename AdapterT, typename MemberT>
    auto operator() (const AdapterT& adapter, const MemberT& member) const -> decltype(auto)
    {
        return get_member(original_embedded_object(adapter),member.path());
    }
};
constexpr original_embedded_object_member_impl original_embedded_object_member;

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EMBEDDED_OBJECT_HPP
