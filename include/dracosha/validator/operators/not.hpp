/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/not.hpp
*
*  Defines logical pseudo operator NOT
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_NOT_HPP
#define DRACOSHA_VALIDATOR_NOT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/operators/and.hpp>
#include <dracosha/validator/detail/logical_not.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Logical pseudo operator NOT
 * @param op Intermediate validator whose result must be negated
 * @return Logical "not" of intermediate validator result
 *
 */
struct not_t
{
    template <typename ... Ops>
    constexpr auto operator() (Ops&&... ops) const
    {
        return (*this)(AND(std::forward<Ops>(ops)...));
    }

    template <typename OpT>
    constexpr auto operator() (OpT&& op) const
    {
        return make_validator(
                    hana::reverse_partial(
                        detail::logical_not,
                        std::forward<OpT>(op)
                    )
               );
    }
};
constexpr not_t NOT{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_NOT_HPP
