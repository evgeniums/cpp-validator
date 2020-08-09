/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/hana_to_std_tuple.hpp
*
*  Defines converter of hana::tuple to std::tuple.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP
#define DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/make_types_tuple.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Wrapper for making std::tuple.
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto make_std_tuple= [] (auto&&... args)
{
    return std::make_tuple(std::forward<decltype(args)>(args)...);
};

/**
  @brief Convert hana::tuple to std::tuple.
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto hana_to_std_tuple= [] (auto&& ts)
{
    return hana::unpack(std::forward<decltype(ts)>(ts),make_std_tuple);
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP
