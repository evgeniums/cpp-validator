/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/property_validator.hpp
*
*  Defines functor for property validation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP
#define DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/dispatcher.hpp>
#include <dracosha/validator/utils/wrap_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct property_validator_tag;

/**
 * @brief Property validator functor.
 */
template <typename Handler, typename PropT>
struct property_validator
{
    using hana_tag=property_validator_tag;
    using property_type=PropT;

    Handler fn;

    template <typename ... Args>
    auto operator () (Args&&... args) const -> decltype(auto)
    {
        return fn(std::forward<Args>(args)...);
    }
};

/**
 * @brief Make property validator functor.
 * @param prop Property.
 * @param args Arguments to forward to validator.
 */
template <typename PropT, typename OpT, typename OperandT>
constexpr auto make_property_validator(PropT&& prop, OpT&& op, OperandT&& operand)
{
    auto fn=hana::reverse_partial(dispatch,std::forward<PropT>(prop),std::forward<OpT>(op),wrap_object(std::forward<OperandT>(operand)));
    return property_validator<decltype(fn),PropT>{std::move(fn)};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTY_VALIDATOR_HPP
