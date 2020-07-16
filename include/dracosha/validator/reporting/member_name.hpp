/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/member_name.hpp
*
* Defines member name wrapper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_NAME_HPP
#define DRACOSHA_VALIDATOR_MEMBER_NAME_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct member_name_tag;

/**
 * @brieaf Wrapper of member name
 */
template <typename T>
struct member_name
{
    using hana_tag=member_name_tag;

    /**
     * @brief Get constant reference
     * @return Constance reference to value
     */
    const T& get() const
    {
        return _v.get();
    }

    /**
     * @brief Get constant reference
     * @return Constant reference to value
     */
    operator const T& () const
    {
        return _v.get();
    }

    reference_wrapper<T> _v;
};

/**
  @brief Make a member name wrapper from id
  @param v ID to wrap into member name
  @return Member name
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto make_member_name =[](const auto& v)
{
    return member_name<typename decltype(cref(v))::type>{cref(v)};
};

//-------------------------------------------------------------
/**
 * @brief String to use for joining nested member names in member's path
 */
struct string_member_name_conjunction_t : public enable_to_string<string_member_name_conjunction_t>
{
    constexpr static const char* description=" of ";
};
constexpr string_member_name_conjunction_t string_member_name_conjunction{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_NAME_HPP
