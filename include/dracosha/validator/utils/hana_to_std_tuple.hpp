/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/hana_to_std_tuple.hpp
*
*  Defines converter of hana::tuple to std::tuple.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP
#define DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP

#include <boost/hana/ext/std/tuple.hpp>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Convert hana::tuple to std::tuple.
  */
template <typename Ts>
auto hana_to_std_tuple(Ts&& ts)
{
    return hana::to<hana::ext::std::tuple_tag>(std::forward<Ts>(ts));
}

/**
  @brief Convert std::tuple to hana::tuple.
  */
template <typename Ts>
auto std_to_hana_tuple(Ts&& ts)
{
    return hana::to_tuple(std::forward<Ts>(ts));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP
