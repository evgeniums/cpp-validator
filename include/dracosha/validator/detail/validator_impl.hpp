/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/validator_impl.hpp
*
*  Defines implementation of validator
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
#define DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/apply.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Implementation of validator
 *
 * This struct is just a container that holds validating executer and has apply() method that is to be called to apply validation
 * using mentioned executor.
 */
template <typename Handler>
struct validator_impl
{
    using hana_tag=validator_tag;

    Handler fn;

    /**
     * @brief Ctor
     * @param fn Validation handler that will be called to perform validating
     */
    validator_impl(Handler fn):fn(std::move(fn))
    {
    }

    /**
     * @brief Invoke validating with supplied args
     */
    template <typename ... Args>
    auto apply(Args&&... args) const -> decltype(auto)
    {
        return fn(std::forward<Args>(args)...);
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
