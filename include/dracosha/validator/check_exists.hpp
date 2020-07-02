/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/check_exists.hpp
*
*  Defines helpers for checking existance of member in an object
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP
#define DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/get.hpp>
#include <dracosha/validator/detail/take_address_of.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------


#ifdef _MSC_VER
// disable warning about taking address of temporary variable 
// because take_address_of() takes address only of lvalue reference 
// which is safe
#pragma warning(disable:4172)
#endif

/**
  @brief Extract member from object
  @param obj Object under test
  @param key Key for member lookup
  @return Operation result, see comments below.

    If member is found and is of lvalue reference type then address of that member will be returned.
    If member is found and is not of lvalue reference type then address of stub object will be returned to avoid taking addresses of temporary objects.
    If member is not found then nullptr will be returned.
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto get_member_ptr =[](auto&& obj,auto&& key)
{
    if (obj && contains(*obj,key))
    {
        return detail::take_address_of<decltype(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key)))>
                (
                        get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key))
                    );
    }
    return decltype(
                detail::take_address_of<decltype(get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key)))>
                                (
                                        get(std::forward<decltype(*obj)>(*obj),std::forward<decltype(key)>(key))
                                    )
                )(nullptr);
};

#ifdef _MSC_VER
#pragma warning(default:4172)
#endif

/**
  @brief Check if member at a given path exist in the object
  @param obj Object under validation
  @param path Member path as a tuple
  @return Validation status

  This operation is performed at runtime
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto check_exists =[](auto&& obj,auto&& path)
{
    return hana::fold(std::forward<decltype(path)>(path),&obj,get_member_ptr)!=nullptr;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_EXISTS_HPP