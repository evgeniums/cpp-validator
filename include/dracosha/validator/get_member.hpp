/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/get_member.hpp
*
*  Defines extractor of object's member
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GET_MEMBER_HPP
#define DRACOSHA_VALIDATOR_GET_MEMBER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Extract nested member from object
  @param Top level object
  @param Path to the member to extract
  @return Extracted member

  In most cases a reference will be returned except for the cases of properties that return rvalues, e.g. size() or empty().
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto get_member= [](auto&& v, auto&& path) -> decltype(auto)
{
    return hana::fold(std::forward<decltype(path)>(path),std::forward<decltype(v)>(v),
            [](auto&& field, auto&& key) -> decltype(auto)
            {
                return get(std::forward<decltype(field)>(field),std::forward<decltype(key)>(key));
            }
        );
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_MEMBER_HPP