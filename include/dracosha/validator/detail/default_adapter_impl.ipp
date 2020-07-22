/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/default_adapter_impl.ipp
*
*  Defines implementation of some helpers for default adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_IPP
#define DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_IPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/detail/default_adapter_impl.hpp>
#include <dracosha/validator/operators/all.hpp>
#include <dracosha/validator/operators/any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename T>
template <typename ContainerT, typename AdapterT, typename OpT>
status aggregate_impl<T,
        hana::when<is_container_t<T>::value>>
        ::all(ContainerT&& container, AdapterT&& adpt, OpT&& op)
{
    for (auto it=container.begin();it!=container.end();++it)
    {
        auto el_member=member<decltype(wrap_it(it,string_all))>(wrap_it(it,string_all));
        auto ret=apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),el_member);
        if (!ret)
        {
            return ret;
        }
    }
    return status::code::success;
}

template <typename T>
template <typename ContainerT, typename AdapterT, typename MemberT, typename OpT>
status aggregate_impl<T,
        hana::when<is_container_t<T>::value>>
        ::all(ContainerT&& container, AdapterT&& adpt, MemberT&& member, OpT&& op)
{
    for (auto it=container.begin();it!=container.end();++it)
    {
        auto el_member=member[wrap_it(it,string_all)];
        auto ret=apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),el_member);
        if (!ret)
        {
            return ret;
        }
    }
    return status::code::success;
}

template <typename T>
template <typename ContainerT, typename AdapterT, typename OpT>
status aggregate_impl<T,
        hana::when<is_container_t<T>::value>>
        ::any(ContainerT&& container, AdapterT&& adpt, OpT&& op)
{
    bool empty=true;
    for (auto it=container.begin();it!=container.end();++it)
    {
        auto el_member=member<decltype(wrap_it(it,string_any))>(wrap_it(it,string_any));
        auto ret=apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),el_member);
        if (ret)
        {
            return ret;
        }
        empty=false;
    }
    if (empty)
    {
        return status::code::success;
    }
    return status::code::fail;
}

template <typename T>
template <typename ContainerT, typename AdapterT, typename MemberT, typename OpT>
status aggregate_impl<T,
        hana::when<is_container_t<T>::value>>
        ::any(ContainerT&& container, AdapterT&& adpt, MemberT&& member, OpT&& op)
{
    bool empty=true;
    for (auto it=container.begin();it!=container.end();++it)
    {
        auto el_member=member[wrap_it(it,string_any)];
        auto ret=apply_member(std::forward<decltype(adpt)>(adpt),std::forward<decltype(op)>(op),el_member);
        if (ret)
        {
            return ret;
        }
        empty=false;
    }
    if (empty)
    {
        return status::code::success;
    }
    return status::code::fail;
}

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DEFAULT_ADAPTER_IMPL_IPP
