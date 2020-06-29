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

#ifdef _MSC_VER
// disable warning about taking address of temporary variable 
// because take_address_of() takes address only of lvalue reference 
// which is safe
#pragma warning(disable:4172)
#endif

BOOST_HANA_CONSTEXPR_LAMBDA auto iterate_exists =[](auto&& obj,auto&& key)
{
    if (obj && contains(*obj,key))
    {
        return take_address_of(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key)));
    }
    return decltype(take_address_of(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key))))(nullptr);
};

#ifdef _MSC_VER
#pragma warning(default:4172)
#endif

}

/**
  @brief Check if member at a given path exist in the object
  @param obj Object under validation
  @param path Member path as a tuple
  @return Validation status

  This operation is performed at runtime
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto check_exists =[](auto&& obj,auto&& path)
{
    return hana::fold(std::forward<decltype(path)>(path),&obj,detail::iterate_exists)!=nullptr;
};

/**
  @brief Check if path of types in member descriptor can exist in the object
  @param obj Object under validation
  @param path Member path as a tuple
  @return Validation status

  This operation is expected to be performed statically at compile time.
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto check_member_path =[](auto obj,auto path)
{
    auto path_c=hana::transform(path,hana::make_type);
    auto obj_c=hana::type_c<decltype(obj)>;

    return hana::is_nothing(hana::monadic_fold_left<hana::optional_tag>(path_c,obj_c,hana::sfinae(check_member)));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP
