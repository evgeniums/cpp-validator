/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/class_method_args.hpp
*
*  Defines helper to extratct types of class method arguments.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CLASS_METHOD_ARGS_HPP
#define DRACOSHA_VALIDATOR_CLASS_METHOD_ARGS_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper to extract list of argument types of a class/struct method.
 */
template <class FnT>
struct class_method_args
{
    constexpr static auto types()
    {
        return hana::tuple_t<>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...)>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) &>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) &&>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) const>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) const &>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) const &&>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

// Starting from c++17 methods with noexcept specifier are considered being of other types.
#if __cplusplus >= 201703L

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) noexcept>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) & noexcept>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) && noexcept>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) const noexcept>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) const & noexcept>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};

template <typename Ret, typename Cls, typename... Args>
struct class_method_args<Ret (Cls::*)(Args...) const && noexcept>
{
    constexpr static auto types()
    {
        return hana::tuple_t<Args...>;
    }
};
#endif

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CLASS_METHOD_ARGS_HPP
