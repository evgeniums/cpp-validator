/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/get_impl.hpp
*
*  Defines utilities for getting member from object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GET_IMPL_HPP
#define DRACOSHA_VALIDATOR_GET_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/can_get.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace detail
{
template <typename T1, typename T2, typename=hana::when<true>>
struct get_t
{
};
/**
 * Get as property
 */
template <typename T1, typename T2>
struct get_t<T1,T2,hana::when<can_get<T1,T2>.property()>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
        return property(std::forward<T1>(v),std::forward<T2>(k));
    }
};

/**
 * Get using at(key) method
 */
template <typename T1, typename T2>
struct get_t<T1,T2,hana::when<can_get<T1,T2>.at() && !can_get<T1,T2>.property()>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
      return v.at(std::forward<T2>(k));
    }
};

/**
 * Get using [key] operator
 */
template <typename T1, typename T2>
struct get_t<T1,T2,hana::when<can_get<T1,T2>.brackets() && !can_get<T1,T2>.at() && !can_get<T1,T2>.property()>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
      return v[std::forward<T2>(k)];
    }
};

/**
 * Traits for getting member from object of type T1 using key of type T2
 */
template <typename T1, typename T2>
constexpr get_t<T1,T2> get_impl{};
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_IMPL_HPP
