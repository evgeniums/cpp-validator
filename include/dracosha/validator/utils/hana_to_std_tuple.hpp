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
struct make_std_tuple_t
{
    template <typename ...Args>
    constexpr auto operator () (Args&&... args) const
    {
        return std::make_tuple(std::forward<decltype(args)>(args)...);
    }
};
constexpr make_std_tuple_t make_std_tuple{};

/**
  @brief Convert hana::tuple to std::tuple.
  */
template <typename Ts>
auto hana_to_std_tuple(Ts&& ts)
{
    return hana::unpack(std::forward<decltype(ts)>(ts),make_std_tuple);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HANA_TO_STD_TUPLE_HPP
