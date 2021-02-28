/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/get_member.hpp
*
*  Defines extractor of object's member.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GET_MEMBER_HPP
#define DRACOSHA_VALIDATOR_GET_MEMBER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/get.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Extract nested member from object.
  @param Top level object.
  @param Path to the member to extract.
  @return Extracted member.

  In most cases a reference will be returned except for the cases of properties that return rvalues, e.g. size() or empty().
*/
template <typename Tv, typename Tpath>
auto get_member(Tv&& v, Tpath&& path) -> decltype(auto)
{
    return hana::if_(
         hana_tuple_empty<Tpath>{},
         [](auto&& v, auto&&) -> decltype(auto)
         {
            // empty path means object itself
            return hana::id(std::forward<decltype(v)>(v));
         },
         [](auto&& v, auto&& path) -> decltype(auto)
         {
            return hana::fold(std::forward<decltype(path)>(path),std::forward<decltype(v)>(v),
                    [](auto&& field, auto&& key) -> decltype(auto)
                    {
                        return get(std::forward<decltype(field)>(field),std::forward<decltype(key)>(key));
                    }
                );
         }
    )(std::forward<decltype(v)>(v),std::forward<decltype(path)>(path));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_MEMBER_HPP
