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

#ifndef HATN_VALIDATOR_GET_MEMBER_HPP
#define HATN_VALIDATOR_GET_MEMBER_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/get.hpp>
#include <hatn/validator/utils/hana_to_std_tuple.hpp>
#include <hatn/validator/utils/pointer_as_reference.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct get_member_impl
{
    template <typename Tv, typename Tpath>
    auto operator () (Tv&& v, Tpath&& path) const -> decltype(auto)
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
        )(std::forward<Tv>(v),std::forward<Tpath>(path));
    }
};
constexpr get_member_impl get_member_inst{};

/**
  @brief Extract nested member from object.
  @param Top level object.
  @param Path to the member to extract.
  @return Extracted member.

  In most cases a reference will be returned except for the cases of properties that return rvalues, e.g. size() or empty().
*/
struct get_member_as_reference_impl
{
    template <typename Tv, typename Tpath>
    auto operator () (Tv&& v, Tpath&& path) const -> decltype(auto)
    {
        return as_reference(get_member_inst(std::forward<Tv>(v),std::forward<Tpath>(path)));
    }
};
constexpr get_member_as_reference_impl get_member{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_GET_MEMBER_HPP
