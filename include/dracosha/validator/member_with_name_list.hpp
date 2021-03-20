/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/member_with_name_list.hpp
*
*  Defines member_with_name_list.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_WITH_NAME_LIST_HPP
#define DRACOSHA_VALIDATOR_MEMBER_WITH_NAME_LIST_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/member_with_name.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct member_with_name_list_tag{};

template <typename T1, typename T2, typename ...ParentPathT>
struct member_with_name_list : public member_with_name<T1,T2,ParentPathT...>,
                               public member_with_name_list_tag
{
    using member_with_name<T1,T2,ParentPathT...>::member_with_name;
};

template <typename MemberT, typename T>
auto make_member_with_name_list(MemberT&& member, T&& name_list)
{
    return member.template create_derived<member_with_name_list>(std::forward<T>(name_list),std::move(member._path));
}

template <typename T, typename = hana::when<true>>
struct member_name_list_t
{
    template <typename T1>
    auto operator() (T1&& v) const
    {
        return v.path();
    }
};
template <typename T>
struct member_name_list_t<T,
                hana::when<std::is_base_of<member_with_name_list_tag,std::decay_t<T>>::value>
            >
{
    template <typename T1>
    auto operator() (T1&& v) const
    {
        return v.name();
    }
};
template <typename T>
struct member_name_list_t<T,
                hana::when<
                    std::is_base_of<member_with_name_tag,std::decay_t<T>>::value
                    &&
                    !std::is_base_of<member_with_name_list_tag,std::decay_t<T>>::value
                >
            >
{
    template <typename T1>
    auto operator() (T1&& v) const
    {
        return hana::make_tuple(v.name());
    }
};
template <typename T>
constexpr member_name_list_t<T> member_name_list_inst{};

struct member_name_list_impl
{
    template <typename T>
    auto operator() (T&& v) const
    {
        return member_name_list_inst<T>(std::forward<T>(v));
    }
};
constexpr member_name_list_impl member_name_list{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_WITH_NAME_LIST_HPP
