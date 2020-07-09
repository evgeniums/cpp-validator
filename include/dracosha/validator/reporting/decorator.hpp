/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/decorator.hpp
*
* Defines decorator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DECORATOR_HPP
#define DRACOSHA_VALIDATOR_DECORATOR_HPP

#include <string>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct decorator_tag;

/**
 * @brief Default decorator class that bypasses string as is.
 */
struct no_decorator_t
{
    using hana_tag=decorator_tag;

    template <typename T>
    auto operator () (T&& val) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(val));
    }
};
constexpr no_decorator_t no_decorator{};

/**
 * @brief Helper to work with object that can optionally have or not have decorators
 *
 * Version for object that does not have decorator
 */
template <typename T, typename Arg, typename =void>
struct decorate_t
{
    template <typename T1, typename Arg1>
    auto operator() (T1&&, Arg1&& arg) const -> decltype(auto)
    {
        return hana::id(std::forward<Arg1>(arg));
    }
};
/**
 * @brief Helper to work with object that can optionally have or not have decorators
 *
 * Version for object that has decorator
 */
template <typename T, typename Arg>
struct decorate_t<T,Arg,
        decltype(
            (void)std::declval<std::decay_t<T>>().decorator(std::declval<std::decay_t<Arg>>())
        )>
{
    template <typename T1, typename Arg1>
    auto operator() (T1&& obj, Arg1&& arg) const -> decltype(auto)
    {
        return obj.decorator(std::forward<Arg1>(arg));
    }
};

/**
 * @brief Helper to work with object that can optionally have or not have decorators
 */
template <typename T, typename Arg>
constexpr decorate_t<T,Arg> decorate{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DECORATOR_HPP
