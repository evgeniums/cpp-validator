/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/prepare_dispatcher.hpp
*
*  Defines helpers for validation preparing
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREPARE_DISPATCHER_HPP
#define DRACOSHA_VALIDATOR_PREPARE_DISPATCHER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Dispatch validation
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto dispatch = [](auto&&... args) -> decltype(auto)
{
    return dispatcher.invoke(std::forward<decltype(args)>(args)...);
};

/**
 * @brief Prepare validation dispatcher
 */
struct prepare_dispatcher_t
{
    template <typename ... Args>
    constexpr auto operator() (Args&&... args) const -> decltype(auto)
    {
        return hana::reverse_partial(dispatch,std::forward<Args>(args)...);
    }
};
constexpr prepare_dispatcher_t prepare_dispatcher{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREPARE_DISPATCHER_HPP
