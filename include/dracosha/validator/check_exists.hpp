/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operator.hpp
*
*  Defines base operator classes
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP
#define DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/get.hpp>
#include <dracosha/validator/contains.hpp>
#include <dracosha/validator/check_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename T>
constexpr auto take_address_of(T&& t) -> decltype(&t)
{
    return &t;
}

BOOST_HANA_CONSTEXPR_LAMBDA auto iterate_exists =[](auto&& obj,auto&& key)
{
    if (obj && contains(*obj,key))
    {
        return take_address_of(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key)));
    }
    return decltype(take_address_of(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key))))(nullptr);
};
}

BOOST_HANA_CONSTEXPR_LAMBDA auto check_exists =[](auto&& obj,auto&& path)
{
    return hana::fold(std::forward<decltype(path)>(path),&obj,detail::iterate_exists)!=nullptr;
};

BOOST_HANA_CONSTEXPR_LAMBDA auto check_exists_static =[](auto obj,auto path)
{
    auto path_c=hana::transform(path,hana::make_type);
    auto obj_c=hana::type_c<decltype(obj)>;

    return hana::is_nothing(hana::monadic_fold_left<hana::optional_tag>(path_c,obj_c,hana::sfinae(check_member)));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP
