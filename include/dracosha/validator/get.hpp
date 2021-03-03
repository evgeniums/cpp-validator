/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/get.hpp
*
*  Defines function for getting member from object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GET_HPP
#define DRACOSHA_VALIDATOR_GET_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/get_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
  @brief Get member/element from object.
  @param v Object.
  @param k Key of the member/element.
  @return Either reference to member's value or rvalue that was returned by property. It depends on the object and member implementation.

  If member can not be found by the key then behaviour depends on the object and member implementation,
  usually a kind of out_of_range exception will be thrown or some default value will be returned.
*/
template <typename Tv, typename Tk>
auto get(Tv&& v, Tk&& k) -> decltype(auto)
{
    return detail::get_impl<decltype(v),decltype(extract_object_wrapper(std::forward<Tk>(k)))>
            (
                std::forward<decltype(v)>(v),
                extract_object_wrapper(std::forward<Tk>(k))
             );
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_HPP
