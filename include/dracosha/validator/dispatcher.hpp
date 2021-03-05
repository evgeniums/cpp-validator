/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/dispatcher.hpp
*
*  Defines validation dispatcher.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DISPATCHER_HPP
#define DRACOSHA_VALIDATOR_DISPATCHER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/detail/dispatcher_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Dispatcher that dispatches validating requests to corresponding methods of supplied adapter.
 */
struct dispatcher_t
{
    /**
     *  @brief Perform validation of object.
     *  @param a Object to validate or adapter.
     *  @param args Validation arguments.
     *  @return Validation status.
     */
    template <typename T1, typename ...Args>
    status operator() (T1&& a, Args&&... args) const
    {
        return detail::dispatcher_impl<T1>(std::forward<T1>(a),std::forward<Args>(args)...);
    }

    /**
     *  @brief Perform validation of object's property at one level.
     *  @param prop Property to validate.
     *  @param a Object to validate or adapter.
     *  @param op Operator for validation.
     *  @param b Sample argument for validation.
     *  @return Validation status.
     */
    template <typename T1, typename T2, typename OpT, typename PropT>
    status operator() (PropT&& prop, T1&& a, OpT&& op, T2&& b) const
    {
        return invoke(std::forward<T1>(a),std::forward<PropT>(prop),std::forward<OpT>(op),std::forward<T2>(b));
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting.
     *  @param a Object to validate or adapter.
     *  @param args Validation arguments.
     *  @return Validation status.
     */
    template <typename T1, typename ...Args>
    static status invoke(T1&& a, Args&&... args)
    {
        return detail::dispatcher_impl<T1>.invoke(std::forward<T1>(a),std::forward<Args>(args)...);
    }

    /**
     * @brief Execute validators on object and aggregate their results using logical AND.
     * @param a Object to validate or adapter.
     * @param args Validation arguments.
     * @return Logical AND of results of intermediate validators.
     */
    template <typename T1, typename ...Args>
    static status validate_and(T1&& a, Args&&... args)
    {
        return detail::dispatcher_impl<T1>.validate_and(std::forward<T1>(a),std::forward<Args>(args)...);
    }

    /**
     * @brief Execute validators on object and aggregate their results using logical OR.
     * @param a Object to validate or adapter.
     * @param args Validation arguments.
     * @return Logical OR of results of intermediate validators.
     */
    template <typename T1, typename ...Args>
    static status validate_or(T1&& a, Args&&... args)
    {
        return detail::dispatcher_impl<T1>.validate_or(std::forward<T1>(a),std::forward<Args>(args)...);
    }

    /**
     * @brief Execute validator on object and negate its result using logical NOT.
     * @param a Object to validate or adapter.
     * @param args Validation arguments.
     * @return Logical NOT of result of intermediate validator.
     */
    template <typename T1, typename ...Args>
    static status validate_not(T1&& a, Args&&... args)
    {
        return detail::dispatcher_impl<T1>.validate_not(std::forward<T1>(a),std::forward<Args>(args)...);
    }
};

/**
  @brief Instance of validation dispatcher.
  */
constexpr dispatcher_t dispatcher{};
constexpr dispatcher_t validate_inline{};

/**
 * @brief Validation dispatching functor.
 */
struct dispatch_t
{
    template <typename ...Args>
    constexpr auto operator () (Args&&... args) const
    {
        return dispatcher.invoke(std::forward<decltype(args)>(args)...);
    }
};

/**
  @brief Callable for validation dispatching.
  */
constexpr dispatch_t dispatch{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DISPATCHER_HPP
