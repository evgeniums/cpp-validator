/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/check_member_path.hpp
*
*  Defines helpers for checking existance of member path in type of an object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CHECK_MEMBER_PATH_HPP
#define DRACOSHA_VALIDATOR_CHECK_MEMBER_PATH_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/check_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Check if path of types in member descriptor can exist in the object.
  @param obj Object under validation.
  @param path Member path as a tuple.
  @return Validation status.

  This operation is expected to be static and assumed to be performed at compile time.
*/
template <typename Tobj, typename Tpath>
auto check_member_path(Tobj&& obj, Tpath&& path)
{
    auto path_c=hana::transform(path,extract_object_wrapper_type_c);
    auto obj_c=hana::type_c<decltype(obj)>;

    return !hana::is_nothing(hana::monadic_fold_left<hana::optional_tag>(path_c,obj_c,hana::sfinae(check_member)));
}

/**
  * @brief Check if members have paths of the same types.
  * @param member1 First member.
  * @param Second member.
  * @return Result of check operation.
  */
template <typename Tm1, typename Tm2>
auto check_member_path_types(const Tm1& member1,const Tm2& member2)
{
    auto path1_c=hana::transform(member1.path(),extract_object_wrapper_type_c);
    auto path2_c=hana::transform(member2.path(),extract_object_wrapper_type_c);

    return hana::equal(path1_c,path2_c);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CHECK_MEMBER_PATH_HPP
