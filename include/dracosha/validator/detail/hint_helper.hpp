/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/hint_helper.hpp
*
*  Defines helpers to invoke hint methods.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HINT_HELPER_HPP
#define DRACOSHA_VALIDATOR_HINT_HELPER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace detail
{

//-------------------------------------------------------------

/**
 * @brief Default helper to invoke hint_before() method.
 */
template <typename T, typename HintT, typename=void>
struct hint_before_t
{
    template <typename ... Args>
    status operator() (Args&&...) const
    {
        return status();
    }
};

/**
 * @brief Helper to invoke hint_before() method.
 */
template <typename T, typename HintT>
struct hint_before_t<T,HintT,
        decltype( (void)std::declval<std::decay_t<T>>().hint_before(std::declval<std::decay_t<HintT>>()))
        >
{
    template <typename T1, typename T2>
    status operator() (T1&& v, T2&& x) const
    {
        return v.hint_before(std::forward<T2>(x));
    }
};

/**
 * @brief Instance of helper to invoke hint_before() method.
 */
template <typename T, typename HintT>
constexpr hint_before_t<T,HintT> hint_before{};

/**
 * @brief Default helper to invoke hint_after() method.
 */
template <typename T, typename HintT, typename=void>
struct hint_after_t
{
    template <typename T1, typename T2>
    status operator() (T1&&, status validation_status, T2&&) const
    {
        return validation_status;
    }
};

/**
 * @brief Helper to invoke hint_after() method.
 */
template <typename T, typename HintT>
struct hint_after_t<T,HintT,
        decltype( (void)std::declval<std::decay_t<T>>().hint_after(std::declval<status>(),std::declval<std::decay_t<HintT>>()))
        >
{
    template <typename T1, typename T2>
    status operator() (T1&& v, status validation_status, T2&& x) const
    {
        return v.hint_after(validation_status,std::forward<T2>(x));
    }
};

/**
 * @brief Instance of helper to invoke hint_after() method.
 */
template <typename T, typename HintT>
constexpr hint_after_t<T,HintT> hint_after{};

//-------------------------------------------------------------

}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HINT_HELPER_HPP
