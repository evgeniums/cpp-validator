/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/invoke_or.hpp
*
*  Defines helpers to sequantially call each callable in a tupple with OR disjunction.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_INVOKE_OR_HPP
#define DRACOSHA_VALIDATOR_INVOKE_OR_HPP

#include <functional>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//---------------------------------------------------------------

/**
 * @brief Traits for OR disjunction where result of each callable invovation is checked as bool.
 */
struct invoke_or_default_traits
{
    template <typename T>
    static bool check(T&& ret) noexcept
    {
        return ret==true;
    }
};

/**
 * @brief Traits for OR disjunction where result of each callable invovation is checked as inverted bool.
 */
struct invoke_or_inverse_traits
{
    template <typename T>
    static bool check(T&& ret) noexcept
    {
        return ret==false;
    }
};

#if __cplusplus >= 201703L

/**
 * @brief Functor implementing invoke_or.
 */
template <typename TraitsT>
struct invoke_or_t
{
    template <typename ...FnT>
    auto operator () (FnT&&... fns) const
    {
        // get return type of the first callable which must be the same for all callables
        using retT=decltype(std::declval<std::tuple_element_t<0,std::tuple<FnT...>>>()());

        // wrap invocation of a callable into lambda in order to keep return result
        retT ret;
        auto each=[&ret](auto&& fn)
        {
            ret=fn();
            return TraitsT::check(ret);
        };

        // fold invocations of callables with &&
        std::ignore=(std::invoke(each,std::forward<FnT>(fns)) || ...);

        // return result
        return ret;
    }
};

#else

namespace detail
{
template <typename TraitsT,typename ...FnT>
struct invoke_or_impl
{
};

template <typename TraitsT,typename FnT>
struct invoke_or_impl<TraitsT,FnT>
{
    template <typename Fn>
    static auto f(Fn&& fn)
    {
        return fn();
    }
};

template <typename TraitsT, typename FnT0, typename ...FnT>
struct invoke_or_impl<TraitsT,FnT0,FnT...>
{
    template <typename Fn0, typename ...Fn>
    static auto f(Fn0&& fn, Fn&&... fns)
    {
        auto ret=fn();
        if (TraitsT::check(ret))
        {
            return ret;
        }
        return invoke_or_impl<TraitsT,FnT...>::f(std::forward<Fn>(fns)...);
    }
};
}

/**
 * @brief Functor implementing invoke_or.
 */
template <typename TraitsT=invoke_or_default_traits>
struct invoke_or_t
{
    template <typename ...FnT>
    auto operator() (FnT&&... fn) const
    {
        return detail::invoke_or_impl<TraitsT,std::decay_t<FnT>...>::f(std::forward<FnT>(fn)...);
    }
};

#endif

/**
 * @brief Invoke callables one by one using OR disjunction.
 * @param fns Callables. All callables must return the same type that must be convertible to bool.
 * @return Accumulated return result of callables invocations.
 *
 * TraitsT must define static bool check(T&& ret) method for convertion result to bool that will be used for chain termintation.
 */
template <typename TraitsT>
constexpr invoke_or_t<TraitsT> invoke_or_configurable{};

/**
 * @brief Invoke callables one by one using OR disjunction where result of each callable is checked as bool.
 * @param fns Callables. All callables must return the same type that must be convertible to bool.
 * @return Accumulated return result of callables invocations.
 */
constexpr decltype(invoke_or_configurable<invoke_or_default_traits>) invoke_or{};

/**
 * @brief Invoke callables one by one using OR disjunction where result of each callable is checked as inverted bool.
 * @param fns Callables. All callables must return the same type that must be convertible to bool.
 * @return Accumulated return result of callables invocations.
 */
constexpr decltype(invoke_or_configurable<invoke_or_inverse_traits>) invoke_or_inverse{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#ifndef DCS_FN
// wrap operation into lambda
#define DCS_FN(fn) [&](){return fn;}
#endif

#endif // DRACOSHA_VALIDATOR_INVOKE_AND_HPP
