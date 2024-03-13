/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/lazy.hpp
*
*  Defines wrapper for deferred invokation of a handler.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_LAZY_HPP
#define HATN_VALIDATOR_LAZY_HPP

#include <hatn/validator/config.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct lazy_tag;

/**
 * @brief Lazy invokation handler.
 */
template <typename T>
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
  @brief Construct handler for deferred invokation.
  @param fn Handler that will be invoked later on demand.
*/
template <typename T>
auto lazy(T&& fn) -> decltype(auto)
{
    return lazy_t<decltype(fn)>{std::forward<decltype(fn)>(fn)};
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_LAZY_HPP
