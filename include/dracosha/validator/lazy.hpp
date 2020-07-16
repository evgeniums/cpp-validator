/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/lazy.hpp
*
*  Defines wrapper for deferred invokation of a handler
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_LAZY_HPP
#define DRACOSHA_VALIDATOR_LAZY_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct lazy_tag;
template <typename T>

/**
 * @brief Lazy invokation handler
 */
struct lazy_t
{
    using hana_tag=lazy_tag;
    T fn;

    auto operator()() const -> decltype(fn())
    {
        return fn();
    }
};

/**
  @brief Construct handler for deferred invokation
  @param fn Handler that will be invoked later on demand
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto lazy = [](auto&& fn)
{
    return lazy_t<decltype(fn)>{std::forward<decltype(fn)>(fn)};
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_LAZY_HPP
